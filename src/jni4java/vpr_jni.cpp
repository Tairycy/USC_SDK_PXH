#include "vpr_jni.h"
#include "log.h"
#include "libusc_vpr.h"
#include <jni.h>


JNIEXPORT jlong JNICALL Java_cn_unisound_UniVprJni_create__(JNIEnv *env, jobject)
{
	VPRHANDLE Handle;
	int ret = usc_vpr_create_service(&Handle);
	if (ret == 0)
		return Handle;
	else
		return -1;
}

JNIEXPORT jlong JNICALL Java_cn_unisound_UniVprJni_create__Ljava_lang_String_2I
  (JNIEnv *env, jobject, jstring juri, jint port)
{
	VPRHANDLE Handle;
	const char *uri = env->GetStringUTFChars(juri, 0);
	int ret = usc_vpr_create_service_ext(&Handle, uri, (unsigned short)port);
	env->ReleaseStringUTFChars(juri, uri);
	if (ret == 0)
		return Handle;
	else
		return -1;
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_setOptionInt
  (JNIEnv *env, jobject, jlong handle, jint jid, jint jvalue)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_setOptionString
  (JNIEnv *env, jobject, jlong handle, jint jid, jstring jvalue)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
  	const char* value = env->GetStringUTFChars(jvalue, 0);
	int ret = usc_vpr_set_option(Handle, jid, value);
	env->ReleaseStringUTFChars(jvalue, value);
	return ret;
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_login
  (JNIEnv *env, jobject, jlong handle)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
	return usc_vpr_login_service(Handle);
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_start
  (JNIEnv *env, jobject, jlong handle)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
	return usc_vpr_start_recognizer(Handle);
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_stop
  (JNIEnv *env, jobject, jlong handle)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
	return usc_vpr_stop_recognizer(Handle);
}

JNIEXPORT jstring JNICALL Java_cn_unisound_UniVprJni_getResult
  (JNIEnv *env, jobject, jlong handle)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
	const char* result = usc_vpr_get_result(handle);
	if(result != NULL)
	{
		return env->NewStringUTF(result);
	}
	return env->NewStringUTF("");
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_release
  (JNIEnv *env, jobject, jlong handle)
{
  	VPRHANDLE Handle = (VPRHANDLE)handle;
	usc_vpr_release_service(Handle);
	return 0;
}

JNIEXPORT jint JNICALL Java_cn_unisound_UniVprJni_feedBuffer
  (JNIEnv *env, jobject, jlong handle, jbyteArray pcmData, jint pcmLen)
{
	VPRHANDLE Handle = (VPRHANDLE)handle;
	int ret;
	jbyte* buffer_ptr = env->GetByteArrayElements(pcmData, JNI_FALSE);
	ret = usc_vpr_feed_buffer(handle, (const char*)buffer_ptr, (int)pcmLen);
	env->ReleaseByteArrayElements(pcmData, buffer_ptr, 0);
	return ret;
}
