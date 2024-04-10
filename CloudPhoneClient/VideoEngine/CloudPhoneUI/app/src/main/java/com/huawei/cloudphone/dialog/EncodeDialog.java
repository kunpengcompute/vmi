/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.dialog;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;

import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.SPUtil;

/**
 * Encode alert dialog.
 *
 * @author gj
 * @since 2019-12-28
 */
public class EncodeDialog extends AlertDialog {
    private final View contentView;
    // 服务端编码四个参数

    private EditText bitRateEditText;
    private EditText profileEditText;
    private EditText gopSizeEditText;
    private EditText rcModeEditText;
    private EditText keyFrameEditText;

    private EditText interpolationEditText;

    private TextView cancelTextView;
    private TextView ensureTextView;

    public EncodeDialog(@NonNull Context context) {
        super(context);
        contentView = LayoutInflater.from(context)
            .inflate(R.layout.dialog_encode_modify, new FrameLayout(context));
        initView(contentView);
        setView(contentView);
    }

    @Override
    protected void onStart() {
        super.onStart();
        initView(contentView);
    }

    private void initView(View contentView) {
        bitRateEditText = contentView.findViewById(R.id.et_bitrate);
        profileEditText = contentView.findViewById(R.id.et_profile);
        gopSizeEditText = contentView.findViewById(R.id.et_gopsize);
        rcModeEditText = contentView.findViewById(R.id.et_rcmode);
        keyFrameEditText = contentView.findViewById(R.id.et_force_key_frame);
        interpolationEditText = contentView.findViewById(R.id.et_interpolation);
        // 按钮
        cancelTextView = contentView.findViewById(R.id.tv_cancel);
        ensureTextView = contentView.findViewById(R.id.tv_ok);

        int bitRate = SPUtil.getInt(SPUtil.VIDEO_BIT_RATE, 3000000);
        int profile = SPUtil.getInt(SPUtil.VIDEO_PROFILE, 1);
        int gopSize = SPUtil.getInt(SPUtil.VIDEO_GOP_SIZE, 30);
        int rcMode = SPUtil.getInt(SPUtil.VIDEO_RC_MODE, 2);
        int keyFrame = SPUtil.getInt(SPUtil.VIDEO_FORCE_KET_FRAME, 0);
        int interpolation = SPUtil.getInt(SPUtil.VIDEO_INTERPOLATION, 0);

        bitRateEditText.setText(String.valueOf(bitRate));
        profileEditText.setText(String.valueOf(profile));
        gopSizeEditText.setText(String.valueOf(gopSize));
        rcModeEditText.setText(String.valueOf(rcMode));
        keyFrameEditText.setText(String.valueOf(keyFrame));
        interpolationEditText.setText(String.valueOf(interpolation));
    }

    @Override
    public void show() {
        super.show();
        if (bitRateEditText.hasFocus()) {
            bitRateEditText.setSelection(bitRateEditText.getText().toString().length());
        } else if (profileEditText.hasFocus()) {
            profileEditText.setSelection(profileEditText.getText().toString().length());
        } else if (gopSizeEditText.hasFocus()) {
            gopSizeEditText.setSelection(gopSizeEditText.getText().toString().length());
        } else if (rcModeEditText.hasFocus()) {
            rcModeEditText.setSelection(rcModeEditText.getText().toString().length());
        } else if (keyFrameEditText.hasFocus()) {
            keyFrameEditText.setSelection(keyFrameEditText.getText().toString().length());
        } else if (interpolationEditText.hasFocus()) {
            interpolationEditText.setSelection(interpolationEditText.getText().toString().length());
        }
    }

    /**
     * set "OK" button listen
     *
     * @param positiveButton View.OnClickListener
     */
    public void setPositiveButton(View.OnClickListener positiveButton) {
        ensureTextView.setOnClickListener(positiveButton);
    }

    /**
     * set "Cancel" button listen
     *
     * @param negativeButton View.OnClickListener
     */
    public void setNegativeButton(View.OnClickListener negativeButton) {
        cancelTextView.setOnClickListener(negativeButton);
    }

    /**
     * get bitrate
     *
     * @return String
     */
    public String getBitRate() {
        return bitRateEditText.getText().toString();
    }

    /**
     * set bitRate
     *
     * @param bitRate String
     */
    public void setBitRate(String bitRate) {
        bitRateEditText.setText(bitRate);
    }

    /**
     * get profile
     *
     * @return String
     */
    public String getProfile() {
        return profileEditText.getText().toString();
    }

    /**
     * set profile
     *
     * @param profile String
     */
    public void setProfile(String profile) {
        profileEditText.setText(profile);
    }

    /**
     * get gopSize
     *
     * @return String
     */
    public String getGopSize() {
        return gopSizeEditText.getText().toString();
    }

    /**
     * set gopSize
     *
     * @param gopSize String
     */
    public void setGopSize(String gopSize) {
        gopSizeEditText.setText(gopSize);
    }

    public String getRcModeEditText() {
        return rcModeEditText.getText().toString();
    }

    public void setRcModeEditText(String str) {
        rcModeEditText.setText(str);
    }

    public String getKeyFrameEditText() {
        return keyFrameEditText.getText().toString();
    }

    public void setKeyFrameEditText(String str) {
        this.keyFrameEditText = keyFrameEditText;
    }

    public String getInterpolationEditText() {
        return interpolationEditText.getText().toString();
    }

    public void setInterpolationEditText(String str) {
        this.interpolationEditText = interpolationEditText;
    }
}
