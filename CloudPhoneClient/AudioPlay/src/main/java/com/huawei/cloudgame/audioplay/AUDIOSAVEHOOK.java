/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

/**
 * interface AUDIOSAVEDHOOK.
 *
 * @since 2020-01-08
 */

public interface AUDIOSAVEHOOK {
    /**
     * Interface for saving audio client data.
     *
     * @param data audio data packet
     * @param length audio data length
     */
    void audioSaveData(byte[] data, int length);
}
