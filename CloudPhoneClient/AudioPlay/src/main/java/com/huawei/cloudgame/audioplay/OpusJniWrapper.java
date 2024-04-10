/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 */

package com.huawei.cloudgame.audioplay;

/**
 * OpusJniWrapper.
 *
 * @since 2018-07-05
 */
public class OpusJniWrapper {
    static {
        System.loadLibrary("audioplay");
    }

    /**
     * create opus decoder
     *
     * @return return code
     */
    protected static native long createOpusDecoder();

    /**
     * destroy opus decoder
     *
     * @param decoder decoder
     * @return return code
     */
    protected static native int destroyOpusDecoder(long decoder);

    /**
     * opus decoder
     *
     * @param decoder decoder
     * @param inputBuffer inputBuffer
     * @param inputBufferLen inputBufferLen
     * @param outBuffer outBuffer
     * @return return code
     */
    protected static native int opusDecode(long decoder, byte[] inputBuffer, int inputBufferLen, short[] outBuffer);

    /**
     * create opus encoder
     *
     * @param sampleRate  sample rate
     * @param channels channels
     * @param bitrate bit rate
     * @return return code
     */
    protected static native int createOpusEncoder(int sampleRate, int channels, int bitrate);

    /**
     * destroy opus encoder
     *
     * @return return code
     */
    protected static native int destroyOpusEncoder();

    /**
     * opus encoder
     *
     * @param inputBuffer input buffer
     * @param frameSize frame size
     * @param outBuffer out buffer
     * @return return code
     */
    protected static native int opusEncode(short[] inputBuffer, int frameSize, byte[] outBuffer);

    /**
     * get opus encoder status
     *
     * @return return code
     */
    protected static native int getOpusEncoderStatus();
}
