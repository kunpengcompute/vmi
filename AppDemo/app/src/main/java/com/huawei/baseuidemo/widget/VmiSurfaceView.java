package com.huawei.baseuidemo.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;

import com.huawei.baseuidemo.engine.InstructionEngine;

public class VmiSurfaceView extends SurfaceView {
    private static final String TAG = "VMISURFACEVIEW";
    private Context context = null;
    private int guestWidth = 1080;
    private int guestHeight = 1920;

    public VmiSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
    }

    public void initialize(int width, int height) {
        guestWidth = width;
        guestHeight = height;
    }

    public void setScreenRotation(InstructionEngine.Rotation rot) {
        setVmiRotation(rot.getValue());
    }

    private void setVmiRotation(int rotation) {
        Log.i(TAG, "rotation is :" + rotation);
        switch (rotation) {
            case Surface.ROTATION_0:
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            case Surface.ROTATION_90:
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            case Surface.ROTATION_180:
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            case Surface.ROTATION_270:
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            default:
                break;
        }
    }
}
