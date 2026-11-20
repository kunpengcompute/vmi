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

import androidx.annotation.StringRes;
import android.text.TextUtils;
import android.widget.Toast;

import com.huawei.cloudphone.MyApplication;

/**
 * 弹出toast的工具类，避免连续点击会一直弹得问题
 *
 * @author zhh
 * @since 2019-09-24
 */
public class ToastUtil {
    // Toast
    private static Toast sToast;

    /**
     * 弹出提示
     *
     * @param messageRes int
     */
    public static void showToast(@StringRes int messageRes) {
        showToast(MyApplication.instance.getString(messageRes));
    }

    /**
     * 弹出提示
     *
     * @param message String
     */
    public static void showToast(String message) {
        if (TextUtils.isEmpty(message)) {
            return;
        }
        if (sToast != null) {
            sToast.cancel();
        }
        sToast = Toast.makeText(MyApplication.instance, message, Toast.LENGTH_SHORT);
        sToast.show();
    }
}
