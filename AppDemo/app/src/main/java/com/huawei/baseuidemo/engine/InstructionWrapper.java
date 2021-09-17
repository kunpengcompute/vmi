package com.huawei.baseuidemo.engine;

import android.view.Surface;

public class InstructionWrapper {

    public static final byte AUDIO = 3;
    // loadJni库
    static {
        System.loadLibrary("InstructionEngineJni");
    }

    /**
     * 指令流引擎初始化函数
     *
     * @return VMI_SUCCESS，代表引擎初始化成功；其他代表引擎初始化失败。
     */
    public static native int initialize();

    /**
     * 指令流引擎初始化启动函数
     *
     * @param surface 代表java层传下来的surface
     * @param width 代表surface的宽度
     * @param height 代表surface的高度
     * @param desityDpi 代表surface的像素密度
     * @return VMI_SUCCESS，代表启动RenderWindow成功；其他代表启动RenderWindow失败。
     */
    public static native int start(Surface surface, int width, int height, float desityDpi);

    /**
     * 指令流引擎停止函数
     */
    public static native void stop();

    /**
     * 指令流引擎获取帧率信息
     *
     * @return 返回帧率统计信息
     */
    public static native String getStat();

    /**
     * 获取agent端传输过来的数据，指令流，心跳，音频信息
     *
     * @param type 代表数据类型
     * @param data 代表接收数组buffer
     * @param length 代表数组长度
     * @return 实际接受到数据长度
     */
    public static native int recvData(byte type, byte[] data, int length);

    /**
     * 发送控数据到agent端
     *
     * @param data 代表音频数据
     * @param length 代表音频数据长度
     * @return 发送结果
     */
    public static native boolean sendAudioDataArray(byte[] data, int length);

    /**
     * 发送触控数据到agent端
     *
     * @param data 代表多点触控Y坐标
     * @param length 代表发送触控信息的长度
     * @return 发送结果
     */
    public static native boolean sendTouchEventArray(byte[] data, int length);

    /**
     * 发送模拟导航栏触控数据到agent端
     *
     * @param data 代表音频数据
     * @param length 代表音频数据长度
     * @return 发送结果
     */
    public static native boolean sendKeyEvent(byte[] data, int length);
}
