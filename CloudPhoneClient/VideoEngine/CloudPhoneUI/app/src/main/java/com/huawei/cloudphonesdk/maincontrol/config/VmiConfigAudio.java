package com.huawei.cloudphonesdk.maincontrol.config;

public class VmiConfigAudio extends VmiConfig{
    private int audioType = 0;
    private AudioPlayParams audioPlayParams;
    public int getAudioType() {
        return audioType;
    }

    public void setAudioType(int audioType) {
        this.audioType = audioType;
    }

    public AudioPlayParams getAudioPlayParams() {
        return audioPlayParams;
    }

    public void setAudioPlayParams(AudioPlayParams audioPlayParams) {
        this.audioPlayParams = audioPlayParams;
    }
}
