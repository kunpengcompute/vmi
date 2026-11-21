/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * vmi is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
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
