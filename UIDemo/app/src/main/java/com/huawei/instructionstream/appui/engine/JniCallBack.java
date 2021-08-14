/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.instructionstream.appui.engine;

import androidx.annotation.Keep;

/**
 * JniCallBack.
 *
 * @since 2018-07-05
 */
public class JniCallBack {
    static {
        System.loadLibrary("InstructionEngineJni");
    }

    private NativeListener listener = null;

    /**
     * setNativeCallback.
     */
    public native void setNativeCallback();

    /**
     * set NativeListener.
     *
     * @param listener NativeListener.
     */
    public void setNativeListener(NativeListener listener) {
        this.listener = listener;
    }

    /**
     * VMI Instruction Engine event notice.
     *
     * @param event VMI Instruction Engine event.
     */
    @Keep
    public void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3) {
        if (listener != null) {
            listener.onVmiInstructionEngineEvent(event, reserved0, reserved1, reserved2, reserved3);
        }
    }
}
