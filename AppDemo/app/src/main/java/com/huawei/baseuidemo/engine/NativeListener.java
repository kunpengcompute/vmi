package com.huawei.baseuidemo.engine;

public interface NativeListener {
    void onVmiInstructionEngineEvent(int event, int reserved0, int reserved1, int reserved2, int reserved3);
}
