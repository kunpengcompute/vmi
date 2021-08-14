/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.common;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.Dialog;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import com.huawei.instructionstream.appui.FullscreenActivity;
import com.huawei.instructionstream.appui.R;

/**
 * RotateDialog.
 *
 * @since 2021-06-10
 */
@SuppressLint("ValidFragment")
public class RotateDialog extends BaseDefaultContentDialog {

    FullscreenActivity fullscreenActivity;
    View dialogView;

    @SuppressLint("ValidFragment")
    public RotateDialog(FullscreenActivity fullscreenActivity, View view) {
        this.fullscreenActivity = fullscreenActivity;
        this.dialogView = view;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        Button okButton = dialogView.findViewById(R.id.rotate_ok);
        Button exitButton = dialogView.findViewById(R.id.rotate_exit);
        CheckBox frameRateButton = dialogView.findViewById(R.id.rotate_frameRate);
        builder.setView(dialogView);
        // 点击帧率显示按钮
        frameRateButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                fullscreenActivity.setShowState(isChecked);
                buttonView.setChecked(fullscreenActivity.isShowState());
            }
        });
        // 点击退出云手机
        exitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fullscreenActivity.stopCloudPhone();
            }
        });
        // 点击确定
        okButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (fullscreenActivity.isShowState()) {
                    fullscreenActivity.findViewById(R.id.textRate).setVisibility(View.VISIBLE);
                } else {
                    fullscreenActivity.findViewById(R.id.textRate).setVisibility(View.INVISIBLE);
                }
                fullscreenActivity.dismissRotateDialog();
            }
        });
        return builder.setCancelable(false)
                .create();
    }
}
