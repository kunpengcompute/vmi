/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol.floatingview;

import android.app.Activity;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.v4.view.ViewCompat;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import com.huawei.instructionstream.appui.R;

/**
 * 悬浮窗管理器.
 *
 * @since 2018-07-05
 */
public class FloatingViewManager implements IFloatingViewManager {
    private static volatile FloatingViewManager floatingViewManager;
    private DragView menfloatingview;
    private FrameLayout container;

    private FloatingViewManager() {
    }

    /**
     * return an instance.
     *
     * @return an instance
     */
    public static FloatingViewManager getInstance() {
        if (floatingViewManager == null) {
            synchronized (FloatingViewManager.class) {
                if (floatingViewManager == null) {
                    floatingViewManager = new FloatingViewManager();
                }
            }
        }
        return floatingViewManager;
    }

    @Override
    public void remove() {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                if (container == null || menfloatingview == null) {
                    return;
                }
                if (menfloatingview.getParent() == container) {
                    container.removeView(menfloatingview);
                }
                menfloatingview = null;
            }
        });
    }

    @Override
    public void add(Context context) {
        ensureMiniPlayer(context);
    }

    private void ensureMiniPlayer(Context context) {
        synchronized (this) {
            if (menfloatingview != null) {
                return;
            }
            menfloatingview = new DragView(context.getApplicationContext());
            menfloatingview.setbg(R.mipmap.vmi_greenwifi_gimp4);
            menfloatingview.setLayoutParams(getParams());
            addViewToWindow(menfloatingview);
        }
    }

    private FrameLayout.LayoutParams getParams() {
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
                RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT
        );
        params.gravity = Gravity.TOP | Gravity.START;
        params.setMargins(13, params.topMargin, params.rightMargin, 56);
        return params;
    }

    private void addViewToWindow(final DragView view) {
        if (container == null) {
            return;
        }
        container.addView(view);
    }

    @Override
    public void attach(Activity activity) {
        attach(getActivityRoot(activity));
    }

    @Override
    public void attach(FrameLayout container) {
        if (container == null || menfloatingview == null) {
            this.container = container;
            return;
        }
        if (menfloatingview.getParent() == container) {
            return;
        }
        if (this.container != null && menfloatingview.getParent() == this.container) {
            this.container.removeView(menfloatingview);
        }
        this.container = container;
        container.addView(menfloatingview);
    }

    private FrameLayout getActivityRoot(Activity activity) {
        FrameLayout frameLayout = null;
        if (activity != null) {
            try {
                Object object = activity.getWindow().getDecorView().findViewById(android.R.id.content);
                if (object instanceof FrameLayout) {
                    frameLayout = (FrameLayout) object;
                }
            } catch (NullPointerException e) {
                e.printStackTrace();
            }
        }
        return frameLayout;
    }

    @Override
    public void detach(Activity activity) {
        detach(getActivityRoot(activity));
    }

    @Override
    public void detach(FrameLayout container) {
        if (menfloatingview != null && container != null && ViewCompat.isAttachedToWindow(menfloatingview)) {
            container.removeView(menfloatingview);
        }
        if (this.container == container) {
            this.container = null;
        }
    }

    /**
     * getFloatingView.
     *
     * @return dragview.
     */
    public DragView getFloatingView() {
        return menfloatingview;
    }
}
