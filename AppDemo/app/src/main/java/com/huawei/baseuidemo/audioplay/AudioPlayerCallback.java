package com.huawei.baseuidemo.audioplay;

import android.util.Log;

import com.huawei.baseuidemo.maincontrol.NewPacketCallback;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;

public class AudioPlayerCallback implements NewPacketCallback {

    private int count = 0;
    private static final String TAG = "VMI_AUDIOPLAYERCALLBACK";
    private static AudioTrackPlayer audioTrackPlayer = null;

    public AudioPlayerCallback() {
        audioTrackPlayer = AudioTrackPlayer.getInstance();
    }

    @Override
    public void onNewPacket(byte[] data) {
        int result = 0;
        if (!AudioTrackPlayer.isReady) {
            result = audioTrackPlayer.onRecvAudioPacket(data, 0);
        } else {
            result = audioTrackPlayer.onRecvAudioPacket(data, data.length);
        }

        if (result == AudioTrackPlayer.VMI_SUCCESS && (++count % 200 == 0)) {
            Log.i(TAG, "audio recv data success.");
        } else if (result == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL && (++count % 200 == 0)) {
            Log.e(TAG, "audio recv data failed.");
        }
    }
}
