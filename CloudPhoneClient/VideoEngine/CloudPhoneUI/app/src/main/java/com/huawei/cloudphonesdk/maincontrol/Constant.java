/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import androidx.annotation.Keep;

/***
 * 一些静态常量
 *
 * @since 2020-08-28
 */
@Keep
public class Constant {
    /**
     * 解码方式:硬解
     */
    public static final int DECODE_METHOD_HARDWARE = 1;

    /**
     * 解码方式:软解
     */
    public static final int DECODE_METHOD_SOFTWARE = 2;

    /**
     * JniStatus:running
     */
    public static final int JNI_STATUS_RUNNING = 1;

    /**
     * JniStatus:disconnected
     */
    public static final int JNI_STATUS_DISCONNECTED = 2;

    /**
     * 接入方式:指令流
     */
    public static final int RUNNING_MODE_INSTRUCTION = 1;

    /**
     * 接入方式:视频流
     */
    public static final int RUNNING_MODE_CLOUD_PHONE = 2;

    /**
     * 接入方式:视频流观众模式
     */
    public static final int RUNNING_MODE_CLOUD_PHONE_AUDIENCE = 3;

    /**
     * 服务端有json接口
     */
    public static final int SERVER_HAS_JSON_API = 1;

    /**
     * 服务端无json接口
     */
    public static final int SERVER_NO_JSON_API = 0;

    /**
     * 服务端鉴权失败
     */
    public static final int SERVER_NO_USER = 5;

    public static final int VMI_VIDEO_ENGINE_EVENT_SOCK_DISCONN = -2;

    public static final int VMI_VIDEO_ENGINE_EVENT_ORIENTATION_CHANGED = -6;

    public static final int VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT = -12;

    public static final int VMI_ENGINE_EVENT_VERSION_MISMATCH = -13;

    public static final int VMI_ENGINE_EVENT_ENGINE_MISMATCH = -14;
}
