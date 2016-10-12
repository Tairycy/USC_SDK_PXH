//
//  JniVadWapper.h
//  usc
//
//  Created by hejinlai on 12-11-15.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//
#include <jni.h>

#ifndef __usc__JniVadWapper__
#define __usc__JniVadWapper__

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jlong JNICALL Java_cn_yunzhisheng_asr_VAD_create(JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_checkPitchOffset(JNIEnv *, jobject, jlong, jbyteArray, jint);
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_isVADTimeout(JNIEnv *, jobject, jlong, jbyteArray, jint);
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_asr_VAD_nativeSetOption(JNIEnv *, jobject, jlong, jint, jstring);
JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_init(JNIEnv *, jobject, jlong);
JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_reset(JNIEnv *, jobject, jlong);
JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_setTime(JNIEnv *, jobject, jlong, jint,jint);
JNIEXPORT void JNICALL Java_cn_yunzhisheng_asr_VAD_destory(JNIEnv *, jobject, jlong);



#ifdef __cplusplus
}
#endif
#endif
