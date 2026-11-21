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

package com.huawei.cloudphone.util;

/**
 * TestFlag.
 *
 * @since 2019-12-20
 */
public class TestFlag {
    /**
     * test vmi Operation client flag.
     */
    public static boolean testTouch;
    /**
     * test vmi audio client flag.
     */
    public static boolean testAudioClient;

    public TestFlag(boolean testTouch, boolean testAudioClient) {
        this.testTouch = testTouch;
        this.testAudioClient = testAudioClient;
    }
}
