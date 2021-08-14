/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.instructionstream.appui.engine;

/**
 * Native Listener.
 *
 * @since 2018-07-05
 */
public interface NativeListener {
    /**
     * VMI Instruction Engine event notice.
     *
     * @param event VMI Instruction Engine event.
     */
    void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3);
}
