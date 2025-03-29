package com.huawei.cloudphone.dialog;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.SPUtil;

public class FpsTestSettingDialog extends AlertDialog{

    public static final String DEFAULT_TEST_TIME_MINUTE = "10";
    public TextView positionTextView;
    public TextView negativeTextView;
    public EditText inputEd;
    private AlertDialog.Builder builder;

    public FpsTestSettingDialog(@NonNull Context context) {
        super(context);
        View inflateView = LayoutInflater.from(getContext()).inflate(R.layout.count_down_time_view,
            new FrameLayout(getContext()));
        inputEd = inflateView.findViewById(R.id.inputEditText);
        positionTextView = inflateView.findViewById(R.id.tv_positive);
        negativeTextView = inflateView.findViewById(R.id.tv_negative);
        inputEd.setText(SPUtil.getString(SPUtil.TIME_MINUTE, DEFAULT_TEST_TIME_MINUTE));
        setView(inflateView);
    }

    public FpsTestSettingDialog setPositiveButton(View.OnClickListener positiveButton) {
        this.positionTextView.setOnClickListener(positiveButton);
        return this;
    }

    /**
     * set "Cancel" button listen
     *
     * @param negativeButton View.OnClickListener
     * @return ExitDialog
     */
    public FpsTestSettingDialog setNegativeButton(View.OnClickListener negativeButton) {
        this.negativeTextView.setOnClickListener(negativeButton);
        return this;
    }
}
