/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import static com.huawei.cloudphone.ui.activities.FullscreenActivity.START_MIC;
import static com.huawei.cloudphone.ui.activities.FullscreenActivity.STOP_MIC;

import android.nfc.Tag;
import android.util.Log;

import androidx.annotation.Keep;

/**
 * C++调Java
 *
 * @since 2020-06-17
 */
@Keep
public class OpenGLJniCallback implements NewPacketCallback {
    static {
        System.loadLibrary("VideoEngineJni");
    }

    public static final String TAG = "OpenGLJniCallback";
    // c++回调Java
    private JniCallback mJniCallback;
    private NativeListener listener = null;

    /**
     * C++绑定Java Object，便于C++直接调用Java方法
     */
    public native void setObj();

    /**
     * set NativeListener.
     *
     * @param listener NativeListener.
     */
    public void setNativeListener(NativeListener listener) {
        this.listener = listener;
    }

    /**
     * 绑定分辨率不支持回调
     *
     * @param callback JniCallback
     */
    public void setJniCallback(JniCallback callback) {
        mJniCallback = callback;
    }

    /**
     * 分辨率不支持回调
     */
    public void resolutionUnsupported() {
        if (mJniCallback != null) {
            mJniCallback.onResolutionUnsupported();
        }
    }

    /**
     * 帧数据回调
     *
     * @param frameType 帧类型
     * @param frameSize 帧大小
     * @param timeout   解码延迟
     * @param timestamp 解码一帧后的时间戳
     */
    public void onFrameChanged(int frameType, int frameSize, long timeout, long timestamp) {
        if (mJniCallback != null) {
            mJniCallback.onFrameChanged(frameType, frameSize, timeout, timestamp);
        }
    }

    @Override
    public void onNewPacket(byte[] data) {
        int cmd = byteArrayToIntLittleEndian(data);
        Log.i(TAG, "onNewPacket receive mic cmd = "+cmd);
        if (cmd == START_MIC) {
            onVmiVideoEngineEvent(START_MIC, 0, 0, 0, 0, "");
        } else if (cmd == STOP_MIC) {
            onVmiVideoEngineEvent(STOP_MIC, 0, 0, 0, 0, "");
        }
    }

    /**
     * 字节数组转int 大端模式
     */
    public static int byteArrayToIntBigEndian(byte[] bytes) {
        int x = 0;
        for (int i = 0; i < 4; i++) {
            x <<= 8;
            int b = bytes[i] & 0xFF;
            x |= b;
        }
        return x;
    }

    /**
     * 字节数组转int 小端模式
     */
    public static int byteArrayToIntLittleEndian(byte[] bytes) {
        int x = 0;
        for (int i = 0; i < 4; i++) {
            int b = (bytes[i] & 0xFF) << (i * 8);
            x |= b;
        }
        return x;
    }


    /**
     * 帧数据回调
     */
    public interface JniCallback {
        /**
         * 分辨率不支持回调
         */
        void onResolutionUnsupported();

        /**
         * 帧数据回调
         *
         * @param frameType 帧类型
         * @param frameSize 帧大小
         * @param timeout   解码延迟
         * @param timestamp 解码一帧后的时间戳
         */
        void onFrameChanged(int frameType, int frameSize, long timeout, long timestamp);
    }

    @Keep
    public void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3, String errInfo) {
        if (listener != null) {
            listener.onVmiVideoEngineEvent(event, reserved0, reserved1, reserved2, reserved3, errInfo);
        }
    }
}