/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

import android.os.Parcel;
import android.os.Parcelable;

import com.google.gson.annotations.SerializedName;

import java.util.List;

/**
 * 设备列表信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public class DeviceListBean extends Response {
    private List<DeviceBean> listData;

    /**
     * getListData
     *
     * @return List
     */
    public List<DeviceBean> getListData() {
        return listData;
    }

    /**
     * setListData
     *
     * @param listData List<DeviceBean>
     */
    public void setListData(List<DeviceBean> listData) {
        this.listData = listData;
    }

    @Override
    public String toString() {
        return "DeviceListBean{" +
            "listData=" + listData +
            '}';
    }

    /**
     * 设备信息
     */
    public static class DeviceBean implements Parcelable {
        /**
         * DeviceBean CREATOR
         */
        public static final Creator<DeviceBean> CREATOR = new Creator<DeviceBean>() {
            @Override
            public DeviceBean createFromParcel(Parcel in) {
                return new DeviceBean(in);
            }

            @Override
            public DeviceBean[] newArray(int size) {
                return new DeviceBean[size];
            }
        };
        @SerializedName("device_Resolution")
        private String mDeviceResolution;
        @SerializedName("mini_port")
        private int mMiniPort;
        @SerializedName("device_ram")
        private String mDeviceRam;
        @SerializedName("device_id")
        private int mDeviceId;
        @SerializedName("device_imageName")
        private String mDeviceImageName;
        @SerializedName("vm_port")
        private int mVmPort;
        @SerializedName("device_CPU")
        private int mDeviceCpu;
        @SerializedName("device_createTime")
        private long mDeviceCreateTime;
        @SerializedName("video_stream_port")
        private int mVideoStreamPort;
        @SerializedName("device_alias")
        private String mDeviceAlias = "";
        @SerializedName("device_onlineTime")
        private long mDeviceOnlineTime;
        @SerializedName("device_VmStatus")
        private int mDeviceVmStatus;
        @SerializedName("device_storage")
        private String mDeviceStorage;
        @SerializedName("device_SD")
        private String mDeviceSd;
        @SerializedName("vm_ip")
        private String mVmIp = "";
        private int decodeMethod = 2; // 1:soft decode 2:hard decode
        @SerializedName("device_status")
        private int mDeviceStatus;
        
        protected DeviceBean(Parcel in) {
            mDeviceResolution = in.readString();
            mMiniPort = in.readInt();
            mDeviceRam = in.readString();
            mDeviceId = in.readInt();
            mDeviceImageName = in.readString();
            mVmPort = in.readInt();
            mDeviceCpu = in.readInt();
            mDeviceCreateTime = in.readLong();
            mVideoStreamPort = in.readInt();
            mDeviceAlias = in.readString();
            mDeviceOnlineTime = in.readLong();
            mDeviceVmStatus = in.readInt();
            mDeviceStorage = in.readString();
            mDeviceSd = in.readString();
            mVmIp = in.readString();
            decodeMethod = in.readInt();
            mDeviceStatus = in.readInt();
        }

        /**
         * getDeviceResolution
         *
         * @return String
         */
        public String getDeviceResolution() {
            return mDeviceResolution;
        }

        /**
         * setDeviceResolution
         *
         * @param deviceResolution String
         */
        public void setDeviceResolution(String deviceResolution) {
            this.mDeviceResolution = deviceResolution;
        }

        /**
         * getMiniPort
         *
         * @return int
         */
        public int getMiniPort() {
            return mMiniPort;
        }

        /**
         * setMiniPort
         *
         * @param miniPort int
         */
        public void setMiniPort(int miniPort) {
            this.mMiniPort = miniPort;
        }

        /**
         * getDeviceRam
         *
         * @return String
         */
        public String getDeviceRam() {
            return mDeviceRam;
        }

        /**
         * setDeviceRam
         *
         * @param deviceRam String
         */
        public void setDeviceRam(String deviceRam) {
            this.mDeviceRam = deviceRam;
        }

        /**
         * getDeviceId
         *
         * @return int
         */
        public int getDeviceId() {
            return mDeviceId;
        }

        /**
         * setDeviceId
         *
         * @param deviceId int
         */
        public void setDeviceId(int deviceId) {
            this.mDeviceId = deviceId;
        }

        /**
         * getDeviceImageName
         *
         * @return String
         */
        public String getDeviceImageName() {
            return mDeviceImageName;
        }

        /**
         * setDeviceImageName
         *
         * @param deviceImageName String
         */
        public void setDeviceImageName(String deviceImageName) {
            this.mDeviceImageName = deviceImageName;
        }

        /**
         * getVmPort
         *
         * @return int
         */
        public int getVmPort() {
            return mVmPort;
        }

        /**
         * setVmPort
         *
         * @param vmPort int
         */
        public void setVmPort(int vmPort) {
            this.mVmPort = vmPort;
        }

        /**
         * getDeviceCpu
         *
         * @return int
         */
        public int getDeviceCpu() {
            return mDeviceCpu;
        }

        /**
         * setDeviceCpu
         *
         * @param deviceCpu int
         */
        public void setDeviceCpu(int deviceCpu) {
            this.mDeviceCpu = deviceCpu;
        }

        /**
         * getDeviceCreateTime
         *
         * @return long
         */
        public long getDeviceCreateTime() {
            return mDeviceCreateTime;
        }

        /**
         * setDeviceCreateTime
         *
         * @param deviceCreateTime long
         */
        public void setDeviceCreateTime(long deviceCreateTime) {
            this.mDeviceCreateTime = deviceCreateTime;
        }

        /**
         * getVideoStreamPort
         *
         * @return int
         */
        public int getVideoStreamPort() {
            return mVideoStreamPort;
        }

        /**
         * setVideoStreamPort
         *
         * @param videoStreamPort int
         */
        public void setVideoStreamPort(int videoStreamPort) {
            this.mVideoStreamPort = videoStreamPort;
        }

        /**
         * getDeviceAlias
         *
         * @return String
         */
        public String getDeviceAlias() {
            return mDeviceAlias;
        }

        /**
         * setDeviceAlias
         *
         * @param deviceAlias String
         */
        public void setDeviceAlias(String deviceAlias) {
            this.mDeviceAlias = deviceAlias;
        }

        /**
         * getDeviceOnlineTime
         *
         * @return long
         */
        public long getDeviceOnlineTime() {
            return mDeviceOnlineTime;
        }

        /**
         * setDeviceOnlineTime
         *
         * @param deviceOnlineTime long
         */
        public void setDeviceOnlineTime(long deviceOnlineTime) {
            this.mDeviceOnlineTime = deviceOnlineTime;
        }

        /**
         * getDeviceVmStatus
         *
         * @return int
         */
        public int getDeviceVmStatus() {
            return mDeviceVmStatus;
        }

        /**
         * setDeviceVmStatus
         *
         * @param deviceVmStatus int
         */
        public void setDeviceVmStatus(int deviceVmStatus) {
            this.mDeviceVmStatus = deviceVmStatus;
        }

        /**
         * getDeviceStorage
         *
         * @return String
         */
        public String getDeviceStorage() {
            return mDeviceStorage;
        }

        /**
         * setDeviceStorage
         *
         * @param deviceStorage String
         */
        public void setDeviceStorage(String deviceStorage) {
            this.mDeviceStorage = deviceStorage;
        }

        /**
         * getDeviceSd
         *
         * @return String
         */
        public String getDeviceSd() {
            return mDeviceSd;
        }

        /**
         * setDeviceSd
         *
         * @param deviceSd String
         */
        public void setDeviceSd(String deviceSd) {
            this.mDeviceSd = deviceSd;
        }

        /**
         * getVmIp
         *
         * @return String
         */
        public String getVmIp() {
            return mVmIp;
        }

        /**
         * setVmIp
         *
         * @param vmIp String
         */
        public void setVmIp(String vmIp) {
            this.mVmIp = vmIp;
        }

        /**
         * getDecodeMethod
         *
         * @return int
         */
        public int getDecodeMethod() {
            return decodeMethod;
        }

        /**
         * setDecodeMethod
         *
         * @param decodeMethod int
         */
        public void setDecodeMethod(int decodeMethod) {
            this.decodeMethod = decodeMethod;
        }

        /**
         * getDeviceStatus
         *
         * @return int
         */
        public int getDeviceStatus() {
            return mDeviceStatus;
        }

        /**
         * setDeviceStatus
         *
         * @param deviceStatus int
         */
        public void setDeviceStatus(int deviceStatus) {
            this.mDeviceStatus = deviceStatus;
        }

        @Override
        public String toString() {
            return "DeviceBean{" +
                "mDeviceResolution='" + mDeviceResolution + '\'' +
                ", mMiniPort=" + mMiniPort +
                ", mDeviceRam='" + mDeviceRam + '\'' +
                ", mDeviceId=" + mDeviceId +
                ", mDeviceImageName='" + mDeviceImageName + '\'' +
                ", mVmPort=" + mVmPort +
                ", mDeviceCpu=" + mDeviceCpu +
                ", mDeviceCreateTime=" + mDeviceCreateTime +
                ", mVideoStreamPort=" + mVideoStreamPort +
                ", mDeviceAlias='" + mDeviceAlias + '\'' +
                ", mDeviceOnlineTime=" + mDeviceOnlineTime +
                ", mDeviceVmStatus=" + mDeviceVmStatus +
                ", mDeviceStorage='" + mDeviceStorage + '\'' +
                ", mDeviceSd='" + mDeviceSd + '\'' +
                ", mVmIp='" + mVmIp + '\'' +
                ", decodeMethod=" + decodeMethod +
                ", mDeviceStatus=" + mDeviceStatus +
                '}';
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel parcel, int i) {
            parcel.writeString(mDeviceResolution);
            parcel.writeInt(mMiniPort);
            parcel.writeString(mDeviceRam);
            parcel.writeInt(mDeviceId);
            parcel.writeString(mDeviceImageName);
            parcel.writeInt(mVmPort);
            parcel.writeInt(mDeviceCpu);
            parcel.writeLong(mDeviceCreateTime);
            parcel.writeInt(mVideoStreamPort);
            parcel.writeString(mDeviceAlias);
            parcel.writeLong(mDeviceOnlineTime);
            parcel.writeInt(mDeviceVmStatus);
            parcel.writeString(mDeviceStorage);
            parcel.writeString(mDeviceSd);
            parcel.writeString(mVmIp);
            parcel.writeInt(decodeMethod);
            parcel.writeInt(mDeviceStatus);
        }
    }
}