
SRC_LIB=../../libs
SRC_SDK=../../sdk

DECODEWRAP_PATH=$(SRC_LIB)/decodeWrap
DECODEASR_PATH=$(SRC_LIB)/decodeAsr
DECODE_PATH=$(SRC_LIB)/decode
TOKENWRAP_PATH = $(SRC_LIB)/tokenWrap
VAD_PATH = $(SRC_LIB)/libvad
OPUS_PATH = $(SRC_LIB)/libopus

TTSSDK_PATH = $(SRC_SDK)/tts
VPRSDK_PATH = $(SRC_SDK)/vpr
ASRSDK_PATH = $(SRC_SDK)/asr
JHTTP_PATH = $(SRC_SDK)/JHttpClient
REQUEST_PATH = $(SRC_SDK)/request
CRYPT_PATH = $(SRC_SDK)/crypt
FUNCTION_PATH = $(SRC_SDK)/function
DEFINE_PATH = $(SRC_SDK)/define
PCLIB_PATH = $(SRC_SDK)/pclib


SRC_FILES = \
	$(DECODEWRAP_PATH)/src/cache.cpp \
	$(DECODE_PATH)/Decode.cpp \
	$(DECODE_PATH)/DecodeUscOpus.cpp \
	$(DECODE_PATH)/DecodeWave.cpp \
	$(TOKENWRAP_PATH)/cBase64.cpp \
	$(TOKENWRAP_PATH)/tokenTools.cpp \
	$(TOKENWRAP_PATH)/tokenWrap.cpp \
	$(TOKENWRAP_PATH)/genToken.cpp \
	$(TOKENWRAP_PATH)/getTime.cpp \
	$(TOKENWRAP_PATH)/AES.cpp \
	$(VAD_PATH)/LibVad.cpp \
	$(TTSSDK_PATH)/tts_client.cpp \
	$(TTSSDK_PATH)/tts_service.cpp \
	$(VPRSDK_PATH)/libusc_vpr.cpp \
	$(ASRSDK_PATH)/asr_client.cpp \
	$(ASRSDK_PATH)/asr_service.cpp \
	$(JHTTP_PATH)/utils.cpp \
	$(JHTTP_PATH)/JHttpClient.cpp \
	$(REQUEST_PATH)/sdk_req.cpp \
	$(REQUEST_PATH)/service.cpp \
	$(CRYPT_PATH)/crypt.cpp \
	$(CRYPT_PATH)/md5.cpp \
	$(CRYPT_PATH)/sdk_crypt.cpp \
	$(FUNCTION_PATH)/function.cpp \
	$(FUNCTION_PATH)/url_code.cpp \
	$(DEFINE_PATH)/log.cpp \
	$(PCLIB_PATH)/function_pc.cpp \
	$(PCLIB_PATH)/AsrServiceControlBlock.cpp \
	$(PCLIB_PATH)/BufferQueue.cpp \
	$(PCLIB_PATH)/tts_sdk.cpp


OPUS_WRAPPER = $(DECODEASR_PATH)/Opuswrapper.cpp	
OPUS_TTS_WRAPPER = $(DECODEWRAP_PATH)/src/uni_opus_decoder.cpp	


INCLUDE_SDK = \
	-I$(TTSSDK_PATH)/ \
	-I$(VPRSDK_PATH) \
	-I$(ASRSDK_PATH) \
	-I$(JHTTP_PATH)/ \
	-I$(REQUEST_PATH)/ \
	-I$(CRYPT_PATH) \
	-I$(FUNCTION_PATH) \
	-I$(DEFINE_PATH) \
	-I$(PCLIB_PATH)
	

INCLUDE_LIB = -Wall\
	-I$(DECODEWRAP_PATH)/inc \
	-I$(DECODEASR_PATH) \
	-I$(DECODE_PATH) \
	-I$(TOKENWRAP_PATH) \
	-I$(VAD_PATH) \
	-DFLOATING_POINT_SPEEX \
	-DUSE_KISS_FFT_SPEEX \
	-DTEST_SERVIER_USAGE -fPIC 

	#-DUSE_VAD \

OPUS_CFLAGS = -DUSE_ALLOCA  \
	-I$(OPUS_PATH)/silk \
	-I$(OPUS_PATH)/include \
	-I$(OPUS_PATH)/silk/float \
	-I$(OPUS_PATH)/celt \
	-I$(OPUS_PATH)/pc \
	-I$(SRC)/libclient/inc \
	-D_ENCODE_ONLY_ \
	-DUSE_KISS_FFT \
	-DLINUX -DCOMMON_VAD -DMINTEGER32B \
	-DTESTATTR -DCOMMON_TAILER_CHECK \

	#-DEXPORT="" \
	
OPUS_LIBS = $(OPUS_PATH)/libopus.a
VAD_LIB = $(VAD_PATH)/car/cent64/asrvad.a

#ifdef JNI
JNI_PATH=../../jni4java
#SRC_FILES += $(JNI_PATH)/vpr_jni.cpp
SRC_FILES += $(PCLIB_PATH)/libusc.cpp $(PCLIB_PATH)/tts_sdk.cpp $(VPRSDK_PATH)/libusc_vpr.cpp 
CFLAGS += -I$(JNI_PATH) \
	  -I$(JAVA_HOME)/include \
	  -I$(JAVA_HOME)/include/linux
#endif

LIB_FILES = $(OPUS_LIBS) $(VAD_LIB)
SRC_FILES += $(OPUS_WRAPPER) $(OPUS_TTS_WRAPPER)
CFLAGS += 	$(INCLUDE_LIB) \
		$(INCLUDE_SDK) \
	        $(OPUS_CFLAGS) \
	        -UHAVE_CONFIG_H \
		-DNO_USE_AMR_SPEEX 

CXXFLAGS = $(CFLAGS)

ifdef DEBUG
CXXFLAGS += -g
endif

AR = ar
RM = rm
CP = cp
ECHO = echo

include	tool.mak
TARGET = libusc.so
USC_LIB=libusc_static.a
USC_JNI_O=libusc_jni.o

OBJ_FILES = $(SRC_FILES:%.cpp=%.o)
OBJ_FILE = $(SRC_FILES:%.c=%.o)


ALL:    $(TARGET)

$(USC_LIB): $(OBJ_FILES) $(OBJ_FILE) $(LIB_FILES)
	$(BUILD_LIBRARY)

$(USC_JNI_O):  $(JNI_PATH)/vpr_jni.cpp
	g++ -Wall -fPIC -shared $^ -c -o $@ $(CXXFLAGS)

$(TARGET):  $(USC_JNI_O)  $(USC_LIB) $(LIB_FILES)
	g++ -Wall -fPIC -shared -lrt $^ -o $@ $(CXXFLAGS) -L. -lusc_static 
	
#$(TARGET): $(PCLIB_PATH)/libusc.cpp $(PCLIB_PATH)/tts_sdk.cpp $(VPRSDK_PATH)/libusc_vpr.cpp $(USC_LIB) $(LIB_FILES)
#	g++ -fPIC -shared -lrt $^ -o $@  $(CXXFLAGS) -L.  -lusc_static 

clean:
	rm -f $(TARGET)
	rm -f $(USC_LIB)
	rm -f $(DECODEASR_PATH)/*.o
	rm -f $(DECODEWRAP_PATH)/src/*.o
	rm -f $(DECODE_PATH)/*.o
	rm -f $(TOKENWRAP_PATH)/*.o
	rm -f $(VAD_PATH)/*.o
	rm -f $(OPUS_PATH)/*.o
	rm -f $(TTSSDK_PATH)/*.o
	rm -f $(VPRSDK_PATH)/*.o
	rm -f $(ASRSDK_PATH)/*.o
	rm -f $(JHTTP_PATH)/*.o
	rm -f $(REQUEST_PATH)/*.o
	rm -f $(CRYPT_PATH)/*.o
	rm -f $(FUNCTION_PATH)/*.o
	rm -f $(DEFINE_PATH)/*.o
	rm -f $(PCLIB_PATH)/*.o
#ifdef JNI
	rm -f $(JNI_PATH)/*.o
	rm -f *.o
#endif

