/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

/**
 * interface NewPacketCallback.
 *
 * @since 2018-07-05
 */
public interface NewPacketCallback {
    /**
     * onNewPacket.
     *
     * @param data byte[].
     */
    void onNewPacket(byte[] data);
}
