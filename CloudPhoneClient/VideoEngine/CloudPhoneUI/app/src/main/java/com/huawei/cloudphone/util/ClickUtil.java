/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
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
