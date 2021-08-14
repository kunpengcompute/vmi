/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.instructionstream.appui.maincontrol;

import com.huawei.instructionstream.appui.utils.LogUtils;
import com.huawei.instructionstream.appui.utils.ThreadPool;
import com.huawei.instructionstream.appui.engine.InstructionEngine;
import com.huawei.instructionstream.appui.engine.InstructionWrapper;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * UpstreamReceiveDispatcher.
 *
 * @since 2018-07-05
 */
public class UpstreamReceiveDispatcher {
    private static final UpstreamReceiveDispatcher SINGLE_INSTANCE = new UpstreamReceiveDispatcher();

    // 1MB
    private static final int MAX_BUF_LEN = 1048576;
    private static final String TAG = "UpstreamReceiveDispatch";
    private static final Map<Byte, NewPacketCallback> NEW_PACKET_CALLBACK_MAP = new ConcurrentHashMap<>();
    private volatile boolean stopFlag = false;
    private volatile boolean stopped = false;
    private final ThreadPool threadPool = new ThreadPool(3, 6);
    private ConsumerThread audioThread;
    private Runnable otherThread;
    private Lock threadCreationLock = new ReentrantLock();
    private InstructionEngine engine;

    private UpstreamReceiveDispatcher() {
        this.audioThread = null;
        this.otherThread = null;
    }

    /**
     * getInstance.
     *
     * @return UpstreamReceiveDispatcher.
     */
    public static UpstreamReceiveDispatcher getInstance() {
        return SINGLE_INSTANCE;
    }

    /**
     * set InstructionEngine.
     *
     * @param engine InstructionEngine
     */
    public void setInstructionEngine(InstructionEngine engine) {
        this.engine = engine;
    }

    /**
     * add callback.
     *
     * @param tag tag
     * @param callback callback
     */
    public void addNewPacketCallback(Byte tag, NewPacketCallback callback) {
        LogUtils.error(TAG, "callback added " + tag);
        NEW_PACKET_CALLBACK_MAP.put(tag, callback);
    }

    /**
     * delete New Packet Callback
     *
     * @param tag tag
     */
    public void deleteNewPacketCallback(Byte tag) {
        LogUtils.error(TAG, "callback removed " + tag);
        NEW_PACKET_CALLBACK_MAP.remove(tag);
    }

    /**
     * stop Blocked
     */
    public void stopBlocked() {
        LogUtils.info(TAG, "stopBlocked Entry");
        NEW_PACKET_CALLBACK_MAP.clear();
        stopFlag = true;
        threadCreationLock.lock();
        threadPool.remove(audioThread);
        threadPool.remove(otherThread);
        this.audioThread = null;
        this.otherThread = null;
        threadCreationLock.unlock();
        LogUtils.info(TAG, "stopBlocked Exit");
    }

    /**
     * start
     */
    public void start() {
        LogUtils.info(TAG, "start Entry");
        stopFlag = false;
        threadCreationLock.lock();

        NewPacketCallback callback = NEW_PACKET_CALLBACK_MAP.get(InstructionWrapper.AUDIO);
        if (callback != null && this.audioThread == null) {
            this.audioThread = new ConsumerThread(InstructionWrapper.AUDIO, callback);
            this.threadPool.submit(this.audioThread);
        }

        if (this.otherThread == null) {
            this.otherThread = new DefaultThread();
            this.threadPool.submit(this.otherThread);
        }

        threadCreationLock.unlock();
        LogUtils.info(TAG, "start Exit");
    }

    /**
     * silentAudio
     */
    public void silentAudio(boolean skip) {
        if (this.audioThread != null) {
            this.audioThread.setSkipData(skip);
        }
    }

    class ConsumerThread implements Runnable {
        NewPacketCallback callback;
        Byte mtype;
        boolean skipData = false;

        ConsumerThread(Byte datatype, NewPacketCallback callback) {
            this.callback = callback;
            mtype = datatype;
        }

        public void setSkipData(boolean skipData) {
            this.skipData = skipData;
        }

        @Override
        public void run() {
            byte[] recvBuf = new byte[MAX_BUF_LEN];

            while (!stopFlag) {
                int packetLen = engine.recvData(mtype, recvBuf, recvBuf.length);
                if (packetLen > 0) {
                    byte[] copyData = new byte[packetLen];

                    System.arraycopy(recvBuf, 0, copyData, 0, packetLen);
                    if (!skipData) {
                        callback.onNewPacket(copyData);
                    }
                    // continue recv data when there is data found
                    continue;
                }

                // yield when there is not data at the moment
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    LogUtils.error(TAG, "sleep interrupted");
                }
            }
        }
    }

    class DefaultThread implements Runnable {
        @Override
        public void run() {
            byte[] recvBuf = new byte[MAX_BUF_LEN];

            while (!stopFlag) {
                boolean hasData = false;
                for (Map.Entry<Byte, NewPacketCallback> entry : NEW_PACKET_CALLBACK_MAP.entrySet()) {
                    // AudioTrack and AudioRecord has some issue in Android 8.0, pick audio out.
                    if (entry.getKey().equals(InstructionWrapper.AUDIO)) {
                        continue;
                    }

                    int packetLen = engine.recvData(entry.getKey(), recvBuf, recvBuf.length);
                    if (packetLen > 0) {
                        byte[] copyData = new byte[packetLen];
                        System.arraycopy(recvBuf, 0, copyData, 0, packetLen);
                        entry.getValue().onNewPacket(copyData);
                        hasData = true;
                    }
                }

                if (!hasData) {
                    // yield when there is no data at the moment
                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        LogUtils.error(TAG, "sleep interrupted");
                    }
                }
            }

            stopped = true;
        }
    }
}
