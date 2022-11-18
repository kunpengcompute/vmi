/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.cloudphone;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;

import androidx.annotation.NonNull;


import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;

import java.lang.ref.WeakReference;


/**
 * VmiSurfaceView.
 *
 * @since 2018-07-05
 */
public class VmiSurfaceView extends SurfaceView implements NewPacketCallback {
    private static final String TAG = "VmiSurfaceView";
    private static final boolean DEBUG = false;


    /**
     * input X scale.
     */
    public float inputXScale = 1.0f;

    /**
     * input Y scale.
     */
    public float inputYScale = 1.0f;
    private int guestWidth = 1080;
    private int guestHeight = 1920;
    /**
     * display width.
     */
    private int displayWidth = 1080;

    /**
     * display height.
     */
    private int displayHeight = 1920;

    /**
     * vmi display width.
     */
    private int vmWidth = 720;

    /**
     * vmi display height.
     */
    private int vmHeight = 1280;

    private Context context = null;
    private WeakReference<Activity> activityWeakReference;
    private RotationHandler rotationHandler;

    /**
     * VmiSurfaceView.
     *
     * @param context context.
     * @param attrs   VmiSurfaceView.
     */
    public VmiSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public VmiSurfaceView(Context context) {
        this(context, null);
    }

    public VmiSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        if (context instanceof Activity) {
            activityWeakReference = new WeakReference<Activity>((Activity) context);
            rotationHandler = new RotationHandler(activityWeakReference);
        } else {
            throw new IllegalStateException("VmiSurfaceView should be onlyused in activity");
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        rotationHandler.removeCallbacksAndMessages(null);
        super.onDetachedFromWindow();
    }

    @Override
    public void onNewPacket(byte[] data) {
        Activity activity = activityWeakReference.get();
        if (activity == null || activity.isFinishing()) {
            return;
        }
        int oldOrientation = activity.getRequestedOrientation();
        int newOrientation = getOrientation(data[0]);
        Log.d(TAG, "onNewPacket:newOrientation: " + newOrientation);
        if (oldOrientation != newOrientation) {
            rotationHandler.sendEmptyMessage(newOrientation);
        }
    }

    private int getOrientation(byte rotation) {
        switch (rotation) {
            case Surface.ROTATION_0:
                return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
            case Surface.ROTATION_90:
                return ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
            case Surface.ROTATION_180:
                return ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
            case Surface.ROTATION_270:
                return ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
            default:
                return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
        }
    }

    static class RotationHandler extends Handler {
        private WeakReference<Activity> weakReference;

        RotationHandler(WeakReference<Activity> activityWeakReference) {
            weakReference = activityWeakReference;
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Activity activity = weakReference.get();
            if (activity == null || activity.isFinishing()) {
                return;
            }
            Log.d(TAG, "handleMessage:setRequestedOrientation ");
            activity.setRequestedOrientation(msg.what);
        }
    }
}
