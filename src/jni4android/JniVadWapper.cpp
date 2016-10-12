//
//  JniVadWapper.cpp
//  usc
//
//  Created by hejinlai on 12-11-15.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#include "JniVadWapper.h"
#include "string.h"
#include <stdlib.h>
#include "LibVad.h"
//#define ANDROID_LOG

#ifdef ANDROID_LOG
#include <android/log.h>
#endif


using namespace usc;

#define INPUT_BUFFER_LEN	(8000*2)	// 500ms
//#define VAD_BUFFER_LEN		(4000*2)	// 250ms
#define VAD_BUFFER_LEN		(160*sizeof(short))	// 10ms
#define PITCH_BUFFER_LEN	(32000*2)	// 500ms
#define SET_TIME_VAD_TYPE_ID	-1001

enum VadType {
	DefaultVadType = 0,
	FarFeildVadType
};

class NativeControlBlock {

public:
	usc::LibVAD vad;
	int volume;
	jbyte vad_buffer[PITCH_BUFFER_LEN];
public:
	NativeControlBlock(){

	};

	void init() {
		vad.init();
	}
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_cn_yunzhisheng_asr_VAD_create(JNIEnv *env, jobject obj)
{
	NativeControlBlock* self = new NativeControlBlock();
	
    return (jlong) self;
}


JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_setTime(JNIEnv *env, jobject obj, jlong handle, jint frontSil,jint backSil)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return;
	}
	self->vad.setVadSilTime(frontSil,backSil);
}


JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_init(JNIEnv *env, jobject obj, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return;
	}
	
	self->init();
}

JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_reset(JNIEnv *env, jobject obj, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return;
	}

	self->vad.reset();
}


JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_getSpeakStart(JNIEnv *env, jobject obj, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return 0;
	}

	return self->vad.getSpeakStart() * (16000/100*2);// frame to bytes 
}


JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_isVADTimeout(JNIEnv *env, jobject obj, jlong handle, jbyteArray raw, jint len)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return USC_VAD_HOLDING;
	}

    int status = USC_VAD_HOLDING;
    int offset = 0;
    do {
        int readLen = (len-offset) > VAD_BUFFER_LEN ? VAD_BUFFER_LEN : (len-offset);
        env->GetByteArrayRegion(raw, offset, readLen, self->vad_buffer);

        status = self->vad.process((short*)self->vad_buffer, readLen/(sizeof(short)),&self->volume);

        if(status != USC_VAD_HOLDING) {
            return status;
        }

        offset += readLen;
    } while(offset < len);

  	return status;
}

JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_getVolume(JNIEnv *env, jobject obj, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return 0;
	}

	return self->volume;
}

JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_checkPitchOffset(JNIEnv *env, jobject obj, jlong handle, jbyteArray raw, jint len)
{
	int r = 0;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return 0;
	}


		if(len > PITCH_BUFFER_LEN) {
			len = PITCH_BUFFER_LEN;
		}
		env->GetByteArrayRegion(raw, 0, len, self->vad_buffer);

		r = self->vad.checkPitchOffset((short*)self->vad_buffer, len>>1);

  	return r;
}

JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_destory(JNIEnv *env, jobject obj, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return;
	}

    delete self;
}

JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_nativeSetOption(JNIEnv *env, jobject obj, jlong handle, jint jkey, jstring jvalue)
{
	int r = -1;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if(self == NULL){
		return 0;
	}


	const char *value = env->GetStringUTFChars(jvalue, 0);
	int type = jkey/10000;
	switch(type) {
	case 1: // int
		{
#ifdef ANDROID_LOG
__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "nativeSetOption type=%d key =%d value=%s", type, jkey%10000, value);
#endif			
			int v = atoi(value);
			r = self->vad.setparas(jkey%10000, &v);
		}
		break;
	case 2: // float
		{
#ifdef ANDROID_LOG
__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "nativeSetOption type=%d key =%d value=%s", type, jkey%10000, value);
#endif
			float v = strtod(value, NULL);
			r = self->vad.setparas(jkey%10000, &v);
		}
		break;
	case 3: // double
		{
			// double v = atof(value);
			double v = strtod(value, NULL);
			r = self->vad.setparas(jkey%10000, &v);
		}
		break;
	default:
#ifdef ANDROID_LOG
	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "nativeSetOption type=%d key =%d value=%s", type, jkey%10000, value);
#endif
		break;
	}


	env->ReleaseStringUTFChars(jvalue, value);


  	return r;
}

#ifdef __cplusplus
}
#endif
