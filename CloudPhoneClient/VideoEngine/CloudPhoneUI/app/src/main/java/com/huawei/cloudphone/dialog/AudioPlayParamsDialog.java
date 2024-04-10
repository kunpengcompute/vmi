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

public class AudioPlayParamsDialog extends AlertDialog {
    private final View contentView;
    private TextView cancelTextView;
    private TextView ensureTextView;
    private EditText sampleIntervalEditText;
    private EditText bitRateEditText;

    public AudioPlayParamsDialog(@NonNull Context context) {
        super(context);
        contentView = LayoutInflater.from(context)
            .inflate(R.layout.dialog_audio_params, new FrameLayout(context));
        initView(contentView);
        setView(contentView);
    }

    private void initView(View contentView) {
        sampleIntervalEditText = contentView.findViewById(R.id.et_sample_Interval);
        bitRateEditText = contentView.findViewById(R.id.et_bitrate);
        // 按钮
        cancelTextView = contentView.findViewById(R.id.tv_cancel);
        ensureTextView = contentView.findViewById(R.id.tv_ok);

        int sampleInterval = SPUtil.getInt("sampleInterval", 10);
        int bitRate = SPUtil.getInt("audio_play_bitrate", 192000);
        sampleIntervalEditText.setText(String.valueOf(sampleInterval));
        bitRateEditText.setText(String.valueOf(bitRate));
    }

    public void setPositiveButton(View.OnClickListener positiveButton) {
        ensureTextView.setOnClickListener(positiveButton);
    }

    @Override
    protected void onStart() {
        initView(contentView);
        super.onStart();
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
    public String getSampleInterval() {
        return sampleIntervalEditText.getText().toString();
    }

    /**
     * set bitRate
     *
     * @param bitRate String
     */
    public void setSampleInterval(String bitRate) {
        sampleIntervalEditText.setText(bitRate);
    }

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
}
