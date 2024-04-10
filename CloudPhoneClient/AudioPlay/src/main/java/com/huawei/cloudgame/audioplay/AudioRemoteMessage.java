/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

/**
 * AudioRemoteMessage.
 *
 * @since 2018-07-05
 */
public class AudioRemoteMessage {
    /** return when execution succeeds */
    protected static final int SUCC = 0;

    /** return when execution fails */
    protected static final int FAIL = -1;

    /** commond word to set the audio player */
    protected static final int MSG_SET = 0;

    /** commond word to start the audio player */
    protected static final int MSG_START = 1;

    /** commond word to read audio data for recoder */
    protected static final int MSG_READ = 2;

    /** command word to write opus data */
    protected static final int MSG_WRITE_OPUS = 3;

    /** command word to stop audio player */
    protected static final int MSG_STOP = 4;

    /** command word to destruct audio player */
    protected static final int MSG_DESTRUCT = 5;

    /** command word to set volume */
    protected static final int MSG_SETVOLUME = 6;

    /** command word to write pcm data */
    protected static final int MSG_WRITE_PCM = 7;

    /** 4 bytes */
    protected static final int SIZE_OF_INT = 4;

    /** 8 bytes */
    protected static final int SIZE_OF_LONG = 8;

    /** 4 bytes */
    protected static final int SIZE_OF_FLOAT = 4;

    protected static final int RETURN_AUDIO_PLAY_DATA = 16908289;

    protected static final int SET_CLIENT_VOLUME = 16777218;

    protected static final int AUDIO_DATA_MSG_LEN = 30;

    private byte[] messageBuffer;
    private int readPosition = 0;
    private int writePosition = 0;
    private int length = 0;
    private byte[] payload;
    private int payloadOffset;

    /**
     * AudioRemoteMessage.
     *
     * @param size size
     */
    protected AudioRemoteMessage(int size) {
        messageBuffer = new byte[size];
        length = size;
    }

    /**
     * AudioRemoteMessage.
     *
     * @param buffer byte[] buffer
     */
    protected AudioRemoteMessage(byte[] buffer) {
        this(buffer.length);
        System.arraycopy(buffer, 0, messageBuffer, 0, buffer.length);
    }

    /**
     * read 64 bits.
     *
     * @return a long value
     */
    protected long readLong() {
        long low = readInt();
        long high = readInt();
        long longValue = 0L;

        longValue = high << 32; // 此处将低32位数据移动至高32位，后续拼接低32位数据

        /* note the below 0x00000000ffffffffL is important */
        return longValue | (low & 0x00000000ffffffffL);
    }

    /**
     * read 32 bits integer without updating readPosition.
     *
     * @param offset end index.
     * @return a integer
     */
    protected int lookInt(int offset) {
        int data = 0;
        int bits = SIZE_OF_INT - 1;
        if (offset < 0) {
            return data;
        }
        for (int i = 0; i < SIZE_OF_INT; i++) {
            int index = readPosition + offset + i;
            if (index >= length) {
                return data;
            }
            data |= (messageBuffer[index] & 0xff) << (i << bits);
        }
        return data;
    }

    /**
     * read 32 bits.
     *
     * @return a integer
     */
    protected int readInt() {
        int data = lookInt(0);
        readPosition += SIZE_OF_INT;
        return data;
    }

    /**
     * read 8 bits.
     *
     * @return a integer
     */
    protected int readByte() {
        int data = messageBuffer[readPosition];
        readPosition += 1;
        return data;
    }

    /**
     * read a float number.
     *
     * @return float number
     */
    protected float readFloat() {
        int data = readInt();
        readPosition += SIZE_OF_FLOAT;
        return Float.intBitsToFloat(data);
    }

    /**
     * convert integer to byte array and write.
     *
     * @param value the value to write
     * @return SUCC 0 or FAIL -1
     */
    protected int writeInt(int value) {
        if (length < writePosition + SIZE_OF_INT) {
            return FAIL;
        }
        for (int i = 0; i < SIZE_OF_INT; i++) {
            messageBuffer[writePosition + i] =
                (byte) (value >> ((SIZE_OF_INT - 1 - i) << 3)); // 左移动3位相当于*8，计算出需要移动的字节位数
        }
        writePosition += SIZE_OF_INT;
        return SUCC;
    }

    /**
     * get message buffer's size.
     *
     * @return length of message buffer
     */
    protected int getSize() {
        return length;
    }

    /**
     * init AudioRemoteMessage.
     *
     * @param buffer byte[] buffer
     * @param offset of valid data in buffer
     * @return SUCC 0 or FAIL -1
     */
    protected int init(byte[] buffer, int offset) {
        if (offset > messageBuffer.length) {
            return FAIL;
        }
        System.arraycopy(buffer, 0, messageBuffer, 0, offset);
        readPosition = 0;
        writePosition = 0;
        length = offset;
        return SUCC;
    }

    /**
     * set AudioRemoteMessage payload.
     *
     * @param buffer byte[] buffer
     * @param offset end index.
     */
    protected void setPayload(byte[] buffer, int offset) {
        payload = buffer;
        payloadOffset = offset;
    }

    /**
     * get message buffer getPayload.
     *
     * @return address of message buffer payload
     */
    protected byte[] getPayload() {
        return payload;
    }

    /**
     * get message buffer getPayload offset.
     *
     * @return offset of message buffer payload
     */
    protected int getPayloadOffset() {
        return payloadOffset;
    }
}