package com.huawei.cloudphonesdk.maincontrol.config;

public class VmiConfigMic {

    // 当前的版本号信息
    private int version = 1;
    private int audioType = 0;
    private int sampleInterval = 10;

    public int getSampleInterval() {
        return sampleInterval;
    }

    public void setSampleInterval(int sampleInterval) {
        this.sampleInterval = sampleInterval;
    }

    public int getAudioType() {
        return audioType;
    }

    public void setAudioType(int audioType) {
        this.audioType = audioType;
    }

    public int getVersion() {
        return version;
    }

    enum AudioType {
        OPUS,
        PCM
    }
}
