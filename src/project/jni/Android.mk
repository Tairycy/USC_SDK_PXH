LOCAL_PATH := $(call my-dir)
API_PATH = $(LOCAL_PATH)/../..
SDK_PATH = $(API_PATH)/sdk
LIB_PATH = $(API_PATH)/libs
JNI_PATH = $(API_PATH)/jni4android

VAD_PATH = $(LIB_PATH)/libvad
OPUS_PATH = $(LIB_PATH)/libopus
TOKENWRAP_PATH = $(LIB_PATH)/tokenWrap
DECODEWRAP_PATH = $(LIB_PATH)/decodeWrap
DECODEASR_PATH = $(LIB_PATH)/decodeAsr

TTSSDK_PATH = $(SDK_PATH)/tts
ASRSDK_PATH = $(SDK_PATH)/asr
REQUEST_PATH = $(SDK_PATH)/request
JHTTP_PATH = $(SDK_PATH)/JHttpClient
CRYPT_PATH = $(SDK_PATH)/crypt
FUNCTION_PATH = $(SDK_PATH)/function
DEFINE_PATH = $(SDK_PATH)/define

include $(CLEAR_VARS)
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_CPP_EXTENSION := .cpp
LOCAL_MODULE    := uscasr

OPUS_CFLAGS = -DHAVE_CONFIG_H -DUSE_ALLOCA -Os -fvisibility=hidden  -externs  

LOCAL_CFLAGS = -DANDROID_ENV -DFIXED_POINT -DUSE_KISS_FFT -DEXPORT="" -UHAVE_CONFIG_H -DLINUX -DCOMMON_VAD -DLIBLOG $(OPUS_CFLAGS) -D_ENCODE_ONLY_ -D_ANDROID_BUILD_


VAD_C_INCLUDES = $(VAD_PATH) $(LIB_PATH)/vad $(LIB_PATH)/uvad

LOCAL_C_INCLUDES := $(VAD_C_INCLUDES) \
					$(JNI_PATH) \
					$(LIB_PATH) \
					$(SDK_PATH) \
					$(API_PATH) \
					$(OPUS_PATH)/silk \
					$(OPUS_PATH)/include \
					$(OPUS_PATH)/silk/fixed \
					$(OPUS_PATH)/celt \
					$(OPUS_PATH) \
					$(TOKENWRAP_PATH) \
					$(ASRSDK_PATH) \
					$(TTSSDK_PATH) \
					$(DECODEWRAP_PATH)/inc \
					$(DECODEASR_PATH) \
					$(REQUEST_PATH) \
					$(JHTTP_PATH) \
					$(FUNCTION_PATH) \
					$(CRYPT_PATH) \
					$(DEFINE_PATH) \


CELT_SOURCES = \
$(OPUS_PATH)/celt/bands.c \
$(OPUS_PATH)/celt/celt.c \
$(OPUS_PATH)/celt/cwrs.c \
$(OPUS_PATH)/celt/entcode.c \
$(OPUS_PATH)/celt/entenc.c \
$(OPUS_PATH)/celt/kiss_fft.c \
$(OPUS_PATH)/celt/laplace.c \
$(OPUS_PATH)/celt/mathops.c \
$(OPUS_PATH)/celt/mdct.c \
$(OPUS_PATH)/celt/modes.c \
$(OPUS_PATH)/celt/pitch.c \
$(OPUS_PATH)/celt/celt_lpc.c \
$(OPUS_PATH)/celt/quant_bands.c \
$(OPUS_PATH)/celt/rate.c \
$(OPUS_PATH)/celt/vq.c \
$(OPUS_PATH)/celt/entdec.c \
$(OPUS_PATH)/celt/celt_decoder.c \
$(OPUS_PATH)/celt/celt_encoder.c \

SILK_SOURCES_FIXED = \
$(OPUS_PATH)/silk/fixed/LTP_analysis_filter_FIX.c \
$(OPUS_PATH)/silk/fixed/LTP_scale_ctrl_FIX.c \
$(OPUS_PATH)/silk/fixed/corrMatrix_FIX.c \
$(OPUS_PATH)/silk/fixed/encode_frame_FIX.c \
$(OPUS_PATH)/silk/fixed/find_LPC_FIX.c \
$(OPUS_PATH)/silk/fixed/find_LTP_FIX.c \
$(OPUS_PATH)/silk/fixed/find_pitch_lags_FIX.c \
$(OPUS_PATH)/silk/fixed/find_pred_coefs_FIX.c \
$(OPUS_PATH)/silk/fixed/noise_shape_analysis_FIX.c \
$(OPUS_PATH)/silk/fixed/prefilter_FIX.c \
$(OPUS_PATH)/silk/fixed/process_gains_FIX.c \
$(OPUS_PATH)/silk/fixed/regularize_correlations_FIX.c \
$(OPUS_PATH)/silk/fixed/residual_energy16_FIX.c \
$(OPUS_PATH)/silk/fixed/residual_energy_FIX.c \
$(OPUS_PATH)/silk/fixed/solve_LS_FIX.c \
$(OPUS_PATH)/silk/fixed/warped_autocorrelation_FIX.c \
$(OPUS_PATH)/silk/fixed/apply_sine_window_FIX.c \
$(OPUS_PATH)/silk/fixed/autocorr_FIX.c \
$(OPUS_PATH)/silk/fixed/burg_modified_FIX.c \
$(OPUS_PATH)/silk/fixed/k2a_FIX.c \
$(OPUS_PATH)/silk/fixed/k2a_Q16_FIX.c \
$(OPUS_PATH)/silk/fixed/pitch_analysis_core_FIX.c \
$(OPUS_PATH)/silk/fixed/vector_ops_FIX.c \
$(OPUS_PATH)/silk/fixed/schur64_FIX.c \
$(OPUS_PATH)/silk/fixed/schur_FIX.c

SILK_SOURCES = \
$(OPUS_PATH)/silk/CNG.c \
$(OPUS_PATH)/silk/code_signs.c \
$(OPUS_PATH)/silk/dec_API.c \
$(OPUS_PATH)/silk/decode_core.c \
$(OPUS_PATH)/silk/decode_frame.c \
$(OPUS_PATH)/silk/decode_indices.c \
$(OPUS_PATH)/silk/decode_parameters.c \
$(OPUS_PATH)/silk/decode_pitch.c \
$(OPUS_PATH)/silk/decode_pulses.c \
$(OPUS_PATH)/silk/decoder_set_fs.c \
$(OPUS_PATH)/silk/enc_API.c \
$(OPUS_PATH)/silk/encode_indices.c \
$(OPUS_PATH)/silk/encode_pulses.c \
$(OPUS_PATH)/silk/gain_quant.c \
$(OPUS_PATH)/silk/init_decoder.c \
$(OPUS_PATH)/silk/interpolate.c \
$(OPUS_PATH)/silk/LP_variable_cutoff.c \
$(OPUS_PATH)/silk/NLSF_decode.c \
$(OPUS_PATH)/silk/NSQ.c \
$(OPUS_PATH)/silk/NSQ_del_dec.c \
$(OPUS_PATH)/silk/PLC.c \
$(OPUS_PATH)/silk/shell_coder.c \
$(OPUS_PATH)/silk/tables_gain.c \
$(OPUS_PATH)/silk/tables_LTP.c \
$(OPUS_PATH)/silk/tables_NLSF_CB_NB_MB.c \
$(OPUS_PATH)/silk/tables_NLSF_CB_WB.c \
$(OPUS_PATH)/silk/tables_other.c \
$(OPUS_PATH)/silk/tables_pitch_lag.c \
$(OPUS_PATH)/silk/tables_pulses_per_block.c \
$(OPUS_PATH)/silk/VAD.c \
$(OPUS_PATH)/silk/control_audio_bandwidth.c \
$(OPUS_PATH)/silk/quant_LTP_gains.c \
$(OPUS_PATH)/silk/VQ_WMat_EC.c \
$(OPUS_PATH)/silk/HP_variable_cutoff.c \
$(OPUS_PATH)/silk/NLSF_encode.c \
$(OPUS_PATH)/silk/NLSF_VQ.c \
$(OPUS_PATH)/silk/NLSF_unpack.c \
$(OPUS_PATH)/silk/NLSF_del_dec_quant.c \
$(OPUS_PATH)/silk/process_NLSFs.c \
$(OPUS_PATH)/silk/check_control_input.c \
$(OPUS_PATH)/silk/control_SNR.c \
$(OPUS_PATH)/silk/init_encoder.c \
$(OPUS_PATH)/silk/control_codec.c \
$(OPUS_PATH)/silk/A2NLSF.c \
$(OPUS_PATH)/silk/ana_filt_bank_1.c \
$(OPUS_PATH)/silk/biquad_alt.c \
$(OPUS_PATH)/silk/bwexpander_32.c \
$(OPUS_PATH)/silk/bwexpander.c \
$(OPUS_PATH)/silk/debug.c \
$(OPUS_PATH)/silk/inner_prod_aligned.c \
$(OPUS_PATH)/silk/lin2log.c \
$(OPUS_PATH)/silk/log2lin.c \
$(OPUS_PATH)/silk/LPC_analysis_filter.c \
$(OPUS_PATH)/silk/LPC_inv_pred_gain.c \
$(OPUS_PATH)/silk/table_LSF_cos.c \
$(OPUS_PATH)/silk/NLSF2A.c \
$(OPUS_PATH)/silk/NLSF_stabilize.c \
$(OPUS_PATH)/silk/NLSF_VQ_weights_laroia.c \
$(OPUS_PATH)/silk/pitch_est_tables.c \
$(OPUS_PATH)/silk/resampler.c \
$(OPUS_PATH)/silk/resampler_down2_3.c \
$(OPUS_PATH)/silk/resampler_down2.c \
$(OPUS_PATH)/silk/resampler_private_AR2.c \
$(OPUS_PATH)/silk/resampler_private_down_FIR.c \
$(OPUS_PATH)/silk/resampler_private_IIR_FIR.c \
$(OPUS_PATH)/silk/resampler_private_up2_HQ.c \
$(OPUS_PATH)/silk/resampler_rom.c \
$(OPUS_PATH)/silk/sigm_Q15.c \
$(OPUS_PATH)/silk/sort.c \
$(OPUS_PATH)/silk/sum_sqr_shift.c \
$(OPUS_PATH)/silk/stereo_encode_pred.c \
$(OPUS_PATH)/silk/stereo_find_predictor.c \
$(SILK_SOURCES_FIXED) \
$(OPUS_PATH)/silk/stereo_quant_pred.c \
$(OPUS_PATH)/silk/stereo_LR_to_MS.c \
$(OPUS_PATH)/silk/stereo_MS_to_LR.c \
$(OPUS_PATH)/silk/stereo_decode_pred.c \


OPUS_SOURCES = \
$(OPUS_PATH)/src/opus.c \
$(OPUS_PATH)/src/opus_decoder.c \
$(OPUS_PATH)/src/opus_encoder.c \
$(OPUS_PATH)/src/opus_multistream.c \
$(OPUS_PATH)/src/repacketizer.c \
$(OPUS_PATH)/src/analysis.c \
$(OPUS_PATH)/src/mlp.c \
$(OPUS_PATH)/src/mlp_data.c 

SRC_SOURCES = \
$(REQUEST_PATH)/sdk_req.cpp \
$(REQUEST_PATH)/service.cpp

TOKENWRAP_SOURCES = \
$(TOKENWRAP_PATH)/AES.cpp \
$(TOKENWRAP_PATH)/cBase64.cpp \
$(TOKENWRAP_PATH)/checkTokenValid.cpp \
$(TOKENWRAP_PATH)/genToken.cpp \
$(TOKENWRAP_PATH)/getTime.cpp \
$(TOKENWRAP_PATH)/tokenTools.cpp \
$(TOKENWRAP_PATH)/tokenWrap.cpp

TTS_SOURCES = \
$(TTSSDK_PATH)/tts_client.cpp \
$(TTSSDK_PATH)/tts_service.cpp 

JHTTP_SOURCES = \
$(JHTTP_PATH)/JHttpClient.cpp \
$(JHTTP_PATH)/utils.cpp

FUNCTION_SOURCES = \
$(FUNCTION_PATH)/function.cpp \
$(FUNCTION_PATH)/url_code.cpp

CRYPT_SOURCES = \
$(CRYPT_PATH)/crypt.cpp \
$(CRYPT_PATH)/md5.cpp \
$(CRYPT_PATH)/sdk_crypt.cpp

ASRSDK_SOURCES = \
$(ASRSDK_PATH)/asr_client.cpp \
$(ASRSDK_PATH)/asr_service.cpp


libopus_la_SOURCES = $(CELT_SOURCES) $(SILK_SOURCES) $(OPUS_SOURCES) $(TOKENWRAP_SOURCES) $(TTS_SOURCES) $(SRC_SOURCES) $(JHTTP_SOURCES) $(FUNCTION_SOURCES) $(CRYPT_SOURCES) $(ASRSDK_SOURCES)

LOCAL_SRC_FILES :=  \
$(JNI_PATH)/JniUscClient.cpp \
$(JNI_PATH)/JniVadWapper.cpp \
$(JNI_PATH)/EncodeContent.cpp \
$(VAD_PATH)/LibVad.cpp \
$(libopus_la_SOURCES) \
$(LIB_PATH)/uvad/decision.c  \
$(JNI_PATH)/jni_tts.cpp \
$(DECODEASR_PATH)/Opuswrapper.cpp \
$(DECODEWRAP_PATH)/src/cache.cpp \
$(DECODEWRAP_PATH)/src/uni_opus_decoder.cpp \
$(DEFINE_PATH)/log.cpp

LOCAL_LDFLAGS := $(VAD_PATH)/car/android_r10/$(TARGET_ARCH_ABI)/libAsrVad.a

LOCAL_STATIC_LIBRARIES := AsrVad

include $(BUILD_SHARED_LIBRARY)

