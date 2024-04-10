/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.dialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import androidx.annotation.NonNull;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ViewUtil;

/**
 * 加载框
 *
 * @author zhh
 * @since 2019-09-24
 */
public final class LoadingDialog extends Dialog {
    private TextView mTextView;
    private String mMessage;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public LoadingDialog(@NonNull Context context) {
        super(context, R.style.dialog);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_loading,
            new FrameLayout(getContext()));
        setContentView(contentView);
        // 点击外面不消失
        setCanceledOnTouchOutside(false);
        // 可以返回键取消
        setCancelable(true);

        mTextView = CastUtil.toTextView(contentView.findViewById(R.id.tv_message));

        setMessage(mMessage);
    }

    /**
     * 设置弹出提示文字
     *
     * @param message 文字
     */
    public void setMessage(String message) {
        this.mMessage = message;

        if (mTextView != null) {
            if (TextUtils.isEmpty(mMessage)) {
                ViewUtil.setVisibility(mTextView, View.GONE);
            } else {
                ViewUtil.setVisibility(mTextView, View.VISIBLE);
                ViewUtil.setText(mTextView, mMessage);
            }
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        // 设置弹窗位置居中
        Window window = getWindow();
        if (window != null) {
            WindowManager.LayoutParams params = window.getAttributes();
            params.gravity = Gravity.CENTER;
            window.setAttributes(params);
        }
    }
}
