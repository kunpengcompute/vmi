package com.huawei.baseuidemo.engine;

public class JniCallBack implements NativeListener{
    /**
     * loadjni库
     */
    static {
        System.loadLibrary("InstructionEngineJni");
    }

    private NativeListener listener = null;

    /**
     * 回调
     */
    public native void setNativeCallback();

    /**
     * set NativeListener.
     *
     * @param listener
     */
    public void setNativeListener(NativeListener listener) {
        this.listener = listener;
    }

    public void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3) {
        if (listener != null) {
            listener.onVmiInstructionEngineEvent(event, reserved0, reserved1, reserved2, reserved3);
        }
    }
}
