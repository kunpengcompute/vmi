/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.net;

/**
 * 接口信息
 *
 * @author zhh
 * @since 2019-09-24
 */
public class RequestParam {
    /**
     * 登录、登出、注册、修改密码
     */
    public static final String API_ACCOUNT = "/armnative/api/v1.0/account";

    /**
     * 获取设备列表、删除设备、申请设备
     */
    public static final String API_DEVICE = "/armnative/api/v1.0/sourcepool/device";

    /**
     * 获取区域
     */
    public static final String API_SOURCE = "/armnative/api/v1.0/sourcepool/0";

    /**
     * 获取模板
     */
    public static final String API_TEMPLATE = "/armnative/api/v1.0/templates";

    /**
     * 重启设备
     */
    public static final String DEVICE_API_REBOOT = "/armnative/api/v1.0/device/reboot";

    /**
     * 检查配服最新客户端
     */
    public static final String CHECK_APK_UPDATE = "/armnative/queryAPK.do";
}
