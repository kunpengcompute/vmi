/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

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
