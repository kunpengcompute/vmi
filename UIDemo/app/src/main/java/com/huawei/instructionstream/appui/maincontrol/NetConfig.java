/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

/**
 * NetConfig.
 *
 * @since 2021-03-25
 */
public class NetConfig {
    static {
        System.loadLibrary("InstructionEngineJni");
    }

    public static final int COMM_PROTO_TCP = 0;
    public static final int COMM_PROTO_NSTACK = 1;
    public static final int COMM_PROTO_UNKNOWN = 2;

    public static native boolean initialize();

    public static native boolean setNetConfig(String ip, int port, int connectType);
}