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

    // 引擎渲染第一帧画面成功
    public static final int VMI_ENGINE_EVENT_READY = -5;

    public static final int VMI_VIDEO_ENGINE_EVENT_ORIENTATION_CHANGED = -6;

    public static final int VMI_ENGINE_EVENT_GET_VERSION_TIMEOUT = -12;

    public static final int VMI_ENGINE_EVENT_VERSION_MISMATCH = -13;

    public static final int VMI_ENGINE_EVENT_ENGINE_MISMATCH = -14;

    public static final int VMI_360P_WIDTH = 360;
    public static final int VMI_360P_HEIGHT = 640;
    public static final int VMI_540P_WIDTH=540;
    public static final int VMI_540P_HEIGHT=960;
    public static final int VMI_720P_WIDTH = 720;
    public static final int VMI_720P_HEIGHT = 1280;
    public static final int VMI_1080P_WIDTH = 1080;
    public static final int VMI_1080P_HEIGHT = 1980;

    public static final boolean VMI_ADAPTIVE_RESOLUTION = true;
    public static final int VIDEO_RESOLUTION_INDEX = 1;
}
