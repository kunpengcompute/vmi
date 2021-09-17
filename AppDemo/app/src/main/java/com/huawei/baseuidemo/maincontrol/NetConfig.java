package com.huawei.baseuidemo.maincontrol;

public class NetConfig {

    // loadJni库
    static {
        System.loadLibrary("InstructionEngineJni");
    }

    /**
     * 加载原型通信库函数
     *
     * @return true，代表加载通信库运行；false代表加载通信库失败
     */
    public static native boolean initialize();

    /**
     * 设置网路参数，云手机的IP或者port
     *
     * @param ip 代表云手机服务器的IP
     * @param port 代表云手机服务器的port
     * @param type 代表通信连接类型
     * @return true，代表加载通信库运行；false代表加载通信库失败
     */
    public static native boolean setNetConfig(String ip, int port, int type);
}
