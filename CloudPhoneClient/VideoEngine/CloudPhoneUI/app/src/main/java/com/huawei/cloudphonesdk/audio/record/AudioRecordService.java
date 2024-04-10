package com.huawei.cloudphonesdk.audio.record;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import com.huawei.cloudphonesdk.audio.OpusUtils;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniWrapper;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

public class AudioRecordService extends Service {
    public static final String TAG = "AudioRecordService";
    public static final int SAMPLE_RATE = 48000;
    public static final int CHANNELS = 2;
    public static final int SUCCESS = 1;
    public static final int timeInterval = 10;
    private AudioRecord recorder;
    private Thread recordingThread;
    private boolean isRecording = false;
    private AudioDataListener audioDataListener;
    private final AudioBinder audioBinder = new AudioBinder();

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return audioBinder;
    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate: ");
        super.onCreate();
    }

    public class AudioBinder extends Binder {
        public AudioRecordService getService() {
            return AudioRecordService.this;
        }
    }

    public void setAudioDataListener(AudioDataListener audioDataListener) {
        this.audioDataListener = audioDataListener;
    }

    @SuppressLint("MissingPermission")
    public void startRecord(int bitrate, int audioType, int sampleInterval) {
        recorder = new AudioRecord(MediaRecorder.AudioSource.MIC,
            48000, AudioFormat.CHANNEL_IN_STEREO, AudioFormat.ENCODING_PCM_16BIT,
            bitrate / (1000 / sampleInterval));
        recorder.startRecording();
        isRecording = true;
        recordingThread = new Thread(new Runnable() {
            public void run() {
                writeAudioDataToFile(audioType, bitrate, sampleInterval);
            }
        }, "AudioRecorder Thread");
        recordingThread.start();
    }

    public void stop() {
        if (recorder != null) {
            isRecording = false;
            recorder.stop();
            recorder.release();
            recorder = null;
            recordingThread = null;
        }
    }

    @Override
    public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);

    }

    private void writeAudioDataToFile(int audioType, int bitrate, int sampleInterval) {
        Log.d(TAG, "writeAudioDataToFile: ");
        // 每次从麦克风获取的数据=采样率*时间间隔（s）*声道数*位深/8=48000*(sampleInterval/1000)*2*16/8
        byte data[] = new byte[192 * sampleInterval];
        String filePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/audio.opus";
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(filePath);
        } catch (IOException e) {
            e.printStackTrace();
        }
        int opusEncoder = OpusUtils.createOpusEncoder(SAMPLE_RATE, CHANNELS, bitrate);
        if (opusEncoder == SUCCESS) {
            Log.i(TAG, "创建opus编码器成功");
        } else {
            Log.e(TAG, "创建opus编码器失败");
        }
        long startTime = System.currentTimeMillis();
        int index = 0;
        while (isRecording) {
            int read = recorder.read(data, 0, data.length);
            if (read > 0) {
                try {
                    if (audioType == 0) {
                        short[] shortData = new short[data.length / 2];
                        ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shortData);
                        // 码率*时间间隔(s)/8
                        byte[] outData = new byte[(int) (bitrate * sampleInterval / 1000 / 8)];
                        // 采样间隔(s)*采样率=sampleInterval/1000*48000
                        int i = OpusUtils.opusEncode(shortData, 48*sampleInterval, outData);
                        // 保存文件到本地
                        // os.write(outData);
                        // 发送opus数据
                        audioDataListener.onAudioDataReceived(outData);
                    } else {
                        // 发送pcm数据
                        audioDataListener.onAudioDataReceived(data);
                    }
                } catch (Exception e) {
                    Log.e(TAG, "writeAudioDataToFile: " + e.getMessage());
                }
            } else {
                Log.e(TAG, "writeAudioDataToFile: do not receive mic data");
            }
            delayCompensation(startTime, index, sampleInterval);
            index++;
        }
        try {
            os.close();
            OpusUtils.destroyOpusEncoder();
        } catch (IOException e) {
            Log.d(TAG, "close pcm file err:" + e.getMessage());
        }
    }

    private void delayCompensation(long startTime, int index, int sampleInterval) {
        long currentTime = System.currentTimeMillis();
        long value = currentTime - startTime - index * 10L;
        Log.d(TAG, "delayCompensation: currentTime:"+currentTime+",startTime+"+startTime+",value:" + value +",index:"+index);
        if (value >= 0 && value < sampleInterval) {
            try {
                Thread.sleep(sampleInterval - value);
            } catch (InterruptedException e) {
                Log.e(TAG, "delayCompensation: " + e.getMessage());
            }
        }
    }
}
