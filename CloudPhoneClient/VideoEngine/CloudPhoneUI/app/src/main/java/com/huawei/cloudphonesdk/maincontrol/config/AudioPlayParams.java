package com.huawei.cloudphonesdk.maincontrol.config;

public class AudioPlayParams {
    protected int sampleInterval = 10;
    protected int bitrate = 192000;

    public AudioPlayParams(){

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
}