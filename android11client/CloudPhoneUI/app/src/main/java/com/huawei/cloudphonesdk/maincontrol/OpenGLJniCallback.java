package com.huawei.cloudphonesdk.maincontrol;

import androidx.annotation.Keep;

public class OpenGLJniCallback {
    static {
        System.loadLibrary("VideoEngineJni");
    }

    private NativeListener listener = null;

    /**
     * setObj.
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

    @Keep
    public void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3) {
        if (listener != null) {
            listener.onVmiVideoEngineEvent(event, reserved0, reserved1, reserved2, reserved3);
        }

    }

}
