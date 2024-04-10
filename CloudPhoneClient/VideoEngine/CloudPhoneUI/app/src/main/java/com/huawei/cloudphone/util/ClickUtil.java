/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.util;

/**
 * 工具类
 *
 * @author zhh
 * @since 2020-04-30
 */
public class ClickUtil {
    /**
     * 两次点击间隔不能少于1000ms
     */
    private static final int FAST_CLICK_DELAY_TIME = 1000;
    private static long lastClickTime;

    /**
     * 判断是否是快速点击
     *
     * @return true:是快速点击 false:不是
     */
    public static boolean isFastClick() {
        long currentClickTime = System.currentTimeMillis();
        if ((currentClickTime - lastClickTime) >= FAST_CLICK_DELAY_TIME) {
            return false;
        }
        lastClickTime = currentClickTime;
        return true;
    }
}
