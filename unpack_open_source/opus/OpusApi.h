#ifndef OPUSAPI_H
#define OPUSAPI_H
#endif
#if defined (__cplusplus)
extern "C" {
#endif

#include"opus.h"
OpusEncoder *OpusEncoderCreateApi(opus_int32 Fs, int channels, int application, int *error);

opus_int32 OpusEncodeApi(OpusEncoder *st, const opus_int16 *pcm, int frame_size,
    unsigned char *data, opus_int32 max_data_bytes);

int OpusEncoderCtlApi(OpusEncoder *st, int request, opus_int32 par);

void OpusEncoderDestroyApi(OpusEncoder *st);

OpusDecoder *OpusDecoderCreateApi(opus_int32 Fs, int channels, int *error);

int OpusDecodeApi(OpusDecoder *st, const unsigned char *data, opus_int32 len, opus_int16 *pcm, int frame_size,
    int decode_fec);

int OpusDecoderCtlApi(OpusDecoder *st, int request, opus_int32 par);

void OpusDecoderDestroyApi(OpusDecoder *st);

const char *OpusStrerrorApi(int error);

#if defined (__cplusplus)
}
#endif
