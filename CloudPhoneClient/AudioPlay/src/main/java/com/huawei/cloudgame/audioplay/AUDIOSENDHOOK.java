/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

/**
 * interface AUDIOSENDHOOK.
 *
 * @since 2020-01-08
 */
public interface AUDIOSENDHOOK {
    /**
     * Interface for sending audio client data.
     *
     * @param data audio data packet
     * @param length audio data length
     */
    void audioSendData(byte[] data, int length);
}
