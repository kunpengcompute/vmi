/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
 */

package com.huawei.cloudphonesdk.fps;

import static com.huawei.cloudphone.ui.activities.FullscreenActivity.FPS_TEST_FAIL_FLAG;

import androidx.annotation.Keep;
import com.huawei.cloudphonesdk.maincontrol.NativeListener;
import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniCallback;

/**
 * FpsTestCallBack.
 *
 * @since 2024-08-22
 */
public class FpsTestCallBack implements NewPacketCallback {

    private static final String TAG = "VmiFPS";
    public static final int FPS_TEST_RECEIVE_DATA_LENGTH = 12;
    private NativeListener listener = null;

    public void setNativeListener(NativeListener listener) {
        this.listener = listener;
    }

    /**
     * FpsTestCallBack.
     */
    public FpsTestCallBack() {
    }

    @Override
    public void onNewPacket(byte[] data) {
        if (data.length == FPS_TEST_RECEIVE_DATA_LENGTH) {
            byte[] fps = new byte[4];
            System.arraycopy(data, 0, fps, 0, 4);
            int fpsValue = OpenGLJniCallback.byteArrayToIntLittleEndian(fps);
            byte[] jank = new byte[4];
            System.arraycopy(data, 4, jank, 0, 4);
            int jankValue = OpenGLJniCallback.byteArrayToIntLittleEndian(jank);
            byte[] bJank = new byte[4];
            System.arraycopy(data, 8, bJank, 0, 4);
            int bJankValue = OpenGLJniCallback.byteArrayToIntLittleEndian(bJank);
            onVmiFpsDataReceive(fpsValue, jankValue, bJankValue);
        } else if (data.length == 1) {
            onVmiFpsDataReceive(FPS_TEST_FAIL_FLAG, FPS_TEST_FAIL_FLAG, FPS_TEST_FAIL_FLAG);
        }
    }

    @Keep
    public void onVmiFpsDataReceive(int fps, int jank, int bjank) {
        if (listener != null) {
            listener.onVmiFpsDataReceive(fps, jank, bjank);
        }
    }
}