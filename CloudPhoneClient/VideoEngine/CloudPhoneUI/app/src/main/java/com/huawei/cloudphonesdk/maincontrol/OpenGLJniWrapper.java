/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import android.view.Surface;

import com.huawei.cloudphonesdk.maincontrol.config.VmiConfigAudio;
import com.huawei.cloudphonesdk.maincontrol.config.VmiConfigMic;
import com.huawei.cloudphonesdk.maincontrol.config.VmiConfigVideo;

/**
 * OpenGLJniWrapper.
 *
 * @since 2018-07-05
 */
public class OpenGLJniWrapper {
    /**
     * sdk invalid status.
     */
    public static final byte INVALID = 0;

    /**
     * heartbeat stream type.
     */
    public static final byte HEARTBEAT = 1;
    /**
     * game control stream type.
     */
    public static final byte GL = 2;
    /**
     * audio play stream type.
     */
    public static final byte AUDIO = 3;
    /**
     * touch input stream type.
     */
    public static final byte TOUCH_INPUT = 4;
    /**
     * navigation bar input stream type.
     */
    public static final byte NAVBAR_INPUT = 5;
    /**
     * video stream type.
     */
    public static final byte VIDEO_RR2 = 16;

    public static final byte MIC = 8;

    static {
        System.loadLibrary("VideoEngineJni");
    }

    /**
     * recv data from SDK.
     *
     * @param type   stream type
     * @param data   byte array to save data
     * @param length size of byte array
     * @return actual size of the data obtained
     */
    public static native int recvData(byte type, byte[] data, int length);

    /**
     * send key event data to server.
     *
     * @param data the data to send
     * @param length  size of data
     * @return return true if success, return false if failed
     */
    public static native boolean sendKeyEvent(byte[] data, int length);

    /**
     * send touch event data to server in data array.
     *
     * @param data   touch data byte array
     * @param length the length of data
     */
    public static native boolean sendTouchEvent(byte[] data, int length);

    /**
     * initialize SDK, call native method.
     *
     * @return return VMI_SUCCESS if success, else return failed
     */
    public static native int initialize();

    /**
     * start SDK, call native method.
     *
     * @param surface view surface for rendering
     * @param width Surface width.
     * @param height Surface height.
     * @param densityDpi Surface densityDpi.
     * @return return VMI_SUCCESS if success, else return failed
     */
    public static native int start(Surface surface, int width, int height, float densityDpi);

    /**
     * stop cloud game.
     */
    public static native void stop();

    /**
     * get the performance statistics string, such as frame rate, lag and other information of cloud games.
     *
     * @return the performance statistics string
     */
    public static native String getStatistics();

    /**
     * send audio data to server.
     *
     * @param data   the data to send
     * @param length size of data
     * @return return true if success, return false if failed
     */
    public static native boolean sendAudioDataArray(byte[] data, int length);
    
    public static native boolean composeMicData(byte[] data, int length, int audioType, int sampleInterval);

    public static native boolean setVideoParam(VmiConfigVideo config);

    public static native boolean setAudioParam(VmiConfigAudio config);

    public static native boolean setMicParam(VmiConfigMic config);
}
