/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.utils;

/**
 * 工具类
 *
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
     * @return true:两次点击事件超过一秒
     *         false:两次点击事件小于一秒
     */
    public static boolean isFastClick() {
        boolean flag = false;
        long curClickTime = System.currentTimeMillis();
        if ((curClickTime - lastClickTime) >= FAST_CLICK_DELAY_TIME) {
            flag = true;
        }
        lastClickTime = curClickTime;
        return flag;
    }
}