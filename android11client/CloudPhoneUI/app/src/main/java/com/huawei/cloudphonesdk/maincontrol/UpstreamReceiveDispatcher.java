/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.cloudphonesdk.maincontrol;

import android.media.AudioManager;
import android.provider.MediaStore;
import android.util.Log;

import com.huawei.cloudphonesdk.audio.play.AudioPlayerCallback;

import java.lang.ref.WeakReference;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class UpstreamReceiveDispatcher {
    private static final UpstreamReceiveDispatcher SINGLE_INSTANCE = new UpstreamReceiveDispatcher();

    // 1MB
    private static final int MAX_BUF_LEN = 1048576;
    private static final String TAG = "UpstreamReceiveDispatch";
    private Map<Byte, NewPacketCallback> mCallbackMap = new ConcurrentHashMap<>();
    private DefaultThread mDataThread;


    /**
     * add callback.
     *
     * @param tag      tag
     * @param callback callback
     */
    public void addNewPacketCallback(Byte tag, NewPacketCallback callback) {
        mCallbackMap.put(tag, callback);
    }

    /**
     * delete New Packet Callback
     *
     * @param tag tag
     */
    public void deleteNewPacketCallback(Byte tag) {
        mCallbackMap.remove(tag);
    }

    /**
     * stop Blocked
     */
    public void stopBlocked() {
        mCallbackMap.clear();
        mDataThread.stopFlag = true;
        try {
            Method method = AudioPlayerCallback.audioTrackPlayer.getClass().getDeclaredMethod("stopPlayThread");
            method.setAccessible(true);
            method.invoke(AudioPlayerCallback.audioTrackPlayer);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
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


    class DefaultThread extends Thread {
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
                        Log.e(TAG, "sleep interrupted");
                    }
                }
            }
        }
    }
}
