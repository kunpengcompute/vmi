/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
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
