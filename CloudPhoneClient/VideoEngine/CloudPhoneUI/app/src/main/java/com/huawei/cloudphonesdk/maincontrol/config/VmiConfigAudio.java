package com.huawei.cloudphonesdk.maincontrol.config;

public class VmiConfigAudio {
    private int version = 1;                           // 当前的版本号信息
    private int audioType = 0;
    //AudioPlayParams params;                     // 默认的音频播放参数
    private int sampleInterval = 10;               // ms，只有在OPUS格式有效
    private int bitrate = 192000;                  // bps，只有在OPUS格式有效

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }

    public int getAudioType() {
        return audioType;
    }

    public void setAudioType(int audioType) {
        this.audioType = audioType;
    }

    public int getSampleInterval() {
        return sampleInterval;
    }

    public void setSampleInterval(int sampleInterval) {
        this.sampleInterval = sampleInterval;
    }

    public int getBitrate() {
        return bitrate;
    }

    public void setBitrate(int bitrate) {
        this.bitrate = bitrate;
    }


    enum AudioType {
        OPUS,
        PCM
    }

}
