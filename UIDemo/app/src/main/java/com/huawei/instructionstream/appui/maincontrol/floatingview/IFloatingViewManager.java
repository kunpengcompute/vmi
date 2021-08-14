/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol.floatingview;

import android.app.Activity;
import android.content.Context;
import android.widget.FrameLayout;

/**
 * IFloatingViewManager interface.
 *
 * @since 2018-07-05
 */
public interface IFloatingViewManager {
    /**
     * remove.
     */
    void remove();

    /**
     * add.
     *
     * @param applicationContext context.
     */
    void add(Context applicationContext);

    /**
     * attach.
     *
     * @param activity activity.
     */
    void attach(Activity activity);

    /**
     * attach.
     *
     * @param container layout.
     */
    void attach(FrameLayout container);

    /**
     * detach.
     *
     * @param activity activity.
     */
    void detach(Activity activity);

    /**
     * detach.
     *
     * @param container layout.
     */
    void detach(FrameLayout container);

}
