/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.cloudphonesdk.audio.play;


import android.util.Log;

import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;

public class AudioPlayerCallback implements NewPacketCallback {
    /**
     * ignoreAudioSync.
     */
    public static boolean ignoreAudioSync = false;

    private static final String TAG = "AudioPlayerCallback";

    public static AudioTrackPlayer audioTrackPlayer = null;
    private int cnt = 0;

    /**
     * AudioPlayerCallback.
     */
    public AudioPlayerCallback() {
        audioTrackPlayer = AudioTrackPlayer.getInstance();
    }

    @Override
    public void onNewPacket(byte[] data) {
        int ret;
        if (!AudioTrackPlayer.getAudioReadyFlag()) {
            ret = audioTrackPlayer.onRecvAudioPacket(data, 0);
        } else {
            ret = audioTrackPlayer.onRecvAudioPacket(data, data.length);
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS && (++cnt % 200 == 0)) {
            Log.i(TAG, "VMI_SUCCESS Audio recv success ");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL && (++cnt % 200 == 0)) {
            Log.i(TAG, "VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL, Audio recv error ");
        }
    }
}
