/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.audio.play;

import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * AudioTrackPlayer.
 *
 * @since 2018-07-05
 */
public class AudioPlayerCallback implements NewPacketCallback {
    /**
     * audioTrackPlayer
     */
    public static AudioTrackPlayer audioTrackPlayer = null;

    private static final String TAG = "AudioPlayerCallback";

    private int cnt = 0;

    private long now = 0L;

    private long last = 0L;

    private final long LOG_INTERVAL = 1000L;

    /**
     * AudioPlayerCallback.
     */
    public AudioPlayerCallback() {
        audioTrackPlayer = AudioTrackPlayer.getInstance();
    }

    @Override
    public void onNewPacket(byte[] data) {
        int ret = audioTrackPlayer.onRecvAudioPacket(data, data.length);
        if (ret == AudioTrackPlayer.VMI_SUCCESS && (++cnt % 200 == 0)) {
            LogUtil.info(TAG, "Suceeded in receiving Audio data packets.");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL) {
            now = System.currentTimeMillis();
            if ((last == 0) || (now - last >= LOG_INTERVAL)) {
                LogUtil.error(TAG, "Failed to receive Audio data packets.");
                last = now;
            }
        }
    }
}