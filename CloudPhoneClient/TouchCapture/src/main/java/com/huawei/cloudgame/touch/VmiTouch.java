/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudgame.touch;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

/**
 * VmiTouch.
 *
 * @since 2018-07-05
 */
public class VmiTouch implements View.OnTouchListener {
    /**
     * return value when succeed.
     */
    public static final int VMI_SUCCESS = 0;

    /**
     * Failed to register vmi touch register.
     */
    public static final int VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL = 0x0A010001;

    private static final float ABSTRACT_DISPLAY_WIDTH = 720.0f;
    private static final float ABSTRACT_DISPLAY_HEIGHT = 1280.0f;
    private static final float PRESSURE_SCALE = 0.0125f;

    private static final String TAG = "VmiTouch";
    private static VmiTouch instance = new VmiTouch();

    private TOUCHSENDHOOK touchSendHook;
    private int orientation = 0;
    private TouchEvent touchEvent;

    private VmiTouch() {
        this.touchEvent = new TouchEvent();
    }

    /**
     * Get instance of VmiTouch.
     *
     * @param Null
     * @return VmiTouch instance
     */
    public static VmiTouch getInstance() {
        return instance;
    }

    private void updateTouchEvent(View view) {
        Context context = view.getContext();
        if (context instanceof Activity) {
            Activity activity = (Activity) context;    
            int width = view.getWidth();
            int height = view.getHeight();

            touchEvent.setOrientation(activity.getRequestedOrientation());
            touchEvent.setWidth(width);
            touchEvent.setHeight(height);
        }
    }

    private void sendEvent(MotionEvent event) {
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_POINTER_DOWN:
                simpleSendTouchEvent(event);
                break;
            case MotionEvent.ACTION_MOVE:
                sendMoveTouchEvent(event);
                break;
            default:
                Log.e(TAG, "OnTouch: unsupported event " + event.getActionMasked());
        }
    }

    private float getPressure(float eventPressure) {
        return eventPressure / PRESSURE_SCALE;
    }

    private void simpleSendTouchEvent(MotionEvent event) {
        int index = event.getActionIndex();
        touchEvent.setAction(event.getActionMasked());
        touchEvent.setId(event.getPointerId(index));
        touchEvent.setCloudX((int) event.getX(index));
        touchEvent.setCloudY((int) event.getY(index));
        touchEvent.setPressure((int) getPressure(event.getPressure(index)));
        sendTouchEvent(touchEvent);
    }

    private void sendMoveTouchEvent(MotionEvent event) {
        final int pointerCount = event.getPointerCount();
        for (int p = 0; p < pointerCount; p++) {
            sendPointerTouchEvent(event, p);
        }
    }

    private void sendPointerTouchEvent(MotionEvent event, int index) {
        touchEvent.setId(event.getPointerId(index));
        touchEvent.setAction(MotionEvent.ACTION_MOVE);
        touchEvent.setCloudX((int) event.getX(index));
        touchEvent.setCloudY((int) event.getY(index));
        touchEvent.setPressure((int) getPressure(event.getPressure(index)));
        sendTouchEvent(touchEvent);
    }

    private void sendTouchEvent(TouchEvent touchEvent) {
        byte[] data = touchEvent.toByteArray();
        if (touchSendHook == null) {
            Log.e(TAG, "sendHook is null, please first to regist");
            return;
        }
        touchSendHook.touchSendData(data, data.length);
    }

    /**
     * Get implementation of hook interface.
     *
     * @param hook TOUCHSENDHOOK
     * @return int
     */
    public int registerTouchSendHook(TOUCHSENDHOOK hook) {
        Log.i(TAG, "Security Audit: registerSendFunc");
        if (hook == null) {
            Log.e(TAG, "input sendHook is null, register failed");
            return VMI_TOUCH_CLIENT_SENDHOOK_REGISTER_FAIL;
        }

        touchSendHook = hook;
        return VMI_SUCCESS;
    }

    /**
     * It's called when a touch event is dispatched to a view. This allows listeners
     * to have a chance to respond before the target view.
     *
     * @param view     The view the touch event has been dispatched to.
     * @param event The MotionEvent object containing full information about the event.
     * @return True if the listener has consumed the event, false otherwise.
     */
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        if (view == null || event == null) {
            Log.e(TAG, "event called failed");
            return false;
        }
        // first action of one complete touch must be ACTION_DOWN
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            updateTouchEvent(view);
        }

        sendEvent(event);
        return true;
    }
}
