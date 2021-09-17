package com.huawei.baseuidemo.engine;

import android.util.Log;
import android.view.Surface;

public class InstructionEngine {

    private static final String TAG = "VMI_INSTRUCTION_ENGINE";

    public static final int VMI_SUCCESS = 0; //返回成功
    public static final int VMI_INSTRUCTION_ENGINE_INVALID_PARAM = 1; //返回非法参数
    public static final int VMI_INSTRUCTION_ENGINE_UNINIT_SOCK = 2; // 返回socket未初始化
    public static final int VMI_INSTRUCTION_ENGINE_SEND_ALLOC_FAILED = 3; // 返回分配发送内存失败
    public static final int VMI_INSTRUCTION_ENGINE_SEND_MEMSET_FAILED = 4; // 返回发送内存置0失败
    public static final int VMI_INSTRUCTION_ENGINE_SEND_MEMCPY_FAILED = 5; // 返回发送内存拷贝失败
    public static final int VMI_INSTRUCTION_ENGINE_SEND_FAIL = 6; // 返回socket发送失败
    public static final int VMI_INSTRUCTION_ENGINE_HOOK_REGISTER_FAIL = 7; // 返回hook函数注册失败
    public static final int VMI_INSTRUCTION_CLIENT_INVALID_PARAM = 0x0A050001; // 返回客户端引擎非法参数
    public static final int VMI_INSTRUCTION_CLIENT_START_FAIL = 0x0A050002; // 返回客户端引擎启动失败
    public static final int VMI_INSTRUCTION_CLIENT_ALREADY_STARTED = 0x0A050003; // 返回客户端引擎已经启动
    public static final int VMI_INSTRUCTION_CLIENT_STOP_FAIL = 0x0A050004; // 返回客户端引擎停止失败
    public static final int VMI_INSTRUCTION_CLIENT_SEND_HOOK_REGISTER_FAIL = 0x0A050005; // 返回客户端引擎注册hook函数失败
    public static final int VMI_INSTRUCTION_CLIENT_SEND_FAIL = 0x0A050006; // 返回客户端引擎socket函数发送失败
    public static final int VMI_INSTRUCTION_CLIENT_SEND_AGAIN = 0x0A050007; // 返回客户端引擎socket函数发送重试
    public static final int VMI_INSTRUCTION_CLIENT_INITIALIZE_FAIL = 0x0A050008; // 返回客户端引擎初始化失败
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_SOCK_DISCONN = -2; // 连接断开
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_PKG_BROKEN = -3; // 数据包损坏
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_VERSION_ERROR = -4; // 服务端和客户端的版本不匹配
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_READY = -5; // 引擎渲染第一帧画面成功
    public static final int VMI_INSTRUCTION_ENGINE_EVENT_ORIENTATION_CHANGED = -6; // 服务端方向转屏事件

    /**
     * 枚举转屏方向
     */
    public enum Rotation {
        ROTATION0(0),
        ROTATION90(1),
        ROTATION180(2),
        ROTATION270(3);

        private final int value;

        Rotation(int value) {
            this.value = value;
        }

        public int getValue() {
            return this.value;
        }
    }

    /**
     * 指令流引擎初始化函数
     *
     * @return 初始化结果
     */
    public int initialize() {
        int result = InstructionWrapper.initialize();
        Log.i(TAG, "InstructionWrapper initialize result: " + result);
        return result;
    }

    /**
     * 指令流引擎初始化启动函数
     *
     * @param surface 代表java层传下来的surface
     * @param width 代表surface的宽度
     * @param height 代表surface的高度
     * @param desityDpi 代表surface的像素密度
     * @return VMI_SUCCESS，代表启动RenderWindow成功；其他代表启动RenderWindow失败。
     */
    public int start(Surface surface, int width, int height, float desityDpi) {
        int result = InstructionWrapper.start(surface, width, height, desityDpi);
        if (result != InstructionEngine.VMI_SUCCESS) {
            Log.e(TAG, "start instruction engine failed, result: " + result);
            return result;
        }
        return result;
    }

    /**
     * 指令流引擎停止函数
     */
    public void stop() {
        Log.i(TAG, "stop instruction engine.");
        InstructionWrapper.stop();
    }

    /**
     * 指令流引擎获取帧率信息
     *
     * @return 返回帧率统计信息
     */
    public String getStat() {
        Log.i(TAG, "get instruction engine stat.");
        return InstructionWrapper.getStat();
    }

    /**
     * 获取agent端传输过来的数据，指令流，心跳，音频信息
     *
     * @param type 代表数据类型
     * @param data 代表接收数组buffer
     * @param length 代表数组长度
     * @return 实际接受到数据长度
     */
    public int recvData(byte type, byte[] data, int length) {
        Log.i(TAG, "recv instruction engine data.");
        return InstructionWrapper.recvData(type, data,length);
    }

    /**
     * 发送控数据到agent端
     *
     * @param data 代表音频数据
     * @param length 代表音频数据长度
     * @return 发送结果
     */
    public boolean sendAudioDataArray(byte[] data, int length) {
        return InstructionWrapper.sendAudioDataArray(data, length);
    }

    /**
     * 发送触控数据到agent端
     *
     * @param data 代表多点触控Y坐标
     * @param length 代表发送触控信息的长度
     * @return 发送结果
     */
    public boolean sendTouchEventArray(byte[] data, int length) {
        return InstructionWrapper.sendTouchEventArray(data, length);
    }

    /**
     * 发送模拟导航栏触控数据到agent端
     *
     * @param data 代表音频数据
     * @param length 代表音频数据长度
     * @return 发送结果
     */
    public boolean sendKeyEvent(byte[] data, int length) {
        return InstructionWrapper.sendKeyEvent(data, length);
    }
}
