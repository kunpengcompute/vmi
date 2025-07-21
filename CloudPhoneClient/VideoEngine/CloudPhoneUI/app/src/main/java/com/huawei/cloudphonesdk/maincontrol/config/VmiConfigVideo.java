package com.huawei.cloudphonesdk.maincontrol.config;

import androidx.annotation.NonNull;

import com.huawei.cloudphonesdk.maincontrol.VideoConf;

public class VmiConfigVideo extends VmiConfig {
    // 编码器类型
    private int encoderType = 0;
    // 视频输出格式
    private int videoFrameType = 0;
    // 抓图分辨率（仅renderOptimize关闭时有效）
    private int width = 720;                       // 720: 默认720P
    private int height = 1280;                     // 1280: 默认720P
    private int widthAligned = 720;                // 720: 默认不做对齐
    private int heightAligned = 1280;              // 1280: 默认不做对齐
    private int density = 320;                            // 屏幕密度
    private boolean renderOptimize = false;               // 出流优化，默认关闭，开启时displayResolution与captureResolution无效

    // EncodeParams
    private EncodeParams encodeParams;

    public VmiConfigVideo() {
    }
    
    public EncodeParams getEncodeParams() {
	return encodeParams;
    }

    public void setEncodeParams(EncodeParams encodeParams) {
	this.encodeParams = encodeParams;
    }

    public int getEncoderType() {
        return encoderType;
    }

    public void setEncoderType(int encoderType) {
        this.encoderType = encoderType;
    }

    public int getVideoFrameType() {
        return videoFrameType;
    }

    public void setVideoFrameType(int videoFrameType) {
        this.videoFrameType = videoFrameType;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getWidthAligned() {
        return widthAligned;
    }

    public void setWidthAligned(int widthAligned) {
        this.widthAligned = widthAligned;
    }

    public int getHeightAligned() {
        return heightAligned;
    }

    public void setHeightAligned(int heightAligned) {
        this.heightAligned = heightAligned;
    }

    public int getDensity() {
        return density;
    }

    public void setDensity(int density) {
        this.density = density;
    }

    public boolean isRenderOptimize() {
        return renderOptimize;
    }

    public void setRenderOptimize(boolean renderOptimize) {
        this.renderOptimize = renderOptimize;
    }

    enum VideoFrameType {
        H264,
        YUV,                                        // YV12
        RGB,                                        // RGBA8888，暂不支持
        H265,
        FRAME_TYPE_MAX
    }

    // 编码模式
    enum RCMode {
        ABR,                                        // 平均码率，暂不支持
        CRF,                                        // 画质优先，暂不支持
        CBR,                                        // 恒定码率
        CAPPED_CRF,                                 // 画质优先，但限制码率，暂不支持
        RC_MODE_MAX
    }

    // 编码器类型
    enum EncoderType {
        CPU,                                        // 软编
        VPU,                                        // 编码卡硬件加速
        GPU,                                        // GPU硬件加速，暂不支持
        ENCODE_TYPE_MAX
    }


    // 编码级别
    enum ProfileType {
        BASELINE,                                   // H264支持
        MAIN,                                       // H264、H265支持
        HIGH                                        // H264支持
    }

    enum AudioType {
        OPUS,
        PCM
    }

    enum VmiDataType {
        VIDEO,                                      // 码流出流组件
        AUDIO,                                      // 音频播放组件
        TOUCH,                                      // 触控和按键组件
        MIC,                                        // 麦克风组件
        DATA_TYPE_MAX
    }

    @Override
    public String toString() {
        return "VmiConfigVideo{" +
                "version=" + getVersion() +
                ", encoderType=" + encoderType +
                ", videoFrameType=" + videoFrameType +
                ", width=" + width +
                ", height=" + height +
                ", widthAligned=" + widthAligned +
                ", heightAligned=" + heightAligned +
                ", renderOptimize=" + renderOptimize +
                ", bitrate=" + encodeParams.bitrate +
                ", gopSize=" + encodeParams.gopSize +
                ", profile=" + encodeParams.profile +
                ", rcMode=" + encodeParams.rcMode +
                ", forceKeyFrame=" + encodeParams.forceKeyFrame +
                ", interpolation=" + encodeParams.interpolation +
                ", crf=" + encodeParams.crf +
                ", maxCrfRate=" + encodeParams.maxCrfRate +
                ", vbvBufferSize=" + encodeParams.vbvBufferSize +
                ", streamWidth=" + encodeParams.streamWidth +
                ", streamHeight=" + encodeParams.streamHeight +
                '}';
    }
}
