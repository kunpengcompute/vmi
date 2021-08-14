/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.instructionstream.appui.engine;

import android.view.Surface;
import com.huawei.instructionstream.appui.engine.utils.LogUtils;

/**
 * BaseActivity interface.
 *
 * @since 2019-10-31
 */
public class InstructionEngine {
    private static final String TAG = "INSTRUCTION_ENGINE";
    /**
     * vmi success.
     */
    public static final int VMI_SUCCESS = 0;
    /**
     * VMI_INSTRUCTION_CLIENT_INVALID_PARAM.
     */
    public static final int VMI_INSTRUCTION_CLIENT_INVALID_PARAM = 0x0A050001;
    /**
     * VMI_INSTRUCTION_CLIENT_START_FAIL.
     */
    public static final int VMI_INSTRUCTION_CLIENT_START_FAIL = 0x0A050002;
    /**
     * VMI_INSTRUCTION_CLIENT_ALREADY_STARED.
     */
    public static final int VMI_INSTRUCTION_CLIENT_ALREADY_STARED = 0x0A050003;
    /**
     * VMI_INSTRUCTION_CLIENT_STOP_FAIL.
     */
    public static final int VMI_INSTRUCTION_CLIENT_STOP_FAIL = 0x0A050004;
    /**
     * VMI_INSTRUCTION_CLIENT_SEND_HOOK_REGISTER_FAIL.
     */
    public static final int VMI_INSTRUCTION_CLIENT_SEND_HOOK_REGISTER_FAIL = 0x0A050005;
    /**
     * VMI_INSTRUCTION_CLIENT_SEND_FAIL.
     */
    public static final int VMI_INSTRUCTION_CLIENT_SEND_FAIL = 0x0A050006;
    /**
     * VMI_INSTRUCTION_CLIENT_SEND_AGAIN.
     */
    public static final int VMI_INSTRUCTION_CLIENT_SEND_AGAIN = 0x0A050007;
    /**
     * VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN.
     */
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN = -2;
    /**
     * VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN.
     */
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN = -3;
    /**
     * VMI_INSTRUCTION_ENGINE_EVENT_VERSION_ERROR.
     */
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_VERSION_ERROR = -4;
    /**
     * VMI_INSTRUCTION_ENGINE_EVENT_READY.
     */
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_READY = -5;
    /**
     * VMI_INSTRUCTION_ENGINE_EVENT_ORIENTATION_CHANGED.
     */
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_ORIENTATION_CHANGED = -6;
    /**
     * VMI_INSTRUCTION_CLIENT_ALREADY_STARTED.
     */
    public static final int VMI_INSTRUCTION_CLIENT_ALREADY_STARTED = 0x0A050003;

    /**
     * enum rotation.
     */
    public enum Rotation {
        ROTATION0(0),
        ROTATION90(1),
        ROTATION180(2),
        ROTATION270(3);
        Rotation(int value) {
            this.value = value;
        }

        /**
         * getValue.
         *
         * @return get enum Rotation value
         */
        public int getValue() {
            return this.value;
        }

        private int value;
    }

    /**
     * initialize.
     *
     * @return interger data.
     */
    public int initialize() {
        int result = InstructionWrapper.initialize();
        LogUtils.info(TAG, "InstructionWrapper initialize result: " + result);
        return result;
    }

    /**
     * start instruction engine.
     *
     * @param surface surface parameters.
     * @param width surface width parameters.
     * @param height surface height parameters.
     * @param densityDpi surface densityDpi parameters.
     * @return Instruction Engine start result
     */
    public int start(Surface surface, int width, int height, float densityDpi) {
        int result = InstructionWrapper.start(surface, width, height, densityDpi);
        if (result != InstructionEngine.VMI_SUCCESS) {
            LogUtils.error(TAG, "start instruction engine failed, result :" + result);
            return result;
        }

        return result;
    }

    /**
     * getStat.
     *
     * @return string state detail.
     */
    public String getStat() {
        return InstructionWrapper.getStat();
    }

    /**
     * stop.
     */

    public void stop() {
        LogUtils.info(TAG, "Security Audit: InstructionEngine stop");
        InstructionWrapper.stop();
    }

    /**
     * recv data from SDK.
     *
     * @param type stream type
     * @param data byte array to save data
     * @param length size of byte array
     * @return actual size of the data obtained
     */
    public int recvData(byte type, byte[] data, int length) {
        return InstructionWrapper.recvData(type, data, length);
    }

    /**
     * send touch event data to server in data array.
     *
     * @param data touch data byte array
     * @param length the length of data
     * @return return true if success, return false if failed
     */
    public boolean sendTouchEventArray(byte[] data, int length) {
        return InstructionWrapper.sendTouchEventArray(data, length);
    }

    /**
     * send recoder data to server.
     *
     * @param data the data to send
     * @param length size of data
     * @return return true if success, return false if failed
     */
    public boolean sendAudioDataArray(byte[] data, int length) {
        return InstructionWrapper.sendAudioDataArray(data, length);
    }

    /**
     * send navbar data to server.
     *
     * @param data the data to send
     * @param length size of data
     * @return return true if success, return false if failed
     */
    public boolean sendKeyEvent(byte[] data, int length) {
        return InstructionWrapper.sendKeyEvent(data, length);
    }
}
