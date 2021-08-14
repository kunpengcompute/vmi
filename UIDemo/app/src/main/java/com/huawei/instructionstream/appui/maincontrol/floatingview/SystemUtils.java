/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol.floatingview;

import android.content.Context;

/**
 * SystemUtils.
 *
 * @since 2018-07-05
 */
public class SystemUtils {
    /**
     * get status bar's height.
     *
     * @param context activity context
     * @return height of status bar
     */
    public static int getStatusBarHeight(Context context) {
        int result = 0;
        int resourceId = context.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = context.getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    /**
     * get screen'w width.
     *
     * @param context activity context
     * @return screen's width
     */
    public static int getScreenWidth(Context context) {
        int screenWith = -1;
        try {
            screenWith = context.getResources().getDisplayMetrics().widthPixels;
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        return screenWith;
    }

    /**
     * get screen's height.
     *
     * @param context activity context
     * @return height of screen
     */
    public static int getScreenHeight(Context context) {
        int screenHeight = -1;
        try {
            screenHeight = context.getResources().getDisplayMetrics().heightPixels;
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        return screenHeight;
    }

}
