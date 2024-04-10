/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

import com.google.gson.annotations.SerializedName;

/**
 * 申请到的设备实体类
 *
 * @author zhh
 * @since 2019-09-24
 */
public class ApplyDeviceBean extends Response {
    @SerializedName("vncport")
    private int mVncPort;
    @SerializedName("webglport")
    private int mWebGlPort;
    @SerializedName("alias")
    private String mAlias;
    @SerializedName("adbport")
    private int mAdbPort;
    @SerializedName("agentport")
    private int mAgentPort;

    /**
     * getVncPort
     *
     * @return int
     */
    public int getVncPort() {
        return mVncPort;
    }

    /**
     * setVncPort
     *
     * @param vncPort int
     */
    public void setVncPort(int vncPort) {
        this.mVncPort = vncPort;
    }

    /**
     * getWebGlPort
     *
     * @return int
     */
    public int getWebGlPort() {
        return mWebGlPort;
    }

    /**
     * setWebGlPort
     *
     * @param webGlPort int
     */
    public void setWebGlPort(int webGlPort) {
        this.mWebGlPort = webGlPort;
    }

    /**
     * getAlias
     *
     * @return String
     */
    public String getAlias() {
        return mAlias;
    }

    /**
     * setAlias
     *
     * @param alias String
     */
    public void setAlias(String alias) {
        this.mAlias = alias;
    }

    /**
     * getAdbPort
     *
     * @return int
     */
    public int getAdbPort() {
        return mAdbPort;
    }

    /**
     * setAdbPort
     *
     * @param adbPort int
     */
    public void setAdbPort(int adbPort) {
        this.mAdbPort = adbPort;
    }

    /**
     * getAgentPort
     *
     * @return int
     */
    public int getAgentPort() {
        return mAgentPort;
    }

    /**
     * setAgentPort
     *
     * @param agentPort int
     */
    public void setAgentPort(int agentPort) {
        this.mAgentPort = agentPort;
    }

    @Override
    public String toString() {
        return "ApplyDeviceBean{" +
                "mVncPort=" + mVncPort +
                ", mWebGlPort=" + mWebGlPort +
                ", mAlias='" + mAlias + '\'' +
                ", mAdbPort=" + mAdbPort +
                ", mAgentPort=" + mAgentPort +
                '}';
    }
}