/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
import android.util.SparseArray;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Locale;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

/**
 * AudioTrackPlayer.
 *
 * @since 2018-07-05
 */
public class AudioTrackPlayer {
    /** return when execution succeeds */
    public static final int VMI_SUCCESS = 0;

    /** return when receiving msg fails */
    public static final int VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL = 0x0A030001;

    /** return when starting player fails */
    public static final int VMI_AUDIO_ENGINE_CLIENT_START_FAIL = 0x0A030002;

    /** return when stopping player fails */
    public static final int VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL = 0x0A030003;

    /** return when registering data send hook fails */
    public static final int VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL = 0x0A030004;

    /** return when registering data save hook fails */
    public static final int VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL = 0x0A030005;

    /** track map size */
    public static final int TRACK_MAP_SIZE = 32;

    /** audio massage len */
    public static final int AUDIO_MSG_LEN = 30;

    /** short to bytes */
    public static final int SHORT_TO_BYTES = 2;

    /** delta bottom */
    public static final double DELTA_BOTTOM = 1e6D;

    /** minimum audio stream type */
    public static final int AUDIO_STREAM_TYPE_MIN = -1;
 
    /** maximum audio stream type */
    public static final int AUDIO_STREAM_TYPE_MAX = 13;
 
    /** minimum sample rate in HZ */
    public static final int SAMPLE_RATE_HZ_MIN = 4000;
 
    /** maximum sample rate in HZ */
    public static final int SAMPLE_RATE_HZ_MAX = 192000;
    
    /** audioTrack state of play */
    public static final int PLAYSTATE_PLAYING = 3;

    /** Default params of AduioTrack */
    public static final int DEFAULT_STREAM_TYPE = 3;
    public static final int DEFAULT_SAMPLE_RATE_IN_HZ = 48000;
    public static final int DEFAULT_CHANNEL_CONFIG = 12;
    public static final int DEFAULT_AUDIO_FORMAT = 2;
    public static final int DEFAULT_BUFFER_SIZE_IN_BYTES_200MS = 46128;
    public static final int DEFAULT_AUDIO_STREAM_MODE = 1;

    private static final AudioTrackPlayer INSTANCE = new AudioTrackPlayer();
    private static final SparseArray<VmiAudioPlayer> TRACK_MAP = new SparseArray<>(TRACK_MAP_SIZE);
    private static final String TAG = "AudioTrackPlayer";
    private static final int AUDIO_TYPE_OPUS = 0;
    private static final int AUDIO_TYPE_PCM = 1;
    private static final int Q_SIZE_LIMIT = 80; // audio data queue size limit
    private static final int BYTE_SIZE_IN_BIT = 8;
    private static final int AUDIO_SAMPLE_S_TO_MS = 1000;
    private static final int AUDIO_SAMPLE_S_TO_200MS = 5;
    private static int audioType = AUDIO_TYPE_OPUS;
    private static int pid = 0;
    private static volatile boolean isMute = false; // mute audio when running in background

    private AUDIOSENDHOOK audioSendhook;
    private AUDIOSAVEHOOK audioSavehook;
    private final AudioClientData audioClientData;
    private final AudioRemoteMessage msg = new AudioRemoteMessage(AUDIO_MSG_LEN * AudioRemoteMessage.SIZE_OF_INT);

    private AudioTrackPlayer() {
        this.audioClientData = new AudioClientData();
    }

    /**
     * get AudioTrackPlayer singleton
     *
     * @return AudioTrackPlayer
     */
    public static AudioTrackPlayer getInstance() {
        return INSTANCE;
    }

    /**
     * register audio send hook.
     *
     * @param hook AUDIOSENDHOOK
     * @return succ: VMI_SUCCESS
     *         fail: VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL
     */
    public int registerAudioSendHook(AUDIOSENDHOOK hook) {
        Log.i(TAG, "Security Audit: registerSendFunc");
        if (hook == null) {
            Log.e(TAG, "Failed to register audio client send hook, hook is null");
            return VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL;
        }
        audioSendhook = hook;
        return VMI_SUCCESS;
    }

    /**
     * start audioplayer according to pid
     *
     * @param pid pid of videoPlayer
     * @return boolean
     */
    private boolean startAudioPlayer(int pid) {
        // execute a new thread while pid not in TRACK_MAP or already stopped.
        VmiAudioPlayer vmiAudioPlayer = TRACK_MAP.get(pid, null);
        if (vmiAudioPlayer == null) {
            auDebug("new audio track thread:" + pid);
            vmiAudioPlayer = new VmiAudioPlayer(pid);
            int err = vmiAudioPlayer.initAudioPlayer();
            if (err != VMI_SUCCESS) {
                Log.e(TAG, "init vmiAudioPlayer fail");
                return false;
            }
            TRACK_MAP.put(pid, vmiAudioPlayer);
            vmiAudioPlayer.onReceiveMsg(msg);
            return true;
        }
        auDebug("reuse VmiAudioPlayer: " + pid);
        vmiAudioPlayer.onReceiveMsg(msg);
        return true;
    }

    /**
     * register audio save hook.
     *
     * @param hook AUDIOSAVEHOOK
     * @return succ：VMI_SUCCESS
     *         fail：VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL
     */
    public int registerAudioSaveHook(AUDIOSAVEHOOK hook) {
        Log.i(TAG, "Security Audit: registerSaveFunc");
        if (hook == null) {
            Log.e(TAG, "Failed to register audio client save hook, hook is null");
            return VMI_AUDIO_CLIENT_SAVEHOOK_REGISTER_FAIL;
        }
        audioSavehook = hook;
        return VMI_SUCCESS;
    }

    /**
     * public interface to process audio data packet.
     *
     * @param data   The received audio data packet.
     * @param length The byte length of audio data packet.
     * @return VMI_SUCCESS or VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL.
     */
    public int onRecvAudioPacket(byte[] data, int length) {
        final int maxPkgLen = 4096; // maximum packet length
        if (data == null || length != data.length || length < AudioRemoteMessage.SIZE_OF_INT || length > maxPkgLen) {
            Log.e(TAG, "wrong audio msg, input length:" + length + " , data length:" + data.length);
            return VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL;
        }

        // 此处将获取的连续四个字节数据左移24， 16， 8后拼接成32位数据
        int ret;
        int cmd = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
        if (cmd == AudioRemoteMessage.RETURN_AUDIO_PLAY_DATA) {
            ret = msg.init(data, AudioRemoteMessage.AUDIO_DATA_MSG_LEN);
            if (ret < 0) {
                Log.e(TAG, "init msg failed");
                return VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL;
            }
            msg.setPayload(data, AudioRemoteMessage.AUDIO_DATA_MSG_LEN);
        }
        pid = 0;
        if (!startAudioPlayer(pid)) {
            return VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL;
        }
        return VMI_SUCCESS;
    }

    /**
     * unmute audio.
     *
     * @return succ：VMI_SUCCESS
     *         fail：VMI_AUDIO_ENGINE_CLIENT_START_FAIL
     */
    public static int startAudio() {
        Log.i(TAG, "Security Audit: startAudio");
        int ret = setMute(false);
        if (ret != 0) {
            Log.e(TAG, "Failed to start audio, fail to set mute, err=" + ret);
            return VMI_AUDIO_ENGINE_CLIENT_START_FAIL;
        }
        return VMI_SUCCESS;
    }

    /**
     * mute audio.
     *
     * @return succ：VMI_SUCCESS
     *         fail：VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL
     */
    public static int stopAudio() {
        Log.i(TAG, "Security Audit: stopAudio");
        int ret = setMute(true);
        if (ret != 0) {
            return VMI_AUDIO_ENGINE_CLIENT_STOP_FAIL;
        }
        return VMI_SUCCESS;
    }

    private void stopPlayThread() {
        if (TRACK_MAP != null) {
            for (int i = 0; i < TRACK_MAP.size(); i++) {
                VmiAudioPlayer vmiAudioPlayer = TRACK_MAP.valueAt(i);
                if (vmiAudioPlayer == null || vmiAudioPlayer.playThread == null) {
                    continue;
                }
                TRACK_MAP.valueAt(i).isPlayThreadAlived = false;
                TRACK_MAP.valueAt(i).playThread.interrupt();
            }
        }
    }

    /**
     * set audio mute status
     *
     * @param shouldMute true to mute audio, false to unmute audio
     * @return 0:SUCC -1:FAIL
     */
    private static int setMute(boolean shouldMute) {
        if (TRACK_MAP == null) {
            Log.e(TAG, "set mute failed, TRACK_MAP is null");
            return -1;
        }
        isMute = shouldMute;
        for (int i = 0; i < TRACK_MAP.size(); i++) {
            if (TRACK_MAP.valueAt(i) == null) {
                continue;
            }
            if (shouldMute) {
                TRACK_MAP.valueAt(i).mute();
            } else {
                TRACK_MAP.valueAt(i).unMute();
            }
        }
        return 0;
    }

    @SuppressLint("PrivateApi")
    private static String getProperty(String key) {
        String val = "0";
        try {
            Class<?> className = Class.forName("android.os.SystemProperties");
            Method get = className.getMethod("get", String.class, String.class);
            Object obj = get.invoke(className, key, "unknown");
            if (obj instanceof String) {
                val = (String) obj;
            }
        } catch (ClassNotFoundException | NoSuchMethodException | InvocationTargetException
                | IllegalAccessException e) {
            Log.e(TAG, " get system property error", e);
        }

        return val;
    }

    private static void auDebug(String msg) {
        String val = getProperty("audio.debug");
        if ("1".equals(val)) {
            Log.i(TAG, msg);
        }
    }

    class VmiAudioPlayer {
        private static final String TAG = "VmiAudioPlayer";
        private boolean isPlayThreadAlived = false;
        private final int pid;
        private int mode = AudioTrack.MODE_STREAM;
        private int bufferLenPerSecond = 0;
        private int bufferSizeInBytes = 0;
        private final int bufferSize = 3840; // buffer size
        private final int logPrintInterval = 200; // log print interval
        private final int minQueueSizeDefault = 1000; // defualt minimum queue size
        private int logIdx = 0;
        private int saveAudioLogIdx = 0;
        private int min = 0; // minimal play queue size
        private int max = 0; // maximal play queue size
        private int poolIndex = 0;
        private int audioStreamType = 0;
        private int sampleRate = 0;
        private int audioPcmChannelConfig = 0;
        private int audioPcmChannelCount = 0;
        private int audioPcmSampleSize = 0;
        private int audioDataFormat = 0;
        private float volumeValue = 50F; // 50F: default volume
        private long decoder = 0L;
        private long startTimeStamp = 0L;
        private long audioClientTS = 0L;
        private long lastFeedTime = 0L;

        private final BlockingDeque<BufferContent> playBuffer = new LinkedBlockingDeque<>();
        private final BufferContent[] bufferPool = new BufferContent[Q_SIZE_LIMIT];
        private final Semaphore semPcm = new Semaphore(0);
        private AudioPlayerThread playThread;
        private AudioTrack trackPlayer = null;

        private static final int PARAM_UNCHANGE = 0;
        private static final int PARAM_CHANGE = 1;
        private static final int PARAM_ERROR = -1;
        private AudioTrackParam params = new AudioTrackParam();

        public VmiAudioPlayer(int pid) {
            this.pid = pid;
        }

        class BufferContent {
            int dataLen;
            short[] data = new short[bufferSize >> 1];

            void setLength(int len) {
                dataLen = len;
            }

            short[] getData() {
                return data;
            }

            int getLength() {
                return dataLen;
            }
        }

        /**
         * handling audio message
         *
         * @param msg AudioRemoteMessage
         */
        public void onReceiveMsg(AudioRemoteMessage msg) {
            if (msg == null) {
                Log.e(TAG, "Invalid AudioRemoteMessage.");
                return;
            }
            int type = msg.readInt();
            switch (type) {
                case AudioRemoteMessage.MSG_START:
                    handleStart();
                    break;
                case AudioRemoteMessage.RETURN_AUDIO_PLAY_DATA:
                    handleWrite(msg);
                    break;
                case AudioRemoteMessage.MSG_STOP:
                    handleStop();
                    break;
                case AudioRemoteMessage.MSG_DESTRUCT:
                    handleDestruct();
                    break;
                case AudioRemoteMessage.SET_CLIENT_VOLUME:
                    handleSetVolume(msg);
                    break;
                default:
                    Log.e(TAG, "Can not resolve msg type: " + type);
            }
        }

        /**
         * mute.
         */
        public void mute() {
            auDebug("mute(), trackPlayer is: " + trackPlayer);

            if (trackPlayer != null) {
                trackPlayer.setVolume(0);
            }
        }

        /**
         * unMute.
         */
        public void unMute() {
            auDebug("unMute(), trackPlayer is: " + trackPlayer);
            if (trackPlayer != null) {
                trackPlayer.setVolume(volumeValue);
            }
        }

        private int initAudioPlayer() {
            try {
                trackPlayer = new AudioTrack(DEFAULT_STREAM_TYPE, DEFAULT_SAMPLE_RATE_IN_HZ, DEFAULT_CHANNEL_CONFIG,
                    DEFAULT_AUDIO_FORMAT, DEFAULT_BUFFER_SIZE_IN_BYTES_200MS, DEFAULT_AUDIO_STREAM_MODE);
                } catch (IllegalArgumentException e) {
                    Log.e(TAG, "Failed to init audio player, failed to new audioTrack");
                    auDebug("IllegalArgumentException");
                }
            handleStart();
            startPlayThread();
            for (int i = 0; i < Q_SIZE_LIMIT; i++) {
                bufferPool[i] = new BufferContent();
            }
            return VMI_SUCCESS;
        }

        private void feedSize(int size) {
            final int nanoPerMs = 1000000; // milliseconds to nanoseconds
            long now = System.nanoTime() / nanoPerMs;
            final int sleepInterval = 40; // sleep interval in nanoseconds
            if (now - lastFeedTime < sleepInterval) {
                return;
            }
            int maxQueueSizeThreshold = 20; // max queue size threshold
            int minQueueSizeThreshold = 10; // min queue size threshold
            if ((size >= 0) && (size <= minQueueSizeThreshold) || (size > maxQueueSizeThreshold)) {
                lastFeedTime = now;
            }
            if (size > max) {
                max = size;
            }
            if (size < min) {
                min = size;
            }
            if (logIdx == Integer.MAX_VALUE) {
                logIdx = 0;
            }
            if (((++logIdx) % logPrintInterval) == 0) {
                Log.i(TAG, "playQ.size() = " + size + " max = " + max + " min = " + min);
                min = minQueueSizeDefault;
                max = 0;
            }
        }

        private void startPlayThread() {
            // playBuffer.clear() 通常伴随着 index和 信号量清除动作；
            playBuffer.clear();
            poolIndex = 0;
            while (semPcm.tryAcquire()) { // 此处为清空已有的信号量
                Log.d(TAG, "cleaning semPcm");
            }
            playThread = new AudioPlayerThread(playBuffer);
            playThread.setName("AudioPlayerThread");
            isPlayThreadAlived = true;
            playThread.start();
        }

        class AudioPlayerThread extends Thread {
            private static final String LOG_TAG = "AudioPlayerThread";
            private final BlockingDeque<BufferContent> playQ;
            private int controlVar = 0;

            AudioPlayerThread(BlockingDeque<BufferContent> bq) {
                super.setName("AudioPlayerThread");
                playQ = bq;
            }

            private void rateLimitLog(String msg) {
                if (((controlVar++) % logPrintInterval) == 0) {
                    Log.i(LOG_TAG, msg);
                }
            }

            private void playAudioBuffer(BufferContent tmp) {
                if (trackPlayer!=null&&trackPlayer.getPlayState()!=PLAYSTATE_PLAYING){
                    trackPlayer.play();
                }
                short[] playData = tmp.getData();
                int lengthInShort = tmp.getLength();
                trackPlayer.write(playData, 0, lengthInShort);
                rateLimitLog(" write data lengthInShort: " + lengthInShort);
            }

            /**
             * Monbox仿真机客户端不播放音频，调整休眠时间，来代替音频播放
             *
             * @param tmp The buffered audio data
             */
            private void handleAudioBuffer(BufferContent tmp) {
                final int msPerSecond = 1000; // seconds to milliseconds
                try {
                    if (bufferLenPerSecond != 0) {
                        int ms = (tmp.getLength() << 1) * msPerSecond / bufferLenPerSecond;
                        Thread.sleep(ms);
                        rateLimitLog("sleep(" + ms + ")ms instead of performing play.");
                    }
                } catch (InterruptedException e) {
                    Log.e(LOG_TAG, " handleAudioBuffer error", e);
                }
            }

            @Override
            public void run() {
                Log.i(LOG_TAG, " play thread started!");
                final int waterLine = 4; // water line
                while (isPlayThreadAlived && !isInterrupted()) {
                    try {
                        final long timeout = 3L; // play thread dead time
                        if (!semPcm.tryAcquire(timeout, TimeUnit.SECONDS)) {
                            isPlayThreadAlived = false;
                            Log.e(LOG_TAG, "semPcm.acquire() fail!");
                            break;
                        }
                    } catch (InterruptedException ex) {
                        isPlayThreadAlived = false;
                        Log.e(LOG_TAG, "semPcm.acquire() fail!" + ex.getMessage());
                        break;
                    }
                    BufferContent tmp = playQ.poll();
                    if (tmp == null) {
                        continue;
                    }
                    String val = getProperty("ro.product.model");
                    if ("Monbox".equals(val) || "Kbox".equals(val)) {
                        if (playQ.size() >= Q_SIZE_LIMIT * (waterLine - 1) / waterLine) {
                            continue;
                        }
                        handleAudioBuffer(tmp);
                    } else {
                        playAudioBuffer(tmp);
                    }
                }
                Log.i(LOG_TAG, "play thread stopped!");
            }
        }

        private void setAudioStreamFormatInfo() {
            final int audioDataFormatPcm16 = 0x1;
            final int audioDataFormatPcm8 = 0x2;
            final int audioSampleSize = 2;
            switch (audioDataFormat) {
                case audioDataFormatPcm16:
                    audioDataFormat = AudioFormat.ENCODING_PCM_16BIT;
                    audioPcmSampleSize = audioSampleSize;
                    break;
                case audioDataFormatPcm8:
                    audioDataFormat = AudioFormat.ENCODING_PCM_8BIT;
                    audioPcmSampleSize = 1;
                    break;
                default:
                    audioDataFormat = AudioFormat.ENCODING_DEFAULT;
                    audioPcmSampleSize = audioSampleSize;
            }
        }

        private void setAudioStreamChannelInfo() {
            final int audioChannelCount = 2;
            if (audioPcmChannelConfig == AudioFormat.CHANNEL_OUT_STEREO) {
                audioPcmChannelCount = audioChannelCount;
            } else {
                audioPcmChannelConfig = AudioFormat.CHANNEL_OUT_MONO;
                audioPcmChannelCount = 1;
            }
        }
 
        
        private class AudioTrackParam {
            private int streamType = 0;
            private int sampleRateInHz = 0;
            private int channelConfig = 0;
            private int audioFormat = 0;
            private int sampleSize = 0;
            private int channelCount = 0;

            public boolean updateParam(AudioTrackParam newParam) {
                if (newParam.streamType == streamType && newParam.sampleRateInHz == sampleRateInHz &&
                    newParam.channelConfig == channelConfig && newParam.audioFormat == audioFormat &&
                    newParam.sampleSize == sampleSize && newParam.channelCount == channelCount) {
                        return false;
                    }
                streamType = newParam.streamType;
                sampleRateInHz = newParam.sampleRateInHz;
                channelConfig = newParam.channelConfig;
                audioFormat = newParam.audioFormat;
                sampleSize = newParam.sampleSize;
                channelCount = newParam.channelCount;
                return true;
            }

            public void setStreamType(int type) {
                streamType = type;
            }

            public int getStreamType() {
                return streamType;
            }

            public void setSampleRateInHz(int rate) {
                sampleRateInHz = rate;
            }

            public int getSampleRateInHz() {
                return sampleRateInHz;
            }

            public void setChannelConfig(int audioChannelConfig) {
                channelConfig = audioChannelConfig;
            }

            public int getChannelConfig() {
                return channelConfig;
            }

            public void setAudioFormat(int format) {
                audioFormat = format;
            }

            public int getAudioFormat() {
                return audioFormat;
            }

            public void setSampleSize(int audioSampleSize) {
                sampleSize = audioSampleSize;
            }

            public int getSampleSize() {
                return sampleSize;
            }

            public void setChannelCount(int audioChannelCount) {
                channelCount = audioChannelCount;
            }

            public int getChannelCount() {
                return channelCount;
            }
        }

        private void createAudioTrack(AudioTrackParam param) {
            int streamTypeTmp = param.getStreamType();
            int sampleRateInHzTmp = param.getSampleRateInHz();
            int channelConfig = param.getChannelConfig();
            int audioFormatTmp = param.getAudioFormat();
            int sampleSize = param.getSampleSize();
            int channelCount = param.getChannelCount();
            auDebug("handleSet streamType: " + streamTypeTmp
                    + " sampleRateInHz: " + sampleRateInHzTmp
                    + " channelConfig: " + channelConfig
                    + " audioFormat: " + audioFormatTmp
                    + " bufferSizeInBytes: " + bufferSizeInBytes
                    + " mode: " + mode);

            bufferLenPerSecond = sampleRateInHzTmp * sampleSize * channelCount;
            auDebug("bufferLenPerSecond is: " + bufferLenPerSecond);

            if (streamTypeTmp == AudioManager.STREAM_SYSTEM) {
                mode = AudioTrack.MODE_STREAM;
            }

            int minBufferLen = AudioTrack.getMinBufferSize(sampleRateInHzTmp, channelConfig, audioFormatTmp);
            if (minBufferLen <= 0) {
                Log.e(TAG, "Failed to create audioTrack, invalid minBufferLen, minBufferLen=" + minBufferLen);
                return;
            }
            auDebug("minBufferLen is " + minBufferLen);
            int trackBufferLen = (bufferSizeInBytes / minBufferLen + 1) * minBufferLen;

            auDebug(" trackBufferLen=" + trackBufferLen
                    + " minBufferLen=" + minBufferLen
                    + " bufferSizeInBytes=" + bufferSizeInBytes
                    );
            try {
                trackPlayer = new AudioTrack(streamTypeTmp, sampleRateInHzTmp, channelConfig, audioFormatTmp,
                    trackBufferLen, mode);
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Failed to create audioTrack, failed to new audioTrack");
                auDebug("IllegalArgumentException");
            }
        }

        private void handleStart() {
            Log.i(TAG, "handleStart, trackPlayer is: " + trackPlayer);
            if (trackPlayer == null) {
                Log.e(TAG, "Failed to handleStart, trackPlayer is null");
                return;
            }

            if (isMute) {
                mute();
            } else {
                unMute();
            }

            try {
                trackPlayer.play();
            } catch (IllegalStateException e) {
                Log.e(TAG, " handleStart error ", e);
            }
        }

        private int checkParamChanged(AudioRemoteMessage msg) {
            if (msg == null) {
                Log.e(TAG, "invalid msg: null");
                return PARAM_ERROR;
            }
            audioType = msg.readInt();
            int channels = msg.readByte();
            int channelConfig;
            if (channels == 2) {    //数据格式
                channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
            } else if (channels == 1) {
                channelConfig = AudioFormat.CHANNEL_OUT_MONO;
            } else {
                return PARAM_ERROR;
            }

            int audioBitDepth = msg.readByte();
            int audioFormat;
            if (audioBitDepth == 16) {    // 音频位深
                audioFormat = AudioFormat.ENCODING_PCM_16BIT;
            } else if (audioBitDepth == 8) {
                audioFormat = AudioFormat.ENCODING_PCM_8BIT;
            } else {
                audioFormat = AudioFormat.ENCODING_DEFAULT;
                return PARAM_ERROR;
            }

            int sampleRate1 = msg.readInt();
            int sampleInterval = msg.readInt();
            long timestamp = msg.readLong();

            AudioTrackParam newParam = new AudioTrackParam();
            newParam.setStreamType(3); // 3: music type
            newParam.setSampleRateInHz(sampleRate1);
            newParam.setChannelConfig(channelConfig);

            newParam.setAudioFormat(audioFormat);
            int bytesPerSample = audioBitDepth / BYTE_SIZE_IN_BIT * channels;
            int sampleSize = audioBitDepth / BYTE_SIZE_IN_BIT;
            newParam.setSampleSize(sampleSize);
            newParam.setChannelCount(channels);

            audioStreamType = 3; // 3: music type
            sampleRate = sampleRate1;
            audioPcmChannelConfig = channelConfig;
            audioDataFormat = audioFormat;
            bufferSizeInBytes = sampleRate * sampleInterval * bytesPerSample / AUDIO_SAMPLE_S_TO_MS;
            mode = AudioTrack.MODE_STREAM;

            auDebug("receive params = " +
                " sampleRate: " + sampleRate +
                " channels / channelConfig: " + channels + " / "+ channelConfig +
                " audioBitDepth: " + audioBitDepth +
                " audioFormat: " + audioFormat +
                " sampleInterval: " + sampleInterval +
                " sampleSize: " + sampleSize +
                " timestamp: " + timestamp +
                " bufferSizeInBytes: " + bufferSizeInBytes);

            if (params.updateParam(newParam)) {
                return PARAM_CHANGE;
            }
            return PARAM_UNCHANGE;
        }

        private void handleParamChange() {
            playBuffer.clear();
            poolIndex = 0;
            while (semPcm.tryAcquire()) {  // 此处为清空已有的信号量
                Log.d(TAG, "cleaning semPcm");
            }
            if (decoder == 0) {
                decoder = OpusJniWrapper.createOpusDecoder();
            }
            setAudioStreamFormatInfo();
            setAudioStreamChannelInfo();
            createAudioTrack(params);
        }

        private void handleWrite(AudioRemoteMessage msg) {
            int ret = checkParamChanged(msg);
            if (ret == PARAM_CHANGE) {
                Log.i(TAG, "AudioPlayer update params");
                handleParamChange();
            } else if (ret != PARAM_UNCHANGE) {
                Log.e(TAG, "check audio packet param fail");
                return;
            }

            if (!isPlayThreadAlived) {
                startPlayThread();
            }
            final int nanoPerUm = 1000; //  1000 nanoseconds equal to one millisecond
            audioClientTS = System.nanoTime() / nanoPerUm;
            int userSize = msg.readInt();
            if (startTimeStamp == 0) {
                startTimeStamp = audioClientTS;
            }
            if (trackPlayer == null || isMute) {
                return;
            }
            if (mode == AudioTrack.MODE_STATIC) {
                trackPlayer.write(msg.getPayload(), msg.getPayloadOffset(), userSize);
            } else {
                if (decoder == 0) {
                    return;
                }
                queueAudioData(msg, userSize);
            }
        }

        /**
         * 将short数组转换为占两个字节的byte数组
         *
         * @param shorts 要转换的short数组
         * @return byte[]
         */
        public byte[] shortToBytes(short[] shorts) {
            byte[] bytes = new byte[shorts.length * SHORT_TO_BYTES];
            ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().put(shorts);
            return bytes;
        }

        private void saveAudio(short[] inputBuffer, int frameSize) {
            if (audioSavehook == null || inputBuffer == null) {
                if (saveAudioLogIdx == Integer.MAX_VALUE) {
                    saveAudioLogIdx = 0;
                }
                if (((++saveAudioLogIdx) % logPrintInterval) == 0) {
                    Log.e(TAG, "save hook or inputBuffer is null");
                }
                return;
            }
            byte[] bytes = shortToBytes(inputBuffer);
            audioSavehook.audioSaveData(bytes, frameSize * SHORT_TO_BYTES);
        }

        private void queueAudioData(AudioRemoteMessage msg, int userSize) {
            queueSizeCheck();
            poolIndex = (poolIndex + 1) % Q_SIZE_LIMIT;
            byte[] buffer = msg.getPayload();
            int offset = msg.getPayloadOffset();
            int dataLen = userSize >> 1;
            if (audioType == AUDIO_TYPE_OPUS) {
                dataLen = OpusJniWrapper.opusDecode(decoder, buffer, userSize, bufferPool[poolIndex].data);
                if (dataLen <= 0) {
                    Log.e(TAG, "decode opus data failed, userSize=" + userSize + ", buffer.length="
                        + buffer.length + ", dataLen=" + dataLen);
                    return;
                }
            } else {
                for (int i = 0; i < userSize; i += 2) { // 内循环没处理连续两个字节的数据，故移动步进为2
                    int position = i + offset;
                    bufferPool[poolIndex].data[i >> 1] = // 左移8位拼成short类型
                        (short) ((buffer[position] & 0xff) | (buffer[position + 1] & 0xff) << 8);
                }
            }
            saveAudio(bufferPool[poolIndex].data, dataLen);
            bufferPool[poolIndex].setLength(dataLen);
            playBuffer.offer(bufferPool[poolIndex]);
            semPcm.release();
        }

        private void queueSizeCheck() {
            int size = playBuffer.size();
            final int nanoPerUm = 1000; //  1000 nanoseconds equal to one millisecond
            long now = System.nanoTime() / nanoPerUm;
            long deltaTime = now - audioClientTS;
            if ((size > 0) && (((double) deltaTime > DELTA_BOTTOM) || (size >= Q_SIZE_LIMIT - 1))) {
                audioClientTS = now;
                playBuffer.clear();
                poolIndex = 0;
                while (semPcm.tryAcquire()) { // 此处为清空已有的信号量
                    Log.d(TAG, "cleaning semPcm");
                }
                trackPlayer.flush();
                min = minQueueSizeDefault;
                max = 0;
                Log.i(TAG, "queueFullCheck(): clear playBuffer, delTime: " + deltaTime + " size: " + size
                    + " playBuffer.size: " + playBuffer.size());
            }
            final int retry = 10; // retry time
            final int waterLine = 4; // water line
            final int sleepTime = 1;
            for (int i = 0; i < retry; i++) {
                if (playBuffer.size() >= Q_SIZE_LIMIT * (waterLine - 1) / waterLine) {
                    try {
                        Thread.sleep(sleepTime);
                    } catch (InterruptedException e) {
                        Log.e(TAG, " queueSizeCheck error ", e);
                    }
                } else {
                    break;
                }
            }
            feedSize(playBuffer.size());
        }

        private void handleStop() {
            Log.i(TAG, "handleStop, trackPlayer is: " + trackPlayer);
            if (trackPlayer == null) {
                return;
            }
            try {
                trackPlayer.stop();
            } catch (IllegalStateException e) {
                Log.e(TAG, " handleStop error ", e);
            }
        }

        private void handleDestruct() {
            Log.i(TAG, "handleDestruct, trackPlayer is: " + trackPlayer);
            if (trackPlayer == null) {
                return;
            }
            trackPlayer.release();
            if (decoder != 0) {
                OpusJniWrapper.destroyOpusDecoder(decoder);
                decoder = 0;
            }
            TRACK_MAP.delete(pid);
            trackPlayer = null;
        }

        private void handleSetVolume(AudioRemoteMessage msg) {
            Log.i(TAG, "handleSetVolume, trackplayer is: " + trackPlayer);

            if (trackPlayer == null || isMute) {
                return;
            }

            float leftVolume = msg.readInt();
            float rightVolume = msg.readInt();
            final float volumeFactor = 100F; // volume factor
            final float volumeLimits = 1e-6f; // smallest volume bound

            volumeValue = leftVolume / volumeFactor;
            auDebug(String.format(Locale.ENGLISH,
                "handleSetVolume leftVolume=[%f], rightVolume=[%f]", leftVolume, rightVolume));

            boolean isRangeValid = (volumeValue > 0F) && (volumeValue < volumeFactor);
            boolean isLowerBound = Math.abs(volumeValue) < volumeLimits;
            boolean isUpperBound = Math.abs(volumeValue - volumeFactor) < volumeLimits;
            if (isRangeValid || isLowerBound || isUpperBound) {
                trackPlayer.setVolume(volumeValue);
            }
        }
    }
}