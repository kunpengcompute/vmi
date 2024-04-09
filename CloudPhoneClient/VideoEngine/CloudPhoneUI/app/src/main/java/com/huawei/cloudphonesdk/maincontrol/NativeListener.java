/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 */
package com.huawei.cloudphonesdk.maincontrol;

/**
 * Native Listener.
 *
 * @since 2022-08-12
 */
public interface NativeListener {

    /**
     * video engine event notice.
     *
     * @param event video engine event.
     */
    void onVmiVideoEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3, String additionInfo);
}
