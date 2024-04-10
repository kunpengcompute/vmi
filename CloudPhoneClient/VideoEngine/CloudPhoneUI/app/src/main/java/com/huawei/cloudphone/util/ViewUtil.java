/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
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
