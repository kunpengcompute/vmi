package com.huawei.cloudphonesdk.maincontrol.config;

public class EncodeParams {
    protected int bitrate = 2000000;                          // 码率
    protected int gopSize = 30;                               // I帧间隔
    protected int profile = 1;   // 编码复杂度
    protected int rcMode = 2;               // 流控模式
    protected int forceKeyFrame = 0;                          // 在设置后第N帧强制生成I帧，0表示不生效
    protected boolean interpolation = false;                  // 补帧开关
    protected int crf = 34;
    protected int maxCrfRate = 20000000;
    protected int vbvBufferSize = 1000;
    protected int streamWidth = 720;
    protected int streamHeight = 1280;

    public EncodeParams(int bitrate, int gopSize, int profile, int rcMode, int forceKeyFrame, boolean interpolation, int crf, int maxCrfRate, int vbvBufferSize, int streamWidth, int streamHeight) {
        this.bitrate = bitrate;
        this.gopSize = gopSize;
        this.profile = profile;
        this.rcMode = rcMode;
        this.forceKeyFrame = forceKeyFrame;
        this.interpolation = interpolation;
        this.crf = crf;
        this.maxCrfRate = maxCrfRate;
        this.vbvBufferSize = vbvBufferSize;
        this.streamWidth = streamWidth;
        this.streamHeight = streamHeight;
    }

    public EncodeParams() {

    }

    public int getBitrate() {
        return bitrate;
    }

    public void setBitrate(int bitrate) {
        this.bitrate = bitrate;
    }

    public int getMaxCrfRate() {
        return maxCrfRate;
    }

    public void setMaxCrfRate(int maxCrfRate) {
        this.maxCrfRate = maxCrfRate;
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

    public int getCrf() {
        return crf;
    }

    public void setCrf(int crf) {
        this.crf = crf;
    }

    public int getVbvBufferSize() {
        return vbvBufferSize;
    }

    public void setVbvBufferSize(int vbvBufferSize) {
        this.vbvBufferSize = vbvBufferSize;
    }

    public int getStreamWidth() {
        return streamWidth;
    }

    public void setStreamWidth(int streamWidth) {
        this.streamWidth = streamWidth;
    }

    public int getStreamHeight() {
        return streamHeight;
    }

    public void setStreamHeight(int streamHeight) {
        this.streamHeight = streamHeight;
    }

    @Override
    public String toString() {
        return "EncodeParams{" +
                "bitrate=" + bitrate +
                ", gopSize=" + gopSize +
                ", profile=" + profile +
                ", rcMode=" + rcMode +
                ", forceKeyFrame=" + forceKeyFrame +
                ", interpolation=" + interpolation +
                ", crf=" + crf +
                ", maxCrfRate=" + maxCrfRate +
                ", vbvBufferSize=" + vbvBufferSize +
                ", streamWidth=" + streamWidth +
                ", streamHeight=" + streamHeight +
                '}';
    }
}
