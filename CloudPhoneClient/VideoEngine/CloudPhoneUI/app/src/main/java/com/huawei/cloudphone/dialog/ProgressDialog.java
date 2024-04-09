/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;

import java.text.NumberFormat;

/**
 * Class instruction
 *
 * @author zWX809008
 * @since 2020/5/12
 */
public class ProgressDialog {
    private Context mContext;
    private AlertDialog mProgressDialog;
    private ProgressBar mProgressBar;
    private TextView mProgressTextView;
    private NumberFormat mNumberFormat;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public ProgressDialog(Context context) {
        mContext = context;
    }

    /**
     * 显示弹窗
     */
    public void show() {
        if (mProgressDialog == null) {
            mProgressDialog = new AlertDialog.Builder(mContext)
                .setCancelable(false)
                .setTitle("下载中...")
                .setView(initContentView())
                .create();
        }
        mProgressDialog.show();
    }

    /**
     * 隐藏弹窗
     */
    public void dismiss() {
        if (mProgressDialog != null) {
            mProgressDialog.dismiss();
        }
    }

    /**
     * 设置进度
     *
     * @param progress 0-1f
     */
    public void setProgress(float progress) {
        mProgressBar.setProgress((int) (progress * 100));
        // 设置文字显示，保留1位小数
        if (mNumberFormat == null) {
            mNumberFormat = NumberFormat.getPercentInstance();
            mNumberFormat.setMaximumFractionDigits(1); // 保存结果到小数点后1位
        }
        mProgressTextView.setText(mNumberFormat.format(progress));
    }

    private View initContentView() {
        View contentView = LayoutInflater.from(mContext).inflate(R.layout.dialog_progress,
            new FrameLayout(mContext), false);
        mProgressBar = CastUtil.toProgressBar(contentView.findViewById(R.id.pb_progress));
        mProgressTextView = CastUtil.toTextView(contentView.findViewById(R.id.tv_progress));
        return contentView;
    }
}
