package com.huawei.cloudphonesdk.video;

import com.huawei.cloudphonesdk.maincontrol.NativeListener;
import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.maincontrol.OpenGLJniCallback;
import com.huawei.cloudphonesdk.maincontrol.config.EncodeParams;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.util.Arrays;

public class EncodeParamsCallback implements NewPacketCallback {
    private final String TAG = EncodeParamsCallback.class.getSimpleName();
    private NativeListener listener = null;

    @Override
    public void onNewPacket(byte[] data) {
        byte[] bitrate = new byte[4];
        byte[] gopSize = new byte[4];
        byte[] profile = new byte[4];
        byte[] rcMode = new byte[4];
        byte[] forceKeyFrame = new byte[4];
        byte[] interpolation = new byte[1];
        byte[] crf = new byte[4];
        byte[] maxCrfRate = new byte[4];
        byte[] vbvBufferSize = new byte[4];
        byte[] streamWidth = new byte[4];
        byte[] streamHeight = new byte[4];

        // 填充所有数组
        int bias = 4;
        System.arraycopy(data, bias, bitrate, 0, 4);
        System.arraycopy(data, bias + 4, gopSize, 0, 4);
        System.arraycopy(data, bias + 8, profile, 0, 4);
        System.arraycopy(data, bias + 12, rcMode, 0, 4);
        System.arraycopy(data, bias + 16, forceKeyFrame, 0, 4);
        System.arraycopy(data, bias + 20, interpolation, 0, 1);
        System.arraycopy(data, bias + 21, crf, 0, 4);
        System.arraycopy(data, bias + 25, maxCrfRate, 0, 4);
        System.arraycopy(data, bias + 29, vbvBufferSize, 0, 4);
        System.arraycopy(data, bias + 33, streamWidth, 0, 4);
        System.arraycopy(data, bias + 37, streamHeight, 0, 4);

        // 将所有数组转换为int类型的值
        int bitrateValue = OpenGLJniCallback.byteArrayToIntLittleEndian(bitrate);
        int gopSizeValue = OpenGLJniCallback.byteArrayToIntLittleEndian(gopSize);
        int profileValue = OpenGLJniCallback.byteArrayToIntLittleEndian(profile);
        int rcModeValue = OpenGLJniCallback.byteArrayToIntLittleEndian(rcMode);
        int forceKeyFrameValue = OpenGLJniCallback.byteArrayToIntLittleEndian(forceKeyFrame);
        boolean interpolationValue = interpolation[0] != 0;
        int crfValue = OpenGLJniCallback.byteArrayToIntLittleEndian(crf);
        int maxCrfRateValue = OpenGLJniCallback.byteArrayToIntLittleEndian(maxCrfRate);
        int vbvBufferSizeValue = OpenGLJniCallback.byteArrayToIntLittleEndian(vbvBufferSize);
        int streamWidthValue = OpenGLJniCallback.byteArrayToIntLittleEndian(streamWidth);
        int streamHeightValue = OpenGLJniCallback.byteArrayToIntLittleEndian(streamHeight);

        // 打印结果
        EncodeParams encodeParams = new EncodeParams(bitrateValue, gopSizeValue, profileValue, rcModeValue, forceKeyFrameValue, interpolationValue, crfValue, maxCrfRateValue, vbvBufferSizeValue, streamWidthValue, streamHeightValue);
        LogUtil.info(TAG,encodeParams.toString());
        if (listener != null) {
            listener.onVmiEncodeParamsReceive(encodeParams);
        }
    }

    public EncodeParamsCallback() {

    }

    public void setNativeListener(NativeListener listener) {
        this.listener = listener;
    }
}
