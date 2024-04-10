/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

import com.google.gson.annotations.SerializedName;

/**
 * 模板信息实体类
 *
 * @author zhh
 * @since 2019-09-24
 */
public class TemplateBean {
    private int id;
    private String name;
    private int density;
    private String ram;
    private String volume;
    private int stateful;
    private String resolution;
    private Image image;
    private boolean open;
    @SerializedName("internal_storage")
    private String mInternalStorage;
    @SerializedName("sd_card")
    private String mSdCard;
    private int vcpu;
    @SerializedName("image_id")
    private int mImageId;
    @SerializedName("template_type")
    private int mTemplateType;
    @SerializedName("mTypeName")
    private String mTypeName;
    @SerializedName("mExpireTime")
    private int mExpireTime;
    @SerializedName("step_size")
    private int mStepSize;

    /**
     * getId
     *
     * @return int
     */
    public int getId() {
        return id;
    }

    /**
     * setId
     *
     * @param id int
     */
    public void setId(int id) {
        this.id = id;
    }

    /**
     * getName
     *
     * @return String
     */
    public String getName() {
        return name;
    }

    /**
     * setName
     *
     * @param name String
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * getDensity
     *
     * @return int
     */
    public int getDensity() {
        return density;
    }

    /**
     * setDensity
     *
     * @param density int
     */
    public void setDensity(int density) {
        this.density = density;
    }

    /**
     * getRam
     *
     * @return String
     */
    public String getRam() {
        return ram;
    }

    /**
     * setRam
     *
     * @param ram String
     */
    public void setRam(String ram) {
        this.ram = ram;
    }

    /**
     * getVolume
     *
     * @return String
     */
    public String getVolume() {
        return volume;
    }

    /**
     * setVolume
     *
     * @param volume String
     */
    public void setVolume(String volume) {
        this.volume = volume;
    }

    /**
     * getStateful
     *
     * @return int
     */
    public int getStateful() {
        return stateful;
    }

    /**
     * setStateful
     *
     * @param stateful int
     */
    public void setStateful(int stateful) {
        this.stateful = stateful;
    }

    /**
     * getResolution
     *
     * @return String
     */
    public String getResolution() {
        return resolution;
    }

    /**
     * setResolution
     *
     * @param resolution String
     */
    public void setResolution(String resolution) {
        this.resolution = resolution;
    }

    /**
     * getImage
     *
     * @return Image
     */
    public Image getImage() {
        return image;
    }

    /**
     * setImage
     *
     * @param image Image
     */
    public void setImage(Image image) {
        this.image = image;
    }

    /**
     * isOpen
     *
     * @return boolean
     */
    public boolean isOpen() {
        return open;
    }

    /**
     * setOpen
     *
     * @param open boolean
     */
    public void setOpen(boolean open) {
        this.open = open;
    }

    /**
     * getInternalStorage
     *
     * @return String
     */
    public String getInternalStorage() {
        return mInternalStorage;
    }

    /**
     * setInternalStorage
     *
     * @param internalStorage String
     */
    public void setInternalStorage(String internalStorage) {
        this.mInternalStorage = internalStorage;
    }

    /**
     * getSdCard
     *
     * @return String
     */
    public String getSdCard() {
        return mSdCard;
    }

    /**
     * setSdCard
     *
     * @param sdCard String
     */
    public void setSdCard(String sdCard) {
        this.mSdCard = sdCard;
    }

    /**
     * getVcpu
     *
     * @return int
     */
    public int getVcpu() {
        return vcpu;
    }

    /**
     * setVcpu
     *
     * @param vcpu int
     */
    public void setVcpu(int vcpu) {
        this.vcpu = vcpu;
    }

    /**
     * getImageId
     *
     * @return int
     */
    public int getImageId() {
        return mImageId;
    }

    /**
     * setImageId
     *
     * @param imageId int
     */
    public void setImageId(int imageId) {
        this.mImageId = imageId;
    }

    /**
     * getTemplateType
     *
     * @return int
     */
    public int getTemplateType() {
        return mTemplateType;
    }

    /**
     * setTemplateType
     *
     * @param templateType int
     */
    public void setTemplateType(int templateType) {
        this.mTemplateType = templateType;
    }

    /**
     * getTypeName
     *
     * @return String
     */
    public String getTypeName() {
        return mTypeName;
    }

    /**
     * setTypeName
     *
     * @param typeName String
     */
    public void setTypeName(String typeName) {
        this.mTypeName = typeName;
    }

    /**
     * getExpireTime
     *
     * @return int
     */
    public int getExpireTime() {
        return mExpireTime;
    }

    /**
     * setExpireTime
     *
     * @param expireTime int
     */
    public void setExpireTime(int expireTime) {
        this.mExpireTime = expireTime;
    }

    /**
     * getStepSize
     *
     * @return int
     */
    public int getStepSize() {
        return mStepSize;
    }

    /**
     * setStepSize
     *
     * @param stepSize int
     */
    public void setStepSize(int stepSize) {
        this.mStepSize = stepSize;
    }

    @Override
    public String toString() {
        return "TemplateBean{" +
                "id=" + id +
                ", name='" + name + '\'' +
                ", density=" + density +
                ", ram='" + ram + '\'' +
                ", volume='" + volume + '\'' +
                ", stateful=" + stateful +
                ", resolution='" + resolution + '\'' +
                ", image=" + image +
                ", open=" + open +
                ", mInternalStorage='" + mInternalStorage + '\'' +
                ", mSdCard='" + mSdCard + '\'' +
                ", vcpu=" + vcpu +
                ", mImageId=" + mImageId +
                ", mTemplateType=" + mTemplateType +
                ", mTypeName='" + mTypeName + '\'' +
                ", mExpireTime=" + mExpireTime +
                ", mStepSize=" + mStepSize +
                '}';
    }

    /**
     * Image Info
     */
    public static class Image {
        private int id;
        private String name;
        private String label;
        private String vendor;
        private String namespace;
        private int regionId;
        private boolean open;
        private String gameId;
        private boolean paramValid;
        @SerializedName("image_type")
        private int mImageType;
        @SerializedName("type_name")
        private String mTypeName;
        @SerializedName("expire_time")
        private String mExpireTime;
        @SerializedName("region_name")
        private String mRegionName;
        @SerializedName("region_alias")
        private String mRegionAlias;

        /**
         * getId
         *
         * @return int
         */
        public int getId() {
            return id;
        }

        /**
         * setId
         *
         * @param id int
         */
        public void setId(int id) {
            this.id = id;
        }

        /**
         * getName
         *
         * @return String
         */
        public String getName() {
            return name;
        }

        /**
         * setName
         *
         * @param name String
         */
        public void setName(String name) {
            this.name = name;
        }

        /**
         * getLabel
         *
         * @return String
         */
        public String getLabel() {
            return label;
        }

        /**
         * setLabel
         *
         * @param label String
         */
        public void setLabel(String label) {
            this.label = label;
        }

        /**
         * getVendor
         *
         * @return String
         */
        public String getVendor() {
            return vendor;
        }

        /**
         * setVendor
         *
         * @param vendor String
         */
        public void setVendor(String vendor) {
            this.vendor = vendor;
        }

        /**
         * getNamespace
         *
         * @return String
         */
        public String getNamespace() {
            return namespace;
        }

        /**
         * setNamespace
         *
         * @param namespace String
         */
        public void setNamespace(String namespace) {
            this.namespace = namespace;
        }

        /**
         * getRegionId
         *
         * @return int
         */
        public int getRegionId() {
            return regionId;
        }

        /**
         * setRegionId
         *
         * @param regionId int
         */
        public void setRegionId(int regionId) {
            this.regionId = regionId;
        }

        /**
         * isOpen
         *
         * @return boolean
         */
        public boolean isOpen() {
            return open;
        }

        /**
         * setOpen
         *
         * @param open boolean
         */
        public void setOpen(boolean open) {
            this.open = open;
        }

        /**
         * getGameId
         *
         * @return String
         */
        public String getGameId() {
            return gameId;
        }

        /**
         * setGameId
         *
         * @param gameId String
         */
        public void setGameId(String gameId) {
            this.gameId = gameId;
        }

        /**
         * isParamValid
         *
         * @return boolean
         */
        public boolean isParamValid() {
            return paramValid;
        }

        /**
         * setParamValid
         *
         * @param paramValid boolean
         */
        public void setParamValid(boolean paramValid) {
            this.paramValid = paramValid;
        }

        /**
         * getImageType
         *
         * @return int
         */
        public int getImageType() {
            return mImageType;
        }

        /**
         * setImageType
         *
         * @param imageType int
         */
        public void setImageType(int imageType) {
            this.mImageType = imageType;
        }

        /**
         * getTypeName
         *
         * @return String
         */
        public String getTypeName() {
            return mTypeName;
        }

        /**
         * setTypeName
         *
         * @param typeName String
         */
        public void setTypeName(String typeName) {
            this.mTypeName = typeName;
        }

        /**
         * getExpireTime
         *
         * @return String
         */
        public String getExpireTime() {
            return mExpireTime;
        }

        /**
         * setExpireTime
         *
         * @param expireTime String
         */
        public void setExpireTime(String expireTime) {
            this.mExpireTime = expireTime;
        }

        /**
         * getRegionName
         *
         * @return String
         */
        public String getRegionName() {
            return mRegionName;
        }

        /**
         * setRegionName
         *
         * @param regionName String
         */
        public void setRegionName(String regionName) {
            this.mRegionName = regionName;
        }

        /**
         * getRegionAlias
         *
         * @return String
         */
        public String getRegionAlias() {
            return mRegionAlias;
        }

        /**
         * setRegionAlias
         *
         * @param regionAlias String
         */
        public void setRegionAlias(String regionAlias) {
            this.mRegionAlias = regionAlias;
        }

        @Override
        public String toString() {
            return "Image{" +
                    "id=" + id +
                    ", name='" + name + '\'' +
                    ", label='" + label + '\'' +
                    ", vendor='" + vendor + '\'' +
                    ", namespace='" + namespace + '\'' +
                    ", regionId=" + regionId +
                    ", open=" + open +
                    ", gameId='" + gameId + '\'' +
                    ", paramValid=" + paramValid +
                    ", mImageType=" + mImageType +
                    ", mTypeName='" + mTypeName + '\'' +
                    ", mExpireTime='" + mExpireTime + '\'' +
                    ", mRegionName='" + mRegionName + '\'' +
                    ", mRegionAlias='" + mRegionAlias + '\'' +
                    '}';
        }
    }
}