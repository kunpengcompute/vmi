package com.huawei.cloudphonesdk.maincontrol;

import android.view.Surface;

public class OpenGLJniWrapper {
    public static final byte AUDIO = 3;

    static {
        System.loadLibrary("VideoEngineJni");
    }

    public static native int recvData(byte type, byte[] data, int length);

    public static native boolean sendKeyEvent(byte[] data, int length);

    public static native void sendTouchEvent(byte[] data, int length);

    public static native int initialize();

    public static native int start(Surface surface, int width, int height, float densityDpi);

    public static native void stop();

    public static native String getStatistics();

}
