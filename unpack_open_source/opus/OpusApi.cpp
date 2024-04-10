
#include "OpusApi.h"

OpusEncoder *OpusEncoderCreateApi(opus_int32 Fs, int channels, int application, int *error) {
	return opus_encoder_create(Fs, channels, application, error);
}

opus_int32 OpusEncodeApi(OpusEncoder *st, const opus_int16 *pcm, int frame_size, unsigned char *data,
    opus_int32 max_data_bytes) {
	return opus_encode(st, pcm, frame_size, data, max_data_bytes);
}

int OpusEncoderCtlApi(OpusEncoder *st, int request, opus_int32 par) {
	return opus_encoder_ctl(st, request, par);
}

void OpusEncoderDestroyApi(OpusEncoder *st) {
	return opus_encoder_destroy(st);
}

OpusDecoder *OpusDecoderCreateApi(opus_int32 Fs, int channels, int *error) {
	return opus_decoder_create(Fs, channels, error);
}

int OpusDecodeApi(OpusDecoder *st, const unsigned char *data, opus_int32 len, opus_int16 *pcm, int frame_size,
    int decode_fec) {
	return opus_decode(st, data, len, pcm, frame_size, decode_fec);
}

int OpusDecoderCtlApi(OpusDecoder *st, int request, opus_int32 par) {
	return opus_decoder_ctl(st, request, par);
}

void OpusDecoderDestroyApi(OpusDecoder *st) {
	return opus_decoder_destroy(st);
}

const char *OpusStrerrorApi(int error) {
	return opus_strerror(error);
}