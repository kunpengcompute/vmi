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

package com.huawei.cloudphonesdk.maincontrol;

public class NetConfig {
    static {
        System.loadLibrary("VideoEngineJni");
    }

    public static final int COMM_PROTO_TCP = 0;
    public static final int COMM_PROTO_NSTACK = 1;
    public static final int COMM_PROTO_UNKNOWN = 2;

    public static native boolean initialize();

    public static native boolean setNetConfig(String ip, int port, int connectType);
}
