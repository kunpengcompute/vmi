package com.huawei.cloudphone.dialog;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.SPUtil;

public class FrameSizeParamsDialog extends AlertDialog {
    private final View contentView;
    private TextView cancelTextView;
    private TextView ensureTextView;
    private EditText frameSizeWidthEditText;
    private EditText frameSizeHeightEditText;
    private EditText frameSizeWidthAlignedEditText;
    private EditText frameSizeHeightAlignedEditText;

    public FrameSizeParamsDialog(@NonNull Context context) {
        super(context);
        contentView = LayoutInflater.from(context)
            .inflate(R.layout.dialog_frame_size, new FrameLayout(context));
        initView(contentView);
        setView(contentView);
    }

    @Override
    protected void onStart() {
        super.onStart();
        initView(contentView);
    }

    private void initView(View contentView) {
        frameSizeWidthEditText = contentView.findViewById(R.id.et_frame_size_width);
        frameSizeHeightEditText = contentView.findViewById(R.id.et_frame_size_height);
        frameSizeWidthAlignedEditText = contentView.findViewById(R.id.et_frame_size_width_aligned);
        frameSizeHeightAlignedEditText = contentView.findViewById(R.id.et_frame_size_height_aligned);
        // 按钮
        cancelTextView = contentView.findViewById(R.id.tv_cancel);
        ensureTextView = contentView.findViewById(R.id.tv_ok);

        int width = SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH, 720);
        int height = SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT, 1280);
        int widthAligned = SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_WIDTH_ALIGNED, 720);
        int heightAligned = SPUtil.getInt(SPUtil.VIDEO_FRAME_SIZE_HEIGHT_ALIGNED, 1280);

        frameSizeWidthEditText.setText(String.valueOf(width));
        frameSizeHeightEditText.setText(String.valueOf(height));
        frameSizeWidthAlignedEditText.setText(String.valueOf(widthAligned));
        frameSizeHeightAlignedEditText.setText(String.valueOf(heightAligned));
    }

    public void setPositiveButton(View.OnClickListener positiveButton) {
        ensureTextView.setOnClickListener(positiveButton);
    }

    public void setNegativeButton(View.OnClickListener negativeButton) {
        cancelTextView.setOnClickListener(negativeButton);
    }

    public String getFrameSizeWidth() {
        return frameSizeWidthEditText.getText().toString();
    }

    public void setFrameSizeWidth(String width) {
        frameSizeWidthEditText.setText(width);
    }

    public String getFrameSizeHeight() {
        return frameSizeHeightEditText.getText().toString();
    }

    public void setFrameSizeHeight(String width) {
        frameSizeHeightEditText.setText(width);
    }

    public String getFrameSizeWidthAligned() {
        return frameSizeWidthAlignedEditText.getText().toString();
    }

    public void setFrameSizeWidthAligned(String width) {
        frameSizeWidthAlignedEditText.setText(width);
    }

    public String getFrameSizeHeightAligned() {
        return frameSizeHeightAlignedEditText.getText().toString();
    }

    public void setFrameSizeHeightAligned(String width) {
        frameSizeHeightAlignedEditText.setText(width);
    }
}
