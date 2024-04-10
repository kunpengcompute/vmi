/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudgame.touch;

/**
 * interface TOUCHSENDHOOK.
 *
 * @since 2018-07-05
 */
public interface TOUCHSENDHOOK {
    /**
     * The interface for sending touch data.
     *
     * @param data byte[]. int length.
     * @param length int length
     */
    void touchSendData(byte[] data, int length);
}
