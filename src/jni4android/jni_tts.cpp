//#include <string.h>
#include <stdio.h>
#include <string.h>
#include "jni_tts.h"
#include "tts_client.h"
#include "set_option_key.h"
#include "log.h"
#include <string>

#define JNIREG_CLASS "cn/yunzhisheng/tts/JniUscClient"


#define TTSCLIENT_FATAL_ERROR -1

class NativeControlBlock
{
public:
	TTSHANDLE handle;

public:
	NativeControlBlock():handle(0)
	{
	}
};

extern "C"
JNIEXPORT jlong JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeCreate
  (JNIEnv *env, jobject, jstring jappKey)
{
	NativeControlBlock* self = new NativeControlBlock();
	if(self == NULL)
		return (jlong)self;
	const char *appkey = env->GetStringUTFChars(jappKey, 0);
	int ret = tts_create_service(&(self->handle));
  	if (&(self->handle) == NULL || ret < 0)
	{
  		delete self;
  		self = NULL;
  	}
	tts_set_option(self->handle, "appkey", appkey);
	env->ReleaseStringUTFChars(jappKey, appkey);
  	return (jlong)self;
}

extern "C"
JNIEXPORT jlong JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeCreateExt
  (JNIEnv *env, jobject, jstring jappKey, jstring juri, jstring jport)
{
	NativeControlBlock* self = new NativeControlBlock();
	if(self == NULL)
		return (jlong)self;
	const char *uri = env->GetStringUTFChars(juri, 0);
	const char *port = env->GetStringUTFChars(jport, 0);
	const char *appkey = env->GetStringUTFChars(jappKey, 0);
	
	int ret = tts_create_service_ext(&(self->handle), uri, port);
  	if (&(self->handle) == NULL || ret < 0)
	{
  		delete self;
  		self = NULL;
  	}
	tts_set_option(self->handle, "appkey", appkey);
	env->ReleaseStringUTFChars(juri, uri);
	env->ReleaseStringUTFChars(jport, port);
	env->ReleaseStringUTFChars(jappKey, appkey);
	
  	return (jlong)self;
}

extern "C"
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeRelease
(JNIEnv *, jobject, jlong handle)
{
	int ret = TTSCLIENT_FATAL_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if (self != NULL)
	{
		ret = tts_release_service(self->handle);
	}	
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeSetOption
(JNIEnv *env, jobject, jlong handle, jint jkey , jstring jvalue)
{
	int ret = TTSCLIENT_FATAL_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	int key = jkey;
	//const char *key = env->GetStringUTFChars(jkey, 0);
	const char *value = env->GetStringUTFChars(jvalue, 0);
	std::string param_str(value);
	if (self != NULL)
	{
		if (key != USC_OPT_ENGINE_PARAMETER)
			ret = tts_set_option_id(self->handle, key, value);
		else
		{
			int begin_flag = 0, end_flag;
			while ((end_flag = param_str.find("=", begin_flag)) > 0)
			{
				std::string header_str = param_str.substr(begin_flag, end_flag - begin_flag);
				begin_flag = end_flag + 1;
				if ((end_flag = param_str.find(";", begin_flag)) < 0)
					break;
				std::string value_str = param_str.substr(begin_flag, end_flag - begin_flag);
				begin_flag = end_flag + 1;
				
				tts_set_option(self->handle, header_str.c_str(), value_str.c_str());
			}
		}
	}
	//env->ReleaseStringUTFChars(jkey, key);
	env->ReleaseStringUTFChars(jvalue, value);
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeStart
(JNIEnv *env, jobject, jlong handle, jstring jformat, jstring jcodec)
{
	int ret = TTSCLIENT_FATAL_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;	
	const char *format = env->GetStringUTFChars(jformat, 0);
	const char *codec = env->GetStringUTFChars(jcodec, 0);	
	if (self != NULL)
	{
		ret = tts_start_synthesis(self->handle);
		tts_set_option(self->handle, "audioFormat", format);
		tts_set_option(self->handle, "audioCodec", codec);
	}
	env->ReleaseStringUTFChars(jformat, format);
	env->ReleaseStringUTFChars(jcodec, codec);	

  	return ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeStop
(JNIEnv *env, jobject, jlong handle)
{
	int ret = TTSCLIENT_FATAL_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	if (self != NULL)
	{
		ret = tts_stop_synthesis(self->handle);
	}	
	return ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeTextPut
(JNIEnv* env, jobject, jlong handle, jstring jtextdata)
{
	int ret = TTSCLIENT_FATAL_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	const char * textdata = env->GetStringUTFChars(jtextdata, 0);
	if (self != NULL)
	{
		ret = tts_text_put(self->handle, textdata, strlen(textdata));
	}
	env->ReleaseStringUTFChars(jtextdata, textdata);
	return ret;
}

extern "C"
JNIEXPORT jbyteArray JNICALL Java_cn_yunzhisheng_tts_JniClient_nativeGetResult
(JNIEnv *env, jobject, jlong handle, jintArray jaudioLen, jintArray jsynthStatus, jintArray jerrorCode)
{
	const char* ret = NULL;
	NativeControlBlock* self = (NativeControlBlock*) handle;	
	jint * audioLen = env->GetIntArrayElements(jaudioLen, 0);
	jint * synthStatus = env->GetIntArrayElements(jsynthStatus, 0);
	jint * errorCode = env->GetIntArrayElements(jerrorCode, 0);		
	if (self != NULL)
	{
		ret = (const char*)tts_get_result(self->handle, (unsigned int*)audioLen, (int*)synthStatus, (int*)errorCode);
		env->SetIntArrayRegion(jaudioLen, 0, 1, (jint *)audioLen);
		env->SetIntArrayRegion(jsynthStatus, 0, 1, (jint *)synthStatus);
		env->SetIntArrayRegion(jerrorCode, 0, 1, (jint *)errorCode);
	}
	jbyteArray buffer = NULL;
	int len = audioLen[0];
	if(len < 1204*1000 && len > 10) {
		buffer = env->NewByteArray(len); 
		env->SetByteArrayRegion(buffer, 0, len, (jbyte*)ret);	
	}
	env->ReleaseIntArrayElements(jaudioLen, audioLen, 0);
	env->ReleaseIntArrayElements(jsynthStatus, synthStatus, 0);
	env->ReleaseIntArrayElements(jerrorCode, errorCode, 0);
	
	
    return buffer; 
}
