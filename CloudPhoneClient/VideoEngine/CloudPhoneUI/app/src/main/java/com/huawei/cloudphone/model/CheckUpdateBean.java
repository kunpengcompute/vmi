/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.model;

/**
 * 检查客户端更新返回值
 *
 * @author cme
 * @since 2020-05-11
 */
public class CheckUpdateBean extends Response {
    // 版本号 2
    private int versionNo;

    // 版本名 1.2.0
    private String versionName;

    // 更新信息
    private String updateMsg;

    // 是否强制更新
    private int forceUpdate;

    // apk下载url
    private String url;

    public int getVersionNo() {
        return versionNo;
    }

    public void setVersionNo(int versionNo) {
        this.versionNo = versionNo;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getVersionName() {
        return versionName;
    }

    public void setVersionName(String versionName) {
        this.versionName = versionName;
    }

    public String getUpdateMsg() {
        return updateMsg;
    }

    public void setUpdateMsg(String updateMsg) {
        this.updateMsg = updateMsg;
    }

    public int getForceUpdate() {
        return forceUpdate;
    }

    public void setForceUpdate(int forceUpdate) {
        this.forceUpdate = forceUpdate;
    }

    @Override
    public String toString() {
        return "CheckUpdateBean{" +
            "versionNo='" + versionNo + '\'' +
            ", versionName='" + versionName + '\'' +
            ", updateMsg='" + updateMsg + '\'' +
            ", url='" + url + '\'' +
            '}';
    }
}
