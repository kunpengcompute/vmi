/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

package com.huawei.cloudphone.util;

import android.text.TextUtils;
import android.view.View;
import android.widget.TextView;

/**
 * 设置View属性的工具类
 *
 * @author zhh
 * @since 2019-09-24
 */
public class ViewUtil {
    /**
     * 设置View是否可见
     *
     * @param view       View
     * @param visibility {@link View#VISIBLE,View#INVISIBLE,View#GONE}
     */
    public static void setVisibility(View view, int visibility) {
        if (visibility != view.getVisibility()) {
            view.setVisibility(visibility);
        }
    }

    /**
     * 设置TextView文本
     *
     * @param textView TextView
     * @param text     string text
     */
    public static void setText(TextView textView, String text) {
        if (!TextUtils.equals(text, textView.getText())) {
            textView.setText(text);
        }
    }
}
