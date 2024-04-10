/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.widget;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceView;

import com.huawei.cloudphonesdk.maincontrol.NewPacketCallback;
import com.huawei.cloudphonesdk.utils.LogUtil;

import java.lang.ref.WeakReference;

/**
 * VmiSurfaceView.
 *
 * @since 2018-07-05
 */
public class VmiSurfaceView extends SurfaceView implements NewPacketCallback {
    private static final String TAG = VmiSurfaceView.class.getSimpleName();
    private WeakReference<Activity> mActivityReference;
    private RotationHandler mRotationHandler;
    private int guestWidth = 1080;
    private int guestHeight = 1920;

    /**
     * VmiSurfaceView.
     *
     * @param context context.
     */
    public VmiSurfaceView(Context context) {
        this(context, null);
    }

    /**
     * VmiSurfaceView.
     *
     * @param context context.
     * @param attrs   VmiSurfaceView.
     */
    public VmiSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * VmiSurfaceView.
     *
     * @param context      context.
     * @param attrs        VmiSurfaceView.
     * @param defStyleAttr defStyleAttr.
     */
    public VmiSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        if (context instanceof Activity) {
            mActivityReference = new WeakReference<>((Activity) context);
            mRotationHandler = new RotationHandler(mActivityReference);
        } else {
            throw new IllegalStateException("VmiSurfaceView should be only used in activity");
        }
    }

    public void initialize(int width, int height) {
        guestWidth = width;
        guestHeight = height;
    }

    @Override
    protected void onDetachedFromWindow() {
        mRotationHandler.removeCallbacksAndMessages(null);
        super.onDetachedFromWindow();
    }

    @Override
    public void onNewPacket(byte[] data) {
        Activity activity = mActivityReference.get();
        if (activity == null || activity.isFinishing()) {
            return;
        }
        int oldOrientation = activity.getRequestedOrientation();
        int newOrientation = getOrientation(data[0]);
        LogUtil.error(TAG, "current_rotation: " + oldOrientation + " recv_rotation: " + newOrientation);
        if (oldOrientation != newOrientation) {
            mRotationHandler.sendEmptyMessage(newOrientation);
        }
    }

    /**
     * 将服务端传过来的方向与UI的方向做映射
     *
     * @param rotation surface rotation
     * @return ui rotation
     */
    private static int getOrientation(int rotation) {
        switch (rotation) {
            case Surface.ROTATION_0:
                return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
            case Surface.ROTATION_90:
                return ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
            case Surface.ROTATION_180:
                return ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
            case Surface.ROTATION_270:
                return ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
            default:
                return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
        }
    }

    public void setScreenRotation(VmiSurfaceView.Rotation rot) {
        setVmiRotation(rot.getValue());
    }

    /**
     * set vmi rotation
     *
     * @param rotation rotation
     */
    private void setVmiRotation(int rotation) {
        LogUtil.info("refactor", " recv_rotation: " + rotation);
        switch (rotation) {
            case Surface.ROTATION_0:
                // relevant to OpenGl ES
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            case Surface.ROTATION_90:
                getHolder().setFixedSize(guestHeight, guestWidth);
                break;
            case Surface.ROTATION_180:
                getHolder().setFixedSize(guestWidth, guestHeight);
                break;
            case Surface.ROTATION_270:
                getHolder().setFixedSize(guestHeight, guestWidth);
                break;
            default:
                break;
        }
    }

    public enum Rotation {
        ROTATION0(0),
        ROTATION90(1),
        ROTATION180(2),
        ROTATION270(3);
        Rotation(int value) {
            this.value = value;
        }

        /**
         * getValue.
         *
         * @return get enum Rotation value
         */
        public int getValue() {
            return this.value;
        }

        private int value;
    }

    /**
     * 处理旋转的Handler
     */
    static class RotationHandler extends Handler {
        private WeakReference<Activity> mWeakReference;

        RotationHandler(WeakReference<Activity> activityReference) {
            mWeakReference = activityReference;
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            Activity activity = mWeakReference.get();
            if (activity == null || activity.isFinishing()) {
                return;
            }
            activity.setRequestedOrientation(msg.what);
        }
    }
}
