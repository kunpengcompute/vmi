package com.huawei.cloudphone.helper;

import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.example.cloudphoneui.R;
import com.huawei.cloudphone.FullscreenActivity;
import com.huawei.utils.Utils;
import com.huawei.cloudphone.view.floatingView.FloatingManger;

public class FloatingHelper implements View.OnClickListener {

    private FullscreenActivity mActivity;
    private FloatingManger floatingManger;

    public FloatingHelper(FullscreenActivity mActivity) {
        this.mActivity = mActivity;
    }

    public void init() {
        FrameLayout container = new FrameLayout(mActivity);
        container.setWillNotDraw(false);
        FrameLayout decorView = (FrameLayout) mActivity.getWindow().getDecorView();
        decorView.addView(container, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        floatingManger = new FloatingManger().setMainImage(R.drawable.image_menu_green)
                .addMenuItem(R.drawable.image_close, R.id.iv_close, this)
                .addMenuItem(R.drawable.image_frame_show, R.id.iv_frame, this)
                .addMenuItem(R.drawable.image_back_key, R.id.iv_back, this)
                .addMenuItem(R.drawable.image_home_key, R.id.iv_home, this)
                .addMenuItem(R.drawable.image_switch_key, R.id.iv_switch, this).attach(container);
        floatingManger.create();
    }

    public void setMainImage(int res) {
        floatingManger.setColor(res);
    }

    @Override
    public void onClick(View view) {
        if (Utils.isFastClick()) {
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
            case R.id.iv_home:
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_HOME, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_HOME, KeyEvent.ACTION_UP);
                break;
            case R.id.iv_switch:
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_APP_SWITCH, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_APP_SWITCH, KeyEvent.ACTION_UP);
                break;
            case R.id.iv_back:
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_BACK, KeyEvent.ACTION_DOWN);
                mActivity.sendKeyEvent(KeyEvent.KEYCODE_BACK, KeyEvent.ACTION_UP);
                break;
            default:
                break;
        }
        floatingManger.setOpen(!floatingManger.isOpen());
    }

    public void closeFloatButton() {
        floatingManger.setOpen(false);
    }
}
