/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

/**
 * AudioClientData.
 *
 * @since 2020-01-08
 */
public class AudioClientData {
    private static final int SET_CLIENT_PLAY_QUEUE_SIZE = 16777217;

    private static final int SIZE_OF_INT = 4;
    private static final int AUDIO_CLIENT_DATA_LENGTH = 2;
    private static final int QSIZE_DATA_LENGTH = SIZE_OF_INT * 2;
    private static final int BYTE_SIZE_IN_BIT = 8;

    private int clientQueueSize = 0;

    /**
     * set client queue size value
     *
     * @param size the value of size to set
     */
    protected void setClientQueueSize(int size) {
        clientQueueSize = size;
    }

    /**
     * construct byte array of audio client data
     *
     * @return result in byte array
     */
    protected byte[] toByteArray() {
        byte[] result = new byte[AUDIO_CLIENT_DATA_LENGTH];

        // use big endian, namely network byte code
        result[0] = (byte) (clientQueueSize >> BYTE_SIZE_IN_BIT);
        result[1] = (byte) clientQueueSize;
        return result;
    }

    /**
     * construct byte array of audio client data
     *
     * @return result in byte array
     */
    protected byte[] queueSizeToByteArray() {
        byte[] data = new byte[QSIZE_DATA_LENGTH];
        // cmd,此处将客户端向服务端发送的音频数据头按照小端序封装
        data[0] = (byte) (SET_CLIENT_PLAY_QUEUE_SIZE);
        data[1] = (byte) (SET_CLIENT_PLAY_QUEUE_SIZE >> 8);
        data[2] = (byte) (SET_CLIENT_PLAY_QUEUE_SIZE >> 16);
        data[3] = (byte) (SET_CLIENT_PLAY_QUEUE_SIZE >> 24);
        // size
        data[4] = (byte) (clientQueueSize);
        data[5] = (byte) (clientQueueSize >> 8);
        data[6] = (byte) (clientQueueSize >> 16);
        data[7] = (byte) (clientQueueSize >> 24);
        
        return data;
    }
}
