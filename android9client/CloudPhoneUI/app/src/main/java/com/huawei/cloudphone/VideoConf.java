package com.huawei.cloudphone;

import android.os.Parcel;
import android.os.Parcelable;

public class VideoConf implements Parcelable {
    private String ip;
    private int port;
    private int audioPort;
    private int width;
    private int height;
    private int density;
    private int gameID;
    private int encodeMode;
    private int frameRate;
    private int bitrate;
    private int profile;
    private int gopSize;
    private int mDecodeMode;
    private boolean mIsAudience;

    public int getGameID() {
        return gameID;
    }

    public void setGameID(int gameID) {
        this.gameID = gameID;
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

    public int getmDecodeMode() {
        return mDecodeMode;
    }

    public void setmDecodeMode(int mDecodeMode) {
        this.mDecodeMode = mDecodeMode;
    }

    public boolean getmIsAudience() {
        return mIsAudience;
    }

    public void setmIsAudience(boolean mIsAudience) {
        this.mIsAudience = mIsAudience;
    }

    public int getAudioPort() {
        return audioPort;
    }

    public void setAudioPort(int audioPort) {
        this.audioPort = audioPort;
    }



    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getDensity() {
        return density;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public void setDensity(int density) {
        this.density = density;
    }


    protected VideoConf(Parcel in) {
        if (in == null) {
            return;
        }
        this.ip = in.readString();
        this.port = in.readInt();
        this.audioPort = in.readInt();
        this.gameID = in.readInt();
        this.encodeMode = in.readInt();
        this.frameRate = in.readInt();
        this.bitrate = in.readInt();
        this.profile = in.readInt();
        this.gopSize = in.readInt();
        this.mDecodeMode = in.readInt();
        this.mIsAudience = in.readByte() != 0;

    }

    public VideoConf() {
        this(null);
    }

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

    public String getIp() {
        return ip;
    }

    public int getPort() {
        return port;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public void setPort(int port) {
        this.port = port;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(ip);
        dest.writeInt(port);
        dest.writeInt(audioPort);
        dest.writeInt(width);
        dest.writeInt(height);
        dest.writeInt(density);
        dest.writeInt(gameID);
        dest.writeInt(encodeMode);
        dest.writeInt(frameRate);
        dest.writeInt(bitrate);
        dest.writeInt(profile);
        dest.writeInt(gopSize);
        dest.writeInt(mDecodeMode);
        dest.writeByte((byte) (mIsAudience ? 1 : 0));
    }
}
