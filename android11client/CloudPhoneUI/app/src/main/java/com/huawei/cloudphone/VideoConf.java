package com.huawei.cloudphone;

import android.os.Parcel;
import android.os.Parcelable;

public class VideoConf implements Parcelable {
    private String ip;
    private int port;
    private int width;
    private int height;
    private int density;

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
        if (in == null){
            return;
        }
        ip = in.readString();
        port = in.readInt();
    }

    public VideoConf(){
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
        dest.writeInt(width);
        dest.writeInt(height);
        dest.writeInt(density);
    }
}
