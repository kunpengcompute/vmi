package com.huawei.cloudphonesdk.audio.record;

import android.media.AudioFormat;
import android.util.Log;

/**
 * 音频配置类
 */
public class AudioRecorderConfig {

    static final String TAG = "AudioRecorderConfig";

    private int RECORDER_SAMPLERATE;

    private String RECORDER_SAMPLERATE_TAG; //采样率标志位

    private int RECORDER_CHANNELS;

    private String RECORDER_CHANNELS_TAG; //声道数标志位

    private int RECORDER_AUDIO_ENCODING;

    private String RECORDER_AUDIO_ENCODING_TAG; //量化精度标志位


    public AudioRecorderConfig(String RECORDER_SAMPLERATE_TAG, String RECORDER_CHANNELS_TAG, String RECORDER_AUDIO_ENCODING_TAG) {
        this.RECORDER_SAMPLERATE_TAG = RECORDER_SAMPLERATE_TAG;
        this.RECORDER_CHANNELS_TAG = RECORDER_CHANNELS_TAG;
        this.RECORDER_AUDIO_ENCODING_TAG = RECORDER_AUDIO_ENCODING_TAG;
        if (RECORDER_SAMPLERATE_TAG.equals("0")) {
            this.RECORDER_SAMPLERATE = 44100;
        } else {
            this.RECORDER_SAMPLERATE = 48000;
        }
        if (RECORDER_CHANNELS_TAG.equals("0")) {
            this.RECORDER_CHANNELS = AudioFormat.CHANNEL_IN_MONO;
        } else {
            this.RECORDER_CHANNELS = AudioFormat.CHANNEL_IN_STEREO;
        }
        if (RECORDER_AUDIO_ENCODING_TAG.equals("0")) {
            this.RECORDER_AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
        } else {
            //32位参数没找到，先走16位的
            this.RECORDER_AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
        }

        Log.d(TAG, "set AudioRecorderConfig over RECORDER_SAMPLERATE:" + RECORDER_SAMPLERATE + " RECORDER_CHANNELS:" + RECORDER_CHANNELS + " RECORDER_AUDIO_ENCODING:" + RECORDER_AUDIO_ENCODING);

    }

    public int getRECORDER_SAMPLERATE() {
        return RECORDER_SAMPLERATE;
    }

    public void setRECORDER_SAMPLERATE(int RECORDER_SAMPLERATE) {
        this.RECORDER_SAMPLERATE = RECORDER_SAMPLERATE;
    }

    public int getRECORDER_CHANNELS() {
        return RECORDER_CHANNELS;
    }

    public void setRECORDER_CHANNELS(int RECORDER_CHANNELS) {
        this.RECORDER_CHANNELS = RECORDER_CHANNELS;
    }

    public int getRECORDER_AUDIO_ENCODING() {
        return RECORDER_AUDIO_ENCODING;
    }

    public void setRECORDER_AUDIO_ENCODING(int RECORDER_AUDIO_ENCODING) {
        this.RECORDER_AUDIO_ENCODING = RECORDER_AUDIO_ENCODING;
    }
}
