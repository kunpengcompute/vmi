/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

import com.huawei.cloudgame.audioplay.AUDIOSENDHOOK;
import com.huawei.cloudgame.audioplay.AudioTrackPlayer;
import com.huawei.cloudgame.touch.TOUCHSENDHOOK;
import com.huawei.cloudgame.touch.VmiTouch;
import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.engine.InstructionEngine;

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
    private static InstructionEngine instructionEngine;

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
     * set InstructionEngine.
     *
     * @param engine InstructionEngine
     */
    public static void setInstructionEngine(InstructionEngine engine) {
        instructionEngine = engine;
    }

    /**
     * setTestTouch.
     *
     * @param enable enable
     */
    public static void setVmiTouchToJava(boolean enable) {
        vmiTouchToJava = enable;
    }

    /**
     * set test audio client.
     *
     * @param enable enable
     */
    public static void setIsAudioClient(boolean enable) {
        isAudioClient = enable;
    }

    /**
     * registerHookToTouch.
     */
    public static void registerHookToTouch() {
        int ret;
        LogUtils.info(TAG, "DataPipe =" + vmiTouchToJava);
        if (vmiTouchToJava) {
            LogUtils.info(TOUCH_TAG, " hook register null");
            ret = VmiTouch.getInstance().registerTouchSendHook(null);
        } else {
            ret = VmiTouch.getInstance().registerTouchSendHook(DataPipe.getInstance());
            LogUtils.info(TOUCH_TAG, " hook register normal");
        }

        if (ret == VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL) {
            LogUtils.info(TOUCH_TAG, "registerHookFail: VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL"
                    + " input " + null
                    + " output " + VmiTouch.VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == VmiTouch.VMI_SUCCESS) {
            LogUtils.info(TOUCH_TAG, "registerHookSuccess: VMI_SUCCESS"
                    + " input " + DataPipe.getInstance()
                    + " output " + VmiTouch.VMI_SUCCESS
            );
        }
    }

    /**
     * registerHookToAudio.
     */
    public static void registerAudioSendHook() {
        int ret;
        LogUtils.info(TAG, "AudioClient DataPipe " + isAudioClient);
        if (isAudioClient) {
            LogUtils.info(AUDIO_TAG, "AudioClient redister null");
            ret = AudioTrackPlayer.getInstance().registerAudioSendHook(null);
        } else {
            ret = AudioTrackPlayer.getInstance().registerAudioSendHook(DataPipe.getInstance());
            LogUtils.info(AUDIO_TAG, "AudioClient redister normal");
        }

        if (ret == AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL) {
            LogUtils.info(AUDIO_TAG, " audio registerHookFail: VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL"
                    + " input " + null
                    + " output " + AudioTrackPlayer.VMI_AUDIO_CLIENT_SENDHOOK_REGISTER_FAIL
            );
        }

        if (ret == AudioTrackPlayer.VMI_SUCCESS) {
            LogUtils.info(AUDIO_TAG, " audio registerHookFail: VMI_SUCCESS"
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
        if (!instructionEngine.sendTouchEventArray(data, length)) {
            LogUtils.error(AUDIO_TAG, "sendTouchEventArray failed");
        }
    }

    /**
     * send audio client data interface.
     *
     * @param data   byte[]. int length.
     * @param length int length
     */
    @Override
    public void audioSendData(byte[] data, int length) {
        if (!instructionEngine.sendAudioDataArray(data, length)) {
            LogUtils.error(AUDIO_TAG, "sendAudioDataArray failed");
        }
    }
}
