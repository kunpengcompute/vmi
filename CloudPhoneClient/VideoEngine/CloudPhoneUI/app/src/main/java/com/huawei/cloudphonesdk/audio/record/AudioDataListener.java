package com.huawei.cloudphonesdk.audio.record;

public interface AudioDataListener {
    void onAudioDataReceived(byte[] bytes);
}
