/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.dialog;

import android.app.AlertDialog;
import android.content.Context;
import androidx.annotation.NonNull;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;

/**
 * 退出弹窗
 *
 * @author zhh
 * @since 2020-05-21
 */
public final class ExitDialog extends AlertDialog implements RadioGroup.OnCheckedChangeListener {
    private RadioGroup mDefinitionRadioGroup;
    private TextView mNegativeTextView;
    private TextView mPositionTextView;

    /**
     * 构造方法
     *
     * @param context Context
     */
    public ExitDialog(@NonNull Context context) {
        super(context);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_exit,
            new FrameLayout(getContext()));
        mDefinitionRadioGroup = CastUtil.toRadioGroup(contentView.findViewById(R.id.rg_definition));
        mNegativeTextView = CastUtil.toTextView(contentView.findViewById(R.id.tv_negative));
        mPositionTextView = CastUtil.toTextView(contentView.findViewById(R.id.tv_positive));
        setView(contentView);
        // add change listener
        mDefinitionRadioGroup.setOnCheckedChangeListener(this);
    }

    /**
     * set "OK" button listen
     *
     * @param positiveButton View.OnClickListener
     * @return ExitDialog
     */
    public ExitDialog setPositiveButton(View.OnClickListener positiveButton) {
        mPositionTextView.setOnClickListener(positiveButton);
        return this;
    }

    /**
     * set "Cancel" button listen
     *
     * @param negativeButton View.OnClickListener
     * @return ExitDialog
     */
    public ExitDialog setNegativeButton(View.OnClickListener negativeButton) {
        mNegativeTextView.setOnClickListener(negativeButton);
        return this;
    }

    @Override
    public void onCheckedChanged(RadioGroup radioGroup, int id) {
        radioGroup.check(id);
    }
}
