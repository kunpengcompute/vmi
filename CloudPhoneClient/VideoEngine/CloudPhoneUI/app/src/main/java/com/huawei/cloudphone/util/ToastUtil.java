/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
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
