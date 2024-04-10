/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
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
