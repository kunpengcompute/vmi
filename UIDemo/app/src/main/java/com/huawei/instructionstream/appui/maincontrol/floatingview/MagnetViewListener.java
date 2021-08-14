/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol.floatingview;

/**
 * MagnetViewListener interface.
 *
 * @since 2018-07-05
 */
public interface MagnetViewListener {
    /**
     * onRemove.
     *
     * @param magnetView view.
     */
    void onRemove(DragView magnetView);

    /**
     * click.
     *
     * @param magnetView view.
     */
    void onClick(DragView magnetView);
}
