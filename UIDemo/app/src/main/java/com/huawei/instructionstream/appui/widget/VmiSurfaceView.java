/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceView;

import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.engine.InstructionEngine;

/**
 * VmiSurfaceView.
 *
 * @since 2018-07-05
 */
public class VmiSurfaceView extends SurfaceView {
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

    /**
     * initialize.
     *
     * @param width width parameters.
     * @param height height parameters.
     */
    public void initialize(int width, int height) {
        guestWidth = width;
        guestHeight = height;
    }

    /**
     * setScreenRotation.
     *
     * @param rot Rotation.
     */
    public void setScreenRotation(InstructionEngine.Rotation rot) {
        setVmiRotation(rot.getValue());
    }

    /**
     * VmiSurfaceView.
     *
     * @param context context.
     * @param attrs VmiSurfaceView.
     */
    public VmiSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
    }

    /**
     * getGuestWidth.
     *
     * @return guestWidth.
     */
    private int getGuestWidth() {
        return this.guestWidth;
    }

    private void setGuestWidth(int guestWidth) {
        this.guestWidth = guestWidth;
    }

    private int getGuestHeight() {
        return this.guestHeight;
    }

    private void setGuestHeight(int guestHeight) {
        this.guestHeight = guestHeight;
    }

    /**
     * set vmi rotation
     *
     * @param rotation rotation
     */
    private void setVmiRotation(int rotation) {
        LogUtils.info("refactor", " recv_rotation: " + rotation);
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
}
