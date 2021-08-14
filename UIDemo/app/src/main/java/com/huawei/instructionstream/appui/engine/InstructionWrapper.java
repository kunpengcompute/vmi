/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.instructionstream.appui.engine;

import android.view.Surface;

/**
 * InstructionWrapper.
 *
 * @since 2018-07-05
 */
public class InstructionWrapper {
    /**
     * sdk stopped status.
     */
    public static final int STATUS_STOPPED = 8;
    /**
     * sdk connection failure status.
     */
    public static final int STATUS_CONNECTION_FAILURE = 9;

    /**
     * sdk invalid status.
     */
    public static final byte INVALID = 0;

    /**
     * audio play stream type.
     */
    public static final byte AUDIO = 3;

    static {
        System.loadLibrary("InstructionEngineJni");
    }

    /**
     * initialize.
     *
     * @return initialize result
     */
    public static native int initialize();

    /**
     * start instruction engine.
     *
     * @param surface Surface object.
     * @param width Surface width.
     * @param height Surface height.
     * @param height Surface densityDpi.
     * @return start instruction engine result
     */
    public static native int start(Surface surface, int width, int height, float densityDpi);

    /**
     * stop instruction engine.
     */
    public static native void stop();

    /**
     * getStat
     *
     * @return instruction engine frame rate
     */
    public static native String getStat();

    /**
     * recv data from SDK.
     *
     * @param type stream type
     * @param data byte array to save data
     * @param length size of byte array
     * @return actual size of the data obtained
     */
    public static native int recvData(byte type, byte[] data, int length);

    /**
     * send touch event data to server in data array.
     *
     * @param data touch data byte array
     * @param length the length of data
     * @return return true if success, return false if failed
     */
    public static native boolean sendTouchEventArray(byte[] data, int length);

    /**
     * send recoder data to server.
     *
     * @param data the data to send
     * @param length size of data
     * @return return true if success, return false if failed
     */
    public static native boolean sendAudioDataArray(byte[] data, int length);

    /**
     * send recoder data to server.
     *
     * @param data the data to send
     * @param length size of data
     * @return return true if success, return false if failed
     */
    public static native boolean sendKeyEvent(byte[] data, int length);
}
