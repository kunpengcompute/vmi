/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphone.helper;

import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.huawei.cloudphone.R;
import com.huawei.cloudphone.floating.FloatingManager;
import com.huawei.cloudphone.ui.activities.FullscreenActivity;
import com.huawei.cloudphone.util.CastUtil;
import com.huawei.cloudphone.util.ClickUtil;

/**
 * 悬浮按钮辅助类
 *
 * @author zhh
 * @since 2020/5/28
 */
public class FloatingHelper implements View.OnClickListener {
    private static final String TAG = FloatingHelper.class.getSimpleName();
    private FullscreenActivity mActivity;

    private FloatingManager mFloatingManager;

    /**
     * 构造函数
     *
     * @param activity FullscreenActivity
     */
    public FloatingHelper(FullscreenActivity activity) {
        mActivity = activity;
    }

    /**
     * Init
     */
    public void init() {
        // Init Drag Container
        FrameLayout container = new FrameLayout(mActivity);
        container.setWillNotDraw(false);
        FrameLayout decorView = CastUtil.toFrameLayout(mActivity.getWindow().getDecorView());
        decorView.addView(container, new FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.MATCH_PARENT));
        // Init FloatingView
        mFloatingManager = new FloatingManager()
            .setMainImage(R.drawable.image_menu_green)
            .addMenuItem(R.drawable.image_close, R.id.iv_close, this)
            .addMenuItem(R.drawable.image_frame_show, R.id.iv_frame, this)
            .addMenuItem(R.drawable.image_video_setting, R.id.iv_video, this)
            .addMenuItem(R.drawable.image_audio_setting, R.id.iv_audio, this)
            .addMenuItem(R.drawable.image_back_key, R.id.iv_back, this)
            .addMenuItem(R.drawable.image_home_key, R.id.iv_home, this)
            .addMenuItem(R.drawable.image_switch_key, R.id.iv_switch, this)
            .attach(container);
        mFloatingManager.create();
    }

    public void setMainImage(int res) {
        mFloatingManager.setColor(res);
    }


    @Override
    public void onClick(View view) {
        if (ClickUtil.isFastClick()) {
            return;
        }
        switch (view.getId()) {
            case R.id.iv_close:
                mActivity.showExitDialog();
                break;
            case R.id.iv_frame:
                View frameRateView = mActivity.getFrameRateView();
                if (frameRateView.getVisibility() != View.VISIBLE) {
                    frameRateView.setVisibility(View.VISIBLE);
                } else {
                    frameRateView.setVisibility(View.GONE);
                }
                break;
            case R.id.iv_home: {
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_HOME, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_HOME, KeyEvent.ACTION_UP);
                break;
            }
            case R.id.iv_switch: {
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_APP_SWITCH, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_APP_SWITCH, KeyEvent.ACTION_UP);
                break;
            }
            case R.id.iv_back: {
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_BACK, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_BACK, KeyEvent.ACTION_UP);
                break;
            }
            case R.id.iv_video: {
                mActivity.showEncodeInputDialog();
                break;
            }
            case R.id.iv_audio: {
               mActivity.showAudioPlayInputDialog();
                break;
            }
            default: {
                break;
            }
        }
        mFloatingManager.setOpen(!mFloatingManager.isOpen());
    }
}
