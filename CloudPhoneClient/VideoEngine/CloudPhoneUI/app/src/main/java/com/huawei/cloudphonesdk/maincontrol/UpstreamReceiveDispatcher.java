/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import androidx.annotation.Keep;

import java.lang.ref.WeakReference;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.lang.reflect.Method;

import com.huawei.cloudphonesdk.audio.play.AudioPlayerCallback;
import com.huawei.cloudphonesdk.utils.LogUtil;

/**
 * UpstreamReceiveDispatcher.
 *
 * @since 2018-07-05
 */
@Keep
public class UpstreamReceiveDispatcher {
    private static final String TAG = UpstreamReceiveDispatcher.class.getSimpleName();
    private Map<Byte, NewPacketCallback> mCallbackMap = new ConcurrentHashMap<>();
    private DefaultThread mDataThread;

    /**
     * add callback.
     *
     * @param tag      VmiMsgType
     * @param callback NewPacketCallback
     */
    public void addNewPacketCallback(Byte tag, NewPacketCallback callback) {
        LogUtil.error(TAG, "callback added " + tag);
        mCallbackMap.put(tag, callback);
    }

    /**
     * delete New Packet Callback
     *
     * @param tag tag
     */
    public void deleteNewPacketCallback(Byte tag) {
        LogUtil.error(TAG, "callback removed " + tag);
        mCallbackMap.remove(tag);
    }

    /**
     * stop Blocked
     */
    public synchronized void stopBlocked() {
        mCallbackMap.clear();
        mDataThread.stopFlag = true;
        try {
            Method method = AudioPlayerCallback.audioTrackPlayer.getClass().getDeclaredMethod("stopPlayThread");
            method.setAccessible(true);
            method.invoke(AudioPlayerCallback.audioTrackPlayer);
        } catch (ReflectiveOperationException e2) {
            LogUtil.error(TAG, "call stopPlayThread failed");
        }
    }

    /**
     * start
     */
    public synchronized void start() {
        if (this.mDataThread == null) {
            this.mDataThread = new DefaultThread(this);
        }
        mDataThread.stopFlag = false;
        mDataThread.start();
    }

    static class DefaultThread extends Thread {
        // 1MB
        private static final int MAX_BUF_LEN = 1048576;
        WeakReference<UpstreamReceiveDispatcher> mWeakReference;
        private boolean stopFlag;

        public DefaultThread(UpstreamReceiveDispatcher dispatcher) {
            mWeakReference = new WeakReference<>(dispatcher);
        }

        @Override
        public void run() {
            byte[] recvBuf = new byte[MAX_BUF_LEN];
            while (!stopFlag) {
                boolean hasData = false;
                UpstreamReceiveDispatcher dispatcher = mWeakReference.get();
                if (dispatcher == null) {
                    return;
                }
                for (Map.Entry<Byte, NewPacketCallback> entry : dispatcher.mCallbackMap.entrySet()) {
                    int packetLen = OpenGLJniWrapper.recvData(entry.getKey(), recvBuf, recvBuf.length);
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
                        LogUtil.error(TAG, "sleep interrupted");
                    }
                }
            }
        }
    }
}
