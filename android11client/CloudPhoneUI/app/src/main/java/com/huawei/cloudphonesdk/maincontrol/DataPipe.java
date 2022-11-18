/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import android.util.Log;

import com.huawei.cloudgame.audioplay.AUDIOSENDHOOK;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.TOUCHSENDHOOK;
import com.huawei.cloudgame.touch.VmiTouch;

/**
 * An example full-screen activity that shows and hides the system UI (i.e. status bar and navigation/system bar) with
 * user interaction.
 *
 * @since 2018-07-05
 */
public class DataPipe implements TOUCHSENDHOOK, AUDIOSENDHOOK {
    // just for testVmiTouchJava : registerTouchSendHook
    private static boolean vmiTouchToJava = false;
    private static boolean isAudioClient = false;
    private static final String TAG = "DataPipe";
    private static final String AUDIO_TAG = "VmiAudioClient";
    private static final String TOUCH_TAG = "VmiTouch";
    private static DataPipe instance = new DataPipe();
    private static boolean ready = false;

    private DataPipe() {
    }

    /**
     * DataPipe.
     *
     * @return instance.
     */
    public static DataPipe getInstance() {
        return instance;
    }

    /**
     * registerHookToTouch.
     */
    public static void registerHookToTouch() {
        int ret = VmiTouch.getInstance().registerTouchSendHook(DataPipe.getInstance());
        if (ret == VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL) {
            Log.i(TOUCH_TAG, "registerHookFail: VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL"
                    + " input " + null
                    + " output " + VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == VmiTouch.VMI_SUCCESS) {
            Log.i(TOUCH_TAG, "registerHookSuccess: VMI_SUCCESS"
                    + " input " + DataPipe.getInstance()
                    + " output " + VmiTouch.VMI_SUCCESS
            );
        }
    }

    /**
     * registerHookToAudio.
     */
    public static void registerHookToAudio() {
        int ret = AudioTrackPlayer.getInstance().registerAudioSendHook(DataPipe.getInstance());
        if (ret == AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL) {
            Log.i(AUDIO_TAG, " audio registerHookFail: VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL"
                    + " input " + null
                    + " output " + AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            Log.i(AUDIO_TAG, " audio registerHookFail: VMI_SUCCESS"
                    + " input " + DataPipe.getInstance()
                    + " output " + AudioTrackPlayer.VMI_SUCCESS
            );
        }
    }

    /**
     * onNewPacket.
     *
     * @param data   byte[].
     * @param length int lenth.
     */
    @Override
    public void touchSendData(byte[] data, int length) {
        OpenGLJniWrapper.sendTouchEvent(data, length);
    }

    /**
     * send audio client data interface.
     *
     * @param data   byte[]. int length.
     * @param length int length
     */
    @Override
    public void audioSendData(byte[] data, int length) {

    }
}
