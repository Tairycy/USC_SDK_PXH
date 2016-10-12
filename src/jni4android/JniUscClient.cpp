//#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <netdb.h>
#endif
#include "log.h"
#include "set_option_key.h"
#include "asr_client.h"
#include "JniUscClient.h"
#include "errorNo.h"

#define JNIREG_CLASS "cn/yunzhisheng/asr/JniUscClient"

#define INPUT_BUFFER_LEN	(8000*2)	// 500ms
#define GET_RESULT_LEN	    (3000*2)	// 500ms
#define VAD_BUFFER_LEN		(4000*2)	// 250ms

class NativeControlBlock {
public:
	asr_client* asrService;
	jbyte input_buffer[INPUT_BUFFER_LEN];
	jbyte vad_buffer[VAD_BUFFER_LEN];
	int count;
	int disablePResult;

public:
	NativeControlBlock():asrService(NULL) {
	}
};

extern "C"
JNIEXPORT jlong JNICALL usc_create
  (JNIEnv *env, jobject, jstring juri, jint port)
{
	NativeControlBlock* self = new NativeControlBlock();
	if(self == NULL)
		return (jlong)self;
	
	const char *uri = env->GetStringUTFChars(juri, 0);
	
	self->disablePResult = 0;
	self->asrService = (asr_client*)asrCreateAsrService(uri, port);
  	if (self->asrService == NULL) {
  		delete self;
  		self = NULL;
  	}
	
	env->ReleaseStringUTFChars(juri, uri);
	
  	return (jlong)self;
}


extern "C"
JNIEXPORT jint JNICALL usc_setOptionInt
(JNIEnv *, jobject, jlong handle, jint id , jint value)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;

	if (self != NULL && self->asrService != NULL) {
		if(id == ASR_DISABLE_PARTIAL_RESULT_ID)
			self->disablePResult = value;
		else
			r = self->asrService->SetValueInt(id, value);
	}
	return r;
}

extern "C"
JNIEXPORT jint JNICALL usc_setOptionString
(JNIEnv *env, jobject, jlong handle, jint id , jstring value)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL && self->asrService != NULL) {
		const char* s = env->GetStringUTFChars(value,0);
		r = self->asrService->SetValueString(id, s);
		env->ReleaseStringUTFChars(value,s);
	}
	return r;
}

extern "C"
JNIEXPORT jint JNICALL usc_start
(JNIEnv *, jobject, jlong handle)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*)handle;

	if (self != NULL && self->asrService != NULL) {
		self->count = 0;
		r = self->asrService->Start();
	}
	return r;
}

extern "C"
JNIEXPORT jint JNICALL usc_login
(JNIEnv *, jobject, jlong handle)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*)handle;

	if (self != NULL && self->asrService != NULL) {
		self->count = 0;
		r = self->asrService->Login();
	}
	return r;
}

extern "C"
JNIEXPORT jint JNICALL usc_stop
(JNIEnv *, jobject, jlong handle)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL && self->asrService != NULL) {
		r = self->asrService->Stop();
	}
	return r;
}

extern "C"
JNIEXPORT jint JNICALL usc_recognize
(JNIEnv *env, jobject, jlong handle, jbyteArray raw, jint len)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL && self->asrService != NULL) {
		int offset = 0;
		r = SUCC_CODE::ASRCLIENT_RECOGNIZER_OK;
		do {
			int readLen = (len-offset) > INPUT_BUFFER_LEN ? INPUT_BUFFER_LEN : (len-offset);
			env->GetByteArrayRegion(raw, offset, readLen, self->input_buffer);
	  		
			r = self->asrService->Recognizer((char*)self->input_buffer, readLen);
			if (r<0) {
				return r;
			}
			
			offset += readLen;
		}while(offset < len);
		
		r = self->asrService->QueryResult();

	}
  	return r;
}

extern "C"
JNIEXPORT jstring JNICALL usc_getResult
(JNIEnv *env, jobject, jlong handle)
{
	const char *r = "asrService is NULL";
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL && self->asrService != NULL) {
		r = self->asrService->GetResult();
	}
	return env->NewStringUTF(r);
}


extern "C"
JNIEXPORT jstring JNICALL usc_getOptionValue
(JNIEnv *env, jobject, jlong handle, jint id)
{
	const char *r = (char *)"";
	NativeControlBlock* self = (NativeControlBlock*) handle;

	if (self != NULL && self->asrService != NULL) {
		r = self->asrService->GetOptionValue(id);
	}
	return env->NewStringUTF(r);
}


extern "C"
JNIEXPORT jint JNICALL usc_cancel
  (JNIEnv *, jobject, jlong handle)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;

	if (self != NULL && self->asrService != NULL) {
		r = self->asrService->Cancel();
	}
	return r;
}

extern "C"
JNIEXPORT void JNICALL usc_destroy
  (JNIEnv *, jobject, jlong handle)
{
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL) {
		asrDestroyAsrService(self->asrService);
		self->asrService = NULL;
	}
	
	delete 	(NativeControlBlock*) handle;
}

extern "C"
JNIEXPORT jint JNICALL usc_getLastErrno
(JNIEnv *, jobject, jlong handle)
{
	int r = NO_HANDLE_ERROR;
	NativeControlBlock* self = (NativeControlBlock*) handle;
	
	if (self != NULL && self->asrService != NULL) {
		self->count = 0;
		r = self->asrService->GetLastError();
	}
  	return r;
}

/**
 * Table of methods associated with a single class.
 */
static JNINativeMethod gMethods[] = {
	{ "createNative", "(Ljava/lang/String;I)J", (void*)usc_create },
	{ "setOptionInt", "(JII)I", (void*)usc_setOptionInt },
	{ "setOptionString", "(JILjava/lang/String;)I", (void*)usc_setOptionString },
	{ "start", "(J)I", (void*)usc_start },
	{ "login", "(J)I", (void*)usc_login },
//	{ "isactive", "(J[BI)I", (void*)usc_isactive },
	{ "recognize", "(J[BI)I", (void*)usc_recognize },
	{ "stop", "(J)I", (void*)usc_stop },
	{ "cancel", "(J)I", (void*)usc_cancel },
	{ "getResult", "(J)Ljava/lang/String;", (void*)usc_getResult },
	{ "getOptionValue", "(JI)Ljava/lang/String;", (void*)usc_getOptionValue },
	{ "destroyNative", "(J)V", (void*)usc_destroy },
	{ "getLastErrno", "(J)I", (void*)usc_getLastErrno },
};


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
								 JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	
	clazz = env->FindClass(className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}
	
	return JNI_TRUE;
}


/*
 * Register native methods for all classes we know about.
 */
static int usc_registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, JNIREG_CLASS, gMethods, sizeof(gMethods) / sizeof(gMethods[0])))
		return JNI_FALSE;

	return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGI("ASR-JNI", "JNI_OnLoad pl2.0.2");
	LOGI("ASR-JNI", "vad_20150115_41c85d");

	JNIEnv* env = NULL;
	jint result = -1;
	
	if (vm->GetEnv( (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("ASR-JNI", "vm->GetEnv");
		return -1;
	}
	
	if (env == NULL){
        LOGE("ASR-JNI", "env == NULL");
        return -1;
    }
		
	if (!usc_registerNatives(env)) {
        LOGE("ASR-JNI", "usc_registerNatives fail");
		return -1;
	}
	
	/* success -- return valid version number */
	result = JNI_VERSION_1_6;
    
	return result;
}
