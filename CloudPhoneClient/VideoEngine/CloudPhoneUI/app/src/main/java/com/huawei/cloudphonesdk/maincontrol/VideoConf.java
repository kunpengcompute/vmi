/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.Keep;

/**
 * VideoConf.
 *
 * @since 2018-07-05
 */
@Keep
public class VideoConf implements Parcelable {
    /**
     * Parcelable Creator
     */
    public static final Creator<VideoConf> CREATOR = new Creator<VideoConf>() {
        @Override
        public VideoConf createFromParcel(Parcel in) {
            return new VideoConf(in);
        }

        @Override
        public VideoConf[] newArray(int size) {
            return new VideoConf[size];
        }
    };
    private boolean testTouch;
    private boolean testAudioClient;
    private String ip;
    private int videoAgentPort;
    private int vmiAgentPort;
    private int gameId;
    private int encodeMode;
    private int bitrate;
    private int profile;
    private int gopSize;
    private int width;
    private int height;
    private int density;
    private int mDecodeMode;
    private boolean mIsAudience;
    private int mTemplateType;
    private int isRecord;
    private int frameRate;
    // video配置
    private int videoEncoderType;
    private int videoFrameType;
    private int videoFrameRate;
    private boolean videoForceLandscape;
    private boolean videoRenderOptimize;
    private int videoFrameSizeWidth;
    private int videoFrameSizeHeight;
    private int videoFrameSizeWidthAligned;
    private int videoFrameSizeHeightAligned;
    // 编码参数
    private int videoBitRate;
    private int videoRcMode;
    private int videoForceKeyFrame;
    private int videoInterpolation;
    private int videoProFile;
    private int videoGopSize;
    // 音频播放参数
    private int audioSampleInterval;
    private int audioPlayBitrate;
    private int audioPlayStreamType;
    private int micStreamType;

    public int getMicSampleInterval() {
        return micSampleInterval;
    }

    public void setMicSampleInterval(int micSampleInterval) {
        this.micSampleInterval = micSampleInterval;
    }

    private int micSampleInterval;

    public int getVideoEncoderType() {
        return videoEncoderType;
    }

    public void setVideoEncoderType(int videoEncoderType) {
        this.videoEncoderType = videoEncoderType;
    }

    public int getVideoFrameType() {
        return videoFrameType;
    }

    public void setVideoFrameType(int videoFrameType) {
        this.videoFrameType = videoFrameType;
    }

    public int getVideoFrameRate() {
        return videoFrameRate;
    }

    public void setVideoFrameRate(int videoFrameRate) {
        this.videoFrameRate = videoFrameRate;
    }

    public boolean getVideoForceLandscape() {
        return videoForceLandscape;
    }

    public void setVideoForceLandscape(boolean videoForceLandscape) {
        this.videoForceLandscape = videoForceLandscape;
    }

    public boolean getVideoRenderOptimize() {
        return videoRenderOptimize;
    }

    public void setVideoRenderOptimize(boolean videoRenderOptimize) {
        this.videoRenderOptimize = videoRenderOptimize;
    }

    public int getVideoFrameSizeWidth() {
        return videoFrameSizeWidth;
    }

    public void setVideoFrameSizeWidth(int videoFrameSizeWidth) {
        this.videoFrameSizeWidth = videoFrameSizeWidth;
    }

    public int getVideoFrameSizeHeight() {
        return videoFrameSizeHeight;
    }

    public void setVideoFrameSizeHeight(int videoFrameSizeHeight) {
        this.videoFrameSizeHeight = videoFrameSizeHeight;
    }

    public int getVideoFrameSizeWidthAligned() {
        return videoFrameSizeWidthAligned;
    }

    public void setVideoFrameSizeWidthAligned(int videoFrameSizeWidthAligned) {
        this.videoFrameSizeWidthAligned = videoFrameSizeWidthAligned;
    }

    public int getVideoFrameSizeHeightAligned() {
        return videoFrameSizeHeightAligned;
    }

    public void setVideoFrameSizeHeightAligned(int videoFrameSizeHeightAligned) {
        this.videoFrameSizeHeightAligned = videoFrameSizeHeightAligned;
    }

    public int getVideoBitRate() {
        return videoBitRate;
    }

    public void setVideoBitRate(int videoBitRate) {
        this.videoBitRate = videoBitRate;
    }

    public int getVideoRcMode() {
        return videoRcMode;
    }

    public void setVideoRcMode(int videoRcMode) {
        this.videoRcMode = videoRcMode;
    }

    public int getVideoForceKeyFrame() {
        return videoForceKeyFrame;
    }

    public void setVideoForceKeyFrame(int videoForceKeyFrame) {
        this.videoForceKeyFrame = videoForceKeyFrame;
    }

    public int getVideoInterpolation() {
        return videoInterpolation;
    }

    public void setVideoInterpolation(int videoInterpolation) {
        this.videoInterpolation = videoInterpolation;
    }

    public int getVideoProFile() {
        return videoProFile;
    }

    public void setVideoProFile(int videoProFile) {
        this.videoProFile = videoProFile;
    }

    public int getVideoGopSize() {
        return videoGopSize;
    }

    public void setVideoGopSize(int videoGopSize) {
        this.videoGopSize = videoGopSize;
    }

    public int getAudioSampleInterval() {
        return audioSampleInterval;
    }

    public void setAudioSampleInterval(int audioSampleInterval) {
        this.audioSampleInterval = audioSampleInterval;
    }

    public int getAudioPlayBitrate() {
        return audioPlayBitrate;
    }

    public void setAudioPlayBitrate(int audioPlayBitrate) {
        this.audioPlayBitrate = audioPlayBitrate;
    }

    public int getAudioPlayStreamType() {
        return audioPlayStreamType;
    }

    public void setAudioPlayStreamType(int audioPlayStreamType) {
        this.audioPlayStreamType = audioPlayStreamType;
    }

    public int getMicStreamType() {
        return micStreamType;
    }

    public void setMicStreamType(int micStreamType) {
        this.micStreamType = micStreamType;
    }


    public int isRecord() {
        return isRecord;
    }

    public void setRecord(int record) {
        this.isRecord = record;
    }

    /**
     * constructor
     */
    public VideoConf(Parcel in) {
        if (in == null) {
            return;
        }
        testTouch = in.readByte() != 0;
        testAudioClient = in.readByte() != 0;
        ip = in.readString();
        videoAgentPort = in.readInt();
        vmiAgentPort = in.readInt();
        gameId = in.readInt();
        encodeMode = in.readInt();
        frameRate = in.readInt();
        // opus编码比特率
        bitrate = in.readInt();
        profile = in.readInt();
        gopSize = in.readInt();
        width = in.readInt();
        height = in.readInt();
        density = in.readInt();
        mDecodeMode = in.readInt();
        mIsAudience = in.readByte() != 0;
        mTemplateType = in.readInt();
        isRecord = in.readInt();

        videoEncoderType = in.readInt();
        videoFrameType = in.readInt();
        videoFrameRate = in.readInt();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            videoRenderOptimize = in.readBoolean();
            videoForceLandscape = in.readBoolean();
        }
        videoFrameSizeWidth = in.readInt();
        videoFrameSizeHeight = in.readInt();
        videoFrameSizeWidthAligned = in.readInt();
        videoFrameSizeHeightAligned = in.readInt();

        videoBitRate = in.readInt();
        videoRcMode = in.readInt();
        videoForceKeyFrame = in.readInt();
        videoInterpolation = in.readInt();
        videoProFile = in.readInt();
        videoGopSize = in.readInt();

        audioSampleInterval = in.readInt();
        audioPlayBitrate = in.readInt();
        audioPlayStreamType = in.readInt();
        micStreamType = in.readInt();
        micSampleInterval = in.readInt();
    }

    /**
     * constructor
     */
    public VideoConf() {
        this(null);
    }

    public String getIp() {
        return ip;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public int getVideoAgentPort() {
        return videoAgentPort;
    }

    public void setVideoAgentPort(int videoAgentPort) {
        this.videoAgentPort = videoAgentPort;
    }

    public int getVmiAgentPort() {
        return vmiAgentPort;
    }

    public void setVmiAgentPort(int vmiAgentPort) {
        this.vmiAgentPort = vmiAgentPort;
    }

    public int getGameId() {
        return gameId;
    }

    public void setGameId(int gameId) {
        this.gameId = gameId;
    }

    public int getEncodeMode() {
        return encodeMode;
    }

    public void setEncodeMode(int encodeMode) {
        this.encodeMode = encodeMode;
    }

    public int getFrameRate() {
        return frameRate;
    }

    public void setFrameRate(int frameRate) {
        this.frameRate = frameRate;
    }

    public int getBitrate() {
        return bitrate;
    }

    public void setBitrate(int bitrate) {
        this.bitrate = bitrate;
    }

    public int getProfile() {
        return profile;
    }

    public void setProfile(int profile) {
        this.profile = profile;
    }

    public int getGopSize() {
        return gopSize;
    }

    public void setGopSize(int gopSize) {
        this.gopSize = gopSize;
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

    /**
     * getDensity
     *
     * @return density
     */
    public int getDensity() {
        return density;
    }

    /**
     * setDensity
     *
     * @param density density
     */
    public void setDensity(int density) {
        this.density = density;
    }

    public boolean isTestTouch() {
        return testTouch;
    }

    public void setTestTouch(boolean testTouch) {
        this.testTouch = testTouch;
    }

    public boolean isTestAudioClient() {
        return testAudioClient;
    }

    public void setTestAudioClient(boolean testAudioClient) {
        this.testAudioClient = testAudioClient;
    }

    /**
     * 设置模板类型
     *
     * @param mTemplateType int
     */
    public void setTemplateType(int mTemplateType) {
        this.mTemplateType = mTemplateType;
    }

    @Override
    public String toString() {
        return "VideoConf{" +
            "testTouch=" + testTouch +
            ", testAudioClient=" + testAudioClient +
            ", ip='" + ip + '\'' +
            ", videoAgentPort=" + videoAgentPort +
            ", vmiAgentPort=" + vmiAgentPort +
            ", gameId=" + gameId +
            ", encodeMode=" + encodeMode +
            ", bitrate=" + bitrate +
            ", profile=" + profile +
            ", gopSize=" + gopSize +
            ", width=" + width +
            ", height=" + height +
            ", density=" + density +
            ", mDecodeMode=" + mDecodeMode +
            ", mIsAudience=" + mIsAudience +
            ", mTemplateType=" + mTemplateType +
            ", isRecord=" + isRecord +
            ", frameRate=" + frameRate +
            ", videoEncoderType=" + videoEncoderType +
            ", videoFrameType=" + videoFrameType +
            ", videoFrameRate=" + videoFrameRate +
            ", videoForceLandscape=" + videoForceLandscape +
            ", videoRenderOptimize=" + videoRenderOptimize +
            ", videoFrameSizeWidth=" + videoFrameSizeWidth +
            ", videoFrameSizeHeight=" + videoFrameSizeHeight +
            ", videoFrameSizeWidthAligned=" + videoFrameSizeWidthAligned +
            ", videoFrameSizeHeightAligned=" + videoFrameSizeHeightAligned +
            ", videoBitRate=" + videoBitRate +
            ", videoRcMode=" + videoRcMode +
            ", videoForceKeyFrame=" + videoForceKeyFrame +
            ", videoInterpolation=" + videoInterpolation +
            ", videoProFile=" + videoProFile +
            ", videoGopSize=" + videoGopSize +
            ", audioSampleInterval=" + audioSampleInterval +
            ", audioPlayBitrate=" + audioPlayBitrate +
            ", audioPlayStreamType=" + audioPlayStreamType +
            ", micStreamType=" + micStreamType +
            ", micSampleInterval=" + micSampleInterval +
            '}';
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int flags) {
        parcel.writeByte((byte) (testTouch ? 1 : 0));
        parcel.writeByte((byte) (testAudioClient ? 1 : 0));
        parcel.writeString(ip);
        parcel.writeInt(videoAgentPort);
        parcel.writeInt(vmiAgentPort);
        parcel.writeInt(gameId);
        parcel.writeInt(encodeMode);
        parcel.writeInt(frameRate);
        parcel.writeInt(bitrate);
        parcel.writeInt(profile);
        parcel.writeInt(gopSize);
        parcel.writeInt(width);
        parcel.writeInt(height);
        parcel.writeInt(density);
        parcel.writeInt(mDecodeMode);
        parcel.writeByte((byte) (mIsAudience ? 1 : 0));
        parcel.writeInt(mTemplateType);
        parcel.writeInt(isRecord);

        parcel.writeInt(videoEncoderType);
        parcel.writeInt(videoFrameType);
        parcel.writeInt(videoFrameRate);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            parcel.writeBoolean(videoForceLandscape);
            parcel.writeBoolean(videoRenderOptimize);
        }
        parcel.writeInt(videoFrameSizeWidth);
        parcel.writeInt(videoFrameSizeHeight);
        parcel.writeInt(videoFrameSizeWidthAligned);
        parcel.writeInt(videoFrameSizeHeightAligned);

        parcel.writeInt(videoBitRate);
        parcel.writeInt(videoRcMode);
        parcel.writeInt(videoInterpolation);
        parcel.writeInt(videoProFile);
        parcel.writeInt(videoGopSize);
        parcel.writeInt(videoForceKeyFrame);

        parcel.writeInt(audioSampleInterval);
        parcel.writeInt(audioPlayBitrate);
        parcel.writeInt(audioPlayStreamType);
        parcel.writeInt(micStreamType);
        parcel.writeInt(micSampleInterval);

    }
}
