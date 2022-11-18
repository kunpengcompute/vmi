/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 */

package com.huawei.cloudphonesdk.audio.play;

/**
 * AudioRemoteMessage.
 *
 * @since 2018-07-05
 */
public class AudioRemoteMessage {
    /**
     * MSG_SET.
     */
    public static final int MSG_SET = 0;
    /**
     * MSG_START.
     */
    public static final int MSG_START = 1;
    /**
     * MSG_READ.
     */
    public static final int MSG_READ = 2;
    /**
     * MSG_WRITE.
     */
    public static final int MSG_WRITE = 3;
    /**
     * MSG_STOP.
     */
    public static final int MSG_STOP = 4;
    /**
     * MSG_DESTRUCT.
     */
    public static final int MSG_DESTRUCT = 5;
    /**
     * MSG_SETVOLUME.
     */
    public static final int MSG_SETVOLUME = 6;
    /**
     * MSG_WRITE_PCM.
     */
    public static final int MSG_WRITE_PCM = 7;
    /**
     * SIZEOFINT.
     */
    public static final int SIZEOFINT = 4;
    /**
     * SIZEOFLONG.
     */
    public static final int SIZEOFLONG = 8;
    /**
     * SIZEOFFLOAT.
     */
    public static final int SIZEOFFLOAT = 4;
    private byte[] messageBuffer;
    private int readPosition = 0;
    private int writePosition = 0;

    /**
     * copy a message.
     *
     * @param buffer destination bufffer
     * @param start  start position
     * @param size   size to copy
     */
    public AudioRemoteMessage(byte[] buffer, int start, int size) {
        int size1 = size;
        if (start > 0) {
            if (start + size > buffer.length) {
                size1 = buffer.length - start;
            }
            messageBuffer = new byte[size1];
        } else {
            messageBuffer = new byte[size1];
        }
        System.arraycopy(buffer, start, messageBuffer, 0, size1);
    }

    private static long bytesToLong(byte[] inputByte) {
        return (inputByte[7] & 0xFF) | (inputByte[6] & 0xFF) << 8
                | (inputByte[5] & 0xFF) << 16 | (inputByte[4] & 0xFF) << 24 | (inputByte[3] & 0xFF) << 32
                | (inputByte[2] & 0xFF) << 40 | (inputByte[1] & 0xFF) << 48 | (inputByte[0] & 0xFF) << 56;
    }

    /**
     * read 64 bits.
     *
     * @return a long value
     */
    public long readLong() {
        long high = readInt();
        long low = readInt();
        long longValue = 0;

        longValue = longValue | high;
        longValue = longValue << 32;
        /* note the below 0x00000000ffffffffL is important */
        longValue = longValue | (low & 0x00000000ffffffffL);

        return longValue;
    }

    /**
     * read 32 bits.
     *
     * @return a integer
     */
    public int readInt() {
        byte[] bytes = new byte[SIZEOFINT];
        System.arraycopy(messageBuffer, readPosition, bytes, 0, SIZEOFINT);
        readPosition += SIZEOFINT;
        return bytesToInt(bytes);
    }

    /**
     * check 32 bits integer without updating readPosition.
     *
     * @param offset offset
     * @return a integer
     */
    public int checkInt(int offset) {
        byte[] bytes = new byte[SIZEOFINT];
        System.arraycopy(messageBuffer, readPosition + offset, bytes, 0, SIZEOFINT);
        return bytesToInt(bytes);
    }

    /**
     * convert bytes to int.
     *
     * @param b byte array to convert
     * @return a integer
     */
    private static int bytesToInt(byte[] inputByte) {
        return (inputByte[3] & 0xFF) | (inputByte[2] & 0xFF) << 8
                | (inputByte[1] & 0xFF) << 16 | (inputByte[0] & 0xFF) << 24;
    }

    /**
     * read byte array.
     *
     * @param size size to read
     * @return a byte array
     */
    public byte[] readBytes(int size) {
        byte[] bytes = new byte[size];
        System.arraycopy(messageBuffer, readPosition, bytes, 0, size);
        readPosition += size;
        return bytes;
    }

    /**
     * read a float number.
     *
     * @return float number
     */
    public float readFloat() {
        byte[] bytes = new byte[SIZEOFFLOAT];
        System.arraycopy(messageBuffer, readPosition, bytes, 0, SIZEOFFLOAT);
        readPosition += SIZEOFFLOAT;
        return bytesToFloat(bytes);
    }

    /**
     * convert byte array to float.
     *
     * @param b byte array
     * @return a float number
     */
    private static float bytesToFloat(byte[] inputByte) {
        int intbits = bytesToInt(inputByte);
        return Float.intBitsToFloat(intbits);
    }

    /**
     * convert integer to byte array and write.
     *
     * @param value the value to write
     */
    public void writeInt(int value) {
        System.arraycopy(intToBytes(value), 0, messageBuffer, writePosition, SIZEOFINT);
        writePosition += SIZEOFINT;
    }

    /**
     * convert integer to byte array.
     *
     * @param a integer to convert
     * @return byte array converted
     */
    private static byte[] intToBytes(int inputInt) {
        return new byte[]{
                (byte) ((inputInt >> 24) & 0xFF),
                (byte) ((inputInt >> 16) & 0xFF),
                (byte) ((inputInt >> 8) & 0xFF),
                (byte) (inputInt & 0xff)
        };
    }

    /**
     * write a long value.
     *
     * @param value the value to write
     */
    public void writeLong(int value) {
        System.arraycopy(longToBytes(value), 0, messageBuffer, writePosition, SIZEOFLONG);
        writePosition += SIZEOFLONG;
    }

    /**
     * convert a long to byte array.
     *
     * @param a the long number to convert
     * @return resulting byte array
     */
    private static byte[] longToBytes(long inputLong) {
        return new byte[]{
                (byte) ((inputLong >> 56) & 0xFF),
                (byte) ((inputLong >> 48) & 0xFF),
                (byte) ((inputLong >> 40) & 0xFF),
                (byte) ((inputLong >> 32) & 0xFF),
                (byte) ((inputLong >> 24) & 0xFF),
                (byte) ((inputLong >> 16) & 0xFF),
                (byte) ((inputLong >> 8) & 0xFF),
                (byte) (inputLong & 0xff)
        };
    }

    /**
     * write a byte array.
     *
     * @param bytes byte array to write
     */
    public void writeBytes(byte[] bytes) {
        System.arraycopy(bytes, 0, messageBuffer, writePosition, bytes.length);
        writePosition += bytes.length;
    }

    /**
     * write a float number.
     *
     * @param value float number to write
     */
    public void writeFloat(float value) {
        System.arraycopy(floatToBytes(value), 0, messageBuffer, writePosition, SIZEOFFLOAT);
        writePosition += SIZEOFFLOAT;
    }

    /**
     * convert a float to byte array.
     *
     * @param a a float number
     * @return resulting byte array
     */
    private static byte[] floatToBytes(float inputFloat) {
        int intbits = Float.floatToIntBits(inputFloat);
        return intToBytes(intbits);
    }

    /**
     * getter.
     *
     * @return byte array of buffer
     */
    public byte[] getBuffer() {
        return messageBuffer;
    }

    /**
     * get message buffer's size.
     *
     * @return length of message buffer
     */
    public int getSize() {
        return messageBuffer.length;
    }
}
