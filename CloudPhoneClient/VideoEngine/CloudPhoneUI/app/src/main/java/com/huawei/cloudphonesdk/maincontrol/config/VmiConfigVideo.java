package com.huawei.cloudphonesdk.maincontrol.config;

import androidx.annotation.NonNull;

public class VmiConfigVideo {

    // VmiConfig
    private int version = 1;
    // 编码器类型
    private int encoderType = 0;
    // 视频输出格式
    private int videoFrameType = 0;
    // 帧率
    private int frameRate = 30;
    // 抓图分辨率（仅renderOptimize关闭时有效）
    private int width = 720;                       // 720: 默认720P
    private int height = 1280;                     // 1280: 默认720P
    private int widthAligned = 720;                // 720: 默认不做对齐
    private int heightAligned = 1280;              // 1280: 默认不做对齐

    private boolean forceLandscape = false;               // 强制横屏
    private boolean renderOptimize = false;               // 出流优化，默认关闭，开启时displayResolution与captureResolution无效

    // EncodeParams
    private int bitrate = 3000000;                          // 码率
    private int gopSize = 30;                               // I帧间隔
    private int profile = 1;   // 编码复杂度
    private int rcMode = 2;               // 流控模式
    private int forceKeyFrame = 0;                          // 在设置后第N帧强制生成I帧，0表示不生效
    private boolean interpolation = false;                  // 补帧开关

    public int getVersion() {
        return version;
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

    public int getFrameRate() {
        return frameRate;
    }

    public void setFrameRate(int frameRate) {
        this.frameRate = frameRate;
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

    public boolean isForceLandscape() {
        return forceLandscape;
    }

    public void setForceLandscape(boolean forceLandscape) {
        this.forceLandscape = forceLandscape;
    }

    public boolean isRenderOptimize() {
        return renderOptimize;
    }

    public void setRenderOptimize(boolean renderOptimize) {
        this.renderOptimize = renderOptimize;
    }

    public int getBitrate() {
        return bitrate;
    }

    public void setBitrate(int bitrate) {
        this.bitrate = bitrate;
    }

    public int getGopSize() {
        return gopSize;
    }

    public void setGopSize(int gopSize) {
        this.gopSize = gopSize;
    }

    public int getProfile() {
        return profile;
    }

    public void setProfile(int profile) {
        this.profile = profile;
    }

    public int getRcMode() {
        return rcMode;
    }

    public void setRcMode(int rcMode) {
        this.rcMode = rcMode;
    }

    public int getForceKeyFrame() {
        return forceKeyFrame;
    }

    public void setForceKeyFrame(int forceKeyFrame) {
        this.forceKeyFrame = forceKeyFrame;
    }

    public boolean isInterpolation() {
        return interpolation;
    }

    public void setInterpolation(boolean interpolation) {
        this.interpolation = interpolation;
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

    @NonNull
    @Override
    public String toString() {
        return "VimConfigVideo{" +
            "version=" + version +
            ", encoderType=" + encoderType +
            ", videoFrameType=" + videoFrameType +
            ", frameRate=" + frameRate +
            ", width=" + width +
            ", height=" + height +
            ", widthAligned=" + widthAligned +
            ", heightAligned=" + heightAligned +
            ", forceLandscape=" + forceLandscape +
            ", renderOptimize=" + renderOptimize +
            ", bitrate=" + bitrate +
            ", gopSize=" + gopSize +
            ", profile=" + profile +
            ", rcMode=" + rcMode +
            ", forceKeyFrame=" + forceKeyFrame +
            ", interpolation=" + interpolation +
            '}';
    }
}