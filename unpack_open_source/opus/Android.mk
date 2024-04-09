LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libVmiOpus

LOCAL_LDLIBS := -lm -llog

LOCAL_SRC_FILES := \
    codes/celt/arm/armcpu.c \
    codes/celt/arm/arm_celt_map.c \
    codes/silk/fixed/LTP_analysis_filter_FIX.c \
    codes/silk/fixed/LTP_scale_ctrl_FIX.c \
    codes/silk/fixed/corrMatrix_FIX.c \
    codes/silk/fixed/encode_frame_FIX.c \
    codes/silk/fixed/find_LPC_FIX.c \
    codes/silk/fixed/find_LTP_FIX.c \
    codes/silk/fixed/find_pitch_lags_FIX.c \
    codes/silk/fixed/find_pred_coefs_FIX.c \
    codes/silk/fixed/noise_shape_analysis_FIX.c \
    codes/silk/fixed/process_gains_FIX.c \
    codes/silk/fixed/regularize_correlations_FIX.c \
    codes/silk/fixed/residual_energy16_FIX.c \
    codes/silk/fixed/residual_energy_FIX.c \
    codes/silk/fixed/warped_autocorrelation_FIX.c \
    codes/silk/fixed/apply_sine_window_FIX.c \
    codes/silk/fixed/autocorr_FIX.c \
    codes/silk/fixed/burg_modified_FIX.c \
    codes/silk/fixed/k2a_FIX.c \
    codes/silk/fixed/k2a_Q16_FIX.c \
    codes/silk/fixed/pitch_analysis_core_FIX.c \
    codes/silk/fixed/vector_ops_FIX.c \
    codes/silk/fixed/schur64_FIX.c \
    codes/silk/fixed/schur_FIX.c \
    codes/silk/arm/arm_silk_map.c \
    codes/silk/arm/biquad_alt_neon_intr.c \
    codes/silk/arm/LPC_inv_pred_gain_neon_intr.c \
    codes/silk/arm/NSQ_del_dec_neon_intr.c \
    codes/silk/arm/NSQ_neon.c \
    codes/src/opus.c \
    codes/src/opus_decoder.c \
    codes/src/opus_encoder.c \
    codes/src/opus_multistream.c \
    codes/src/opus_multistream_encoder.c \
    codes/src/opus_multistream_decoder.c \
    codes/src/repacketizer.c \
    codes/src/opus_projection_encoder.c \
    codes/src/opus_projection_decoder.c \
    codes/src/mapping_matrix.c \
    codes/silk/sigm_Q15.c \
    codes/silk/lin2log.c \
    codes/silk/process_NLSFs.c \
    codes/silk/quant_LTP_gains.c \
    codes/silk/bwexpander.c \
    codes/silk/LPC_analysis_filter.c \
    codes/silk/NLSF2A.c \
    codes/silk/interpolate.c \
    codes/silk/A2NLSF.c \
    codes/silk/NSQ.c \
    codes/silk/pitch_est_tables.c \
    codes/silk/sort.c \
    codes/silk/resampler_down2_3.c \
    codes/silk/resampler_down2.c \
    codes/celt/celt_lpc.c \
    codes/silk/tables_other.c \
    codes/silk/LPC_fit.c \
    codes/silk/bwexpander_32.c \
    codes/celt/entdec.c \
    codes/silk/dec_API.c \
    codes/celt/celt_decoder.c \
    codes/silk/LPC_inv_pred_gain.c \
    codes/silk/log2lin.c \
    codes/silk/sum_sqr_shift.c \
    codes/celt/entenc.c \
    codes/celt/mathops.c \
    codes/celt/celt_encoder.c \
    codes/silk/enc_API.c \
    codes/celt/pitch.c \
    codes/silk/tables_LTP.c \
    codes/silk/NLSF_encode.c \
    codes/celt/quant_bands.c \
    codes/celt/mdct.c \
    codes/celt/bands.c \
    codes/celt/celt.c \
    codes/silk/biquad_alt.c \
    codes/silk/encode_pulses.c \
    codes/silk/encode_indices.c \
    codes/silk/gain_quant.c \
    codes/silk/LP_variable_cutoff.c \
    codes/silk/VAD.c \
    codes/silk/init_encoder.c \
    codes/celt/vq.c \
    codes/celt/rate.c \
    codes/celt/entcode.c \
    codes/celt/modes.c \
    codes/silk/decode_pulses.c \
    codes/silk/decode_indices.c \
    codes/silk/resampler.c \
    codes/silk/stereo_MS_to_LR.c \
    codes/silk/decode_frame.c \
    codes/silk/stereo_decode_pred.c \
    codes/silk/decoder_set_fs.c \
    codes/silk/init_decoder.c \
    codes/silk/resampler_private_AR2.c \
    codes/silk/resampler_rom.c \
    codes/silk/table_LSF_cos.c \
    codes/silk/VQ_WMat_EC.c \
    codes/silk/NLSF_VQ_weights_laroia.c \
    codes/silk/NSQ_del_dec.c \
    codes/silk/tables_pulses_per_block.c \
    codes/celt/kiss_fft.c \
    codes/celt/laplace.c \
    codes/silk/NLSF_decode.c \
    codes/silk/control_SNR.c \
    codes/silk/stereo_LR_to_MS.c \
    codes/silk/HP_variable_cutoff.c \
    codes/silk/stereo_encode_pred.c \
    codes/silk/control_codec.c \
    codes/silk/check_control_input.c \
    codes/silk/decode_core.c \
    codes/silk/decode_parameters.c \
    codes/silk/resampler_private_IIR_FIR.c \
    codes/silk/resampler_private_down_FIR.c \
    codes/silk/resampler_private_up2_HQ.c \
    codes/silk/code_signs.c \
    codes/silk/shell_coder.c \
    codes/celt/cwrs.c \
    codes/silk/ana_filt_bank_1.c \
    codes/silk/tables_pitch_lag.c \
    codes/silk/tables_gain.c \
    codes/silk/NLSF_del_dec_quant.c \
    codes/silk/NLSF_unpack.c \
    codes/silk/NLSF_VQ.c \
    codes/silk/NLSF_stabilize.c \
    codes/silk/PLC.c \
    codes/silk/CNG.c \
    codes/silk/tables_NLSF_CB_WB.c \
    codes/silk/tables_NLSF_CB_NB_MB.c \
    codes/silk/stereo_find_predictor.c \
    codes/silk/stereo_quant_pred.c \
    codes/silk/control_audio_bandwidth.c \
    codes/silk/decode_pitch.c \
    codes/silk/inner_prod_aligned.c \
    OpusApi.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/codes/include \
    $(LOCAL_PATH)/codes/silk \
    $(LOCAL_PATH)/codes/silk/fixed \
    $(LOCAL_PATH)/codes/celt \
    $(LOCAL_PATH)/codes

LOCAL_CFLAGS := -DNULL=0 -DSOCKLEN_T=socklen_t -DLOCALE_NOT_USED -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DPACKAGE_VERSION=\"unknown\"
LOCAL_CFLAGS += -Drestrict='' -D__EMX__ -DOPUS_BUILD -DFIXED_POINT=1 -DDISABLE_FLOAT_API -DUSE_ALLOCA -DHAVE_LRINT -DHAVE_LRINTF -O3 -fno-math-errno
LOCAL_CPPFLAGS := -DBSD=1 -ffast-math -O3 -funroll-loops

include $(BUILD_SHARED_LIBRARY)