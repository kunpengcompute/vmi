package com.huawei.cloudphone.dialog;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;

import com.example.cloudphoneui.R;

public class ExitDialog extends AlertDialog {

    private final Button okBt;
    private final TextView exitTv;

    public ExitDialog(@NonNull Context context) {
        super(context);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_exit, new FrameLayout(getContext()));
        okBt = contentView.findViewById(R.id.okButton);
        exitTv = contentView.findViewById(R.id.exit);
        setView(contentView);
    }

    public ExitDialog setPositiveButton(View.OnClickListener onClickListener){
        okBt.setOnClickListener(onClickListener);
        return this;
    }

    public ExitDialog setNegativeButton(View.OnClickListener onClickListener){
        exitTv.setOnClickListener(onClickListener);
        return this;
    }
}
