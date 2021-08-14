/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.common;

import android.content.DialogInterface;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Size;
import android.view.Surface;
import android.view.View;
import android.view.Window;

/**
 * BaseDefaultContentDialog
 *
 * @since 2021-06-10
 */
public class BaseDefaultContentDialog extends BaseDialogFragment {
    private static final double HEIGHT_RATIO = 0.65;
    private static final int WIDTH_COMPENSATION = 50;
    private static final int DURATION = 0;
    private static final int ANGLE = 90;
    private View content;
    private int beginDialogWidth;
    private int beginDialogHeight;

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        getDialog().setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(DialogInterface dialogInterface) {
                Window window = getDialog().getWindow();
                if (window != null) {
                    setRotation(mRotation);
                }
            }
        });
    }

    /**
     * 获取屏幕大小
     *
     * @return Size
     */
    public static Size getWindowSize() {
        DisplayMetrics displayMetrics = Resources.getSystem().getDisplayMetrics();
        return new Size(displayMetrics.widthPixels, displayMetrics.heightPixels);
    }

    @Override
    public void setRotation(int rotation) {
        Window window = getDialog().getWindow();
        if (window != null) {
            content = window.findViewById(android.R.id.content);
            beginDialogWidth = content.getWidth();
            beginDialogHeight = content.getHeight();
        }
        Size windowSize = getWindowSize();
        int width;
        int height;
        int tranX;
        int tranY;
        // 横屏
        if (rotation == Surface.ROTATION_90 || rotation == Surface.ROTATION_270) {
            width = (int) (windowSize.getHeight() * HEIGHT_RATIO);
            height = beginDialogHeight;
            // 计算偏移量
            tranX = (height - width) / 2;
            tranY = (width - height) / 2;
            window.setLayout(height + WIDTH_COMPENSATION, width);
            content.getLayoutParams().width = width;
            content.getLayoutParams().height = height;
            content.setLayoutParams(content.getLayoutParams());
            content.animate()
                    // 旋转
                    .rotation(ANGLE * (rotation))
                    // 平移
                    .translationX(tranX)
                    .translationY(tranY)
                    .setDuration(DURATION);
        }
    }
}


