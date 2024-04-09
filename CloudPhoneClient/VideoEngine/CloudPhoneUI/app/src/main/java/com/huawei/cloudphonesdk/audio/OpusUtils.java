package com.huawei.cloudphonesdk.audio;

import com.huawei.cloudgame.audioplay.OpusJniWrapper;

public class OpusUtils extends OpusJniWrapper {
    /**
     * create opus decoder
     *
     * @return return code
     */
    public static long createOpusDecoder() {
        return OpusJniWrapper.createOpusDecoder();
    }

    /**
     * destroy opus decoder
     *
     * @param decoder decoder
     * @return return code
     */
    public static int destroyOpusDecoder(long decoder) {
        return OpusJniWrapper.destroyOpusDecoder(decoder);
    }

    /**
     * opus decoder
     *
     * @param decoder        decoder
     * @param inputBuffer    inputBuffer
     * @param inputBufferLen inputBufferLen
     * @param outBuffer      outBuffer
     * @return return code
     */
    public static int opusDecode(long decoder, byte[] inputBuffer, int inputBufferLen, short[] outBuffer) {
        return OpusJniWrapper.opusDecode(decoder, inputBuffer, inputBufferLen, outBuffer);
    }

    /**
     * create opus encoder
     *
     * @param sampleRate sample rate
     * @param channels   channels
     * @param bitrate    bit rate
     * @return return code
     */
    public static int createOpusEncoder(int sampleRate, int channels, int bitrate) {
        return OpusJniWrapper.createOpusEncoder(sampleRate, channels, bitrate);
    }


    /**
     * destroy opus encoder
     *
     * @return return code
     */
    public static int destroyOpusEncoder() {
        return OpusJniWrapper.destroyOpusEncoder();
    }

    /**
     * opus encoder
     *
     * @param inputBuffer input buffer
     * @param frameSize   frame size
     * @param outBuffer   out buffer
     * @return return code
     */
    public static int opusEncode(short[] inputBuffer, int frameSize, byte[] outBuffer) {
        return OpusJniWrapper.opusEncode(inputBuffer, frameSize, outBuffer);
    }

    /**
     * get opus encoder status
     *
     * @return return code
     */
    public static int getOpusEncoderStatus() {
        return OpusJniWrapper.getOpusEncoderStatus();
    }
}
