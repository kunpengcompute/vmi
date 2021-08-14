/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.audio.play;

import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.instructionstream.appui.maincontrol.NewPacketCallback;
import com.huawei.instructionstream.appui.utils.LogUtils;

/**
 * AudioTrackPlayer.
 *
 * @since 2018-07-05
 */
public class AudioPlayerCallback implements NewPacketCallback {
    /**
     * ignoreAudioSync.
     */
    public static boolean ignoreAudioSync = false;

    private static final String TAG = "AudioPlayerCallback";

    private static AudioTrackPlayer audioTrackPlayer = null;
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
        if (!AudioTrackPlayer.isReady) {
            ret = audioTrackPlayer.onRecvAudioPacket(data, 0);
        } else {
            ret = audioTrackPlayer.onRecvAudioPacket(data, data.length);
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS && (++cnt % 200 == 0)) {
            LogUtils.info(TAG, "VMI_SUCCESS Audio recv success ");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL && (++cnt % 200 == 0)) {
            LogUtils.error(TAG, "VMI_AUDIO_ENGINE_CLIENT_RECV_FAIL, Audio recv error ");
        }
    }
}
