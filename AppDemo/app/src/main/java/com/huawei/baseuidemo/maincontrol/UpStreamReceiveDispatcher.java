package com.huawei.baseuidemo.maincontrol;

import android.util.Log;

import com.huawei.baseuidemo.engine.InstructionEngine;
import com.huawei.baseuidemo.engine.InstructionWrapper;
import com.huawei.baseuidemo.utils.ThreadPool;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class UpStreamReceiveDispatcher {

    private static final String TAG = "VMIUPSTREAMRECEIVEDISPATCH";
    private static final int MAX_BUF_LEN = 1048576;
    private volatile boolean stopFlag = false;
    private InstructionEngine engine;
    private final ThreadPool threadPool = new ThreadPool(3, 6);
    private static UpStreamReceiveDispatcher SINGLE_INSTANCE = new UpStreamReceiveDispatcher();
    private static final Map<Byte, NewPacketCallback> NEW_PACKET_CALLBACK_MAP = new ConcurrentHashMap<>();
    private final Lock threadCreationLock = new ReentrantLock();
    private ConsumerThread audioThread;

    /**
     * 实例化
     *
     * @return UpStreamReceiveDispatcher
     */
    public static UpStreamReceiveDispatcher getInstance() {
        if (SINGLE_INSTANCE == null) {
            SINGLE_INSTANCE = new UpStreamReceiveDispatcher();
        }
        return SINGLE_INSTANCE;
    }

    /**
     * 设置指令流引擎
     *
     * @param engine
     */
    public void setInstructionEngine(InstructionEngine engine) {
        this.engine = engine;
    }

    /**
     * 注册音频回调
     *
     * @param tag
     * @param callback
     */
    public void addNewPacketCallback(byte tag, NewPacketCallback callback) {
        Log.i(TAG, "add new packet callback.");
        NEW_PACKET_CALLBACK_MAP.put(tag, callback);
    }

    /**
     * 启动逻辑线程接收音频数据
     */
    public void start() {
        Log.i(TAG, "start entry.");
        stopFlag = false;
        threadCreationLock.lock();

        NewPacketCallback callback = NEW_PACKET_CALLBACK_MAP.get(InstructionWrapper.AUDIO);
        if (callback != null) {
            this.audioThread = new ConsumerThread(InstructionWrapper.AUDIO, callback);
            this.threadPool.submit(this.audioThread);
        }
    }

    class ConsumerThread implements Runnable {
        NewPacketCallback callback;
        Byte mType;

        ConsumerThread(Byte dataType, NewPacketCallback callback) {
            mType = dataType;
            this.callback = callback;
        }

        @Override
        public void run() {
            byte[] recvBuf = new byte[MAX_BUF_LEN];
            while (!stopFlag) {
                // 循环调用recvData接收数据
                int packetLen = engine.recvData(mType, recvBuf, recvBuf.length);
                if (packetLen > 0) {
                    byte[] copyData = new byte[packetLen];
                    System.arraycopy(recvBuf, 0, copyData, 0, packetLen);
                    // 将数据回调给钩子函数
                    callback.onNewPacket(copyData);
                    continue;
                }
            }
        }
    }

    /**
     * 停止线程
     */
    public void stopRunnable() {
        Log.i(TAG, "stop runnable start.");
        NEW_PACKET_CALLBACK_MAP.clear();
        stopFlag = true;
        threadCreationLock.lock();
        threadPool.remove(audioThread);
        this.audioThread = null;
        threadCreationLock.unlock();
        Log.i(TAG, "stop runnable exit.");
    }
}
