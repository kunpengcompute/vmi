/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2024. All rights reserved.
 */

package com.huawei.cloudphone.dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;

public final class FpsTestReportDialog extends AlertDialog{
    private final Button positionTextView;
    private final Button negativeTextView;
    public final TextView startTime;
    public final TextView stopTime;
    public TextView averageFpsTextView;
    public TextView averageJankTextView;
    public TextView averageBJankTextView;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public FpsTestReportDialog(@NonNull Context context) {
        super(context);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_fps_test,
            new FrameLayout(getContext()));
        negativeTextView = contentView.findViewById(R.id.tv_negative);
        positionTextView = contentView.findViewById(R.id.tv_positive);
        averageJankTextView = CastUtil.toTextView(contentView.findViewById(R.id.average_jank));
        averageBJankTextView = CastUtil.toTextView(contentView.findViewById(R.id.average_bjank));
        averageFpsTextView = CastUtil.toTextView(contentView.findViewById(R.id.average_fps));
        startTime = contentView.findViewById(R.id.test_start_time);
        stopTime = contentView.findViewById(R.id.test_stop_time);
        setView(contentView);
    }

    /**
     * set "OK" button listen
     *
     * @param positiveButton View.OnClickListener
     * @return ExitDialog
     */
    public FpsTestReportDialog setPositiveButton(View.OnClickListener positiveButton) {
        positionTextView.setOnClickListener(positiveButton);
        return this;
    }

    /**
     * set "Cancel" button listen
     *
     * @param negativeButton View.OnClickListener
     * @return ExitDialog
     */
    public FpsTestReportDialog setNegativeButton(View.OnClickListener negativeButton) {
        negativeTextView.setOnClickListener(negativeButton);
        return this;
    }
}
