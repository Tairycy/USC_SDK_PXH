#include <jni.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
#include "EncodeContent.h"
#include "Encrypter.h"
//#include "check_usrinfo.h"

//#define JNIREG_CLASS "cn/yunzhisheng/casr/EncodeContent"

/*JNIEXPORT jint JNICALL usc_CheckKeyValid
    (JNIEnv * env, jobject, jbyteArray key)
{
	char *chKey = (char*)env->GetByteArrayElements(key,0);
	int keyLen=env->GetArrayLength(key);
	int status=CheckKeyValid(chKey,keyLen);
	env->ReleaseByteArrayElements(key, (jbyte*)chKey, 0);
	return status;
}*/

//#define END_DATA_FIELD_LEN 4

/*
 * Class:     EncodeContent
 * Method:    DecodeContent
 * Signature: ([B[B[B)I
 */
//JNIEXPORT jint JNICALL usc_EncodeContent
  //(JNIEnv * env, jobject, jbyteArray key, jbyteArray targetData, jbyteArray encodedData)
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_casr_EncodeContent_EncodeTotalContent(JNIEnv *env, jobject obj, jbyteArray key, jbyteArray targetData, jbyteArray encodedData)
{
	
	//printf("Enter usc_EncodeContent\n");
	int keyLen=env->GetArrayLength(key);
	int targetDTLen=env->GetArrayLength(targetData);
	int encodeDTLen=env->GetArrayLength(encodedData);
	char *chKey = (char*)env->GetByteArrayElements(key,0);
	char *chTarget = (char*)env->GetByteArrayElements(targetData,0);
	//char *chEncodedData = (char*)env->GetByteArrayElements(encodedData,0);

	char* chEncodedData = (char*)malloc(encodeDTLen);
	memset(chEncodedData, 0x00, encodeDTLen);

	int status=Encrypter::EncodeContent(chKey, keyLen, chTarget, targetDTLen, chEncodedData, encodeDTLen);

	if(status==-1)
		return status;

	//printf("target is %s\n", chTarget);
        //printf("encode data is ", chEncodedData);
	/*int tmpLen=0;
	while(tmpLen<encodeDTLen)
	{
		printf("%02x ", (char)chEncodedData[tmpLen]);
		tmpLen++;
	}*/
	//printf("\n");
	//char *chEncode = (char*)malloc(encodeDTLen);
	//memset(chEncode, 0x00, encodeDTLen);
	//(char*)env->GetByteArrayElements(encodedData,0);
	
	//If key or target data is NULL, we return -1
	//if(chKey==NULL || chTarget==NULL)
	//	return -1;
	
	//If len of encode data is shorter than target we return -1
	//if(encodeDTLen<targetDTLen)
	//	return -1;

	//add user id in the front of encoded data
	//chEncode[0]=(char)keyLen;
	//memcpy(chEncode+1, chKey, keyLen);

	//printf("keyLen is %d\n",keyLen);
	//printf("key embedded is %s\n",chEncode+1);	

	/*char para1=0;
	char para2=chKey[0];
	for(int i=0;i<targetDTLen;i++)
	{
		int indexOfKey = i%keyLen;
		para2 = chKey[indexOfKey];
		chEncode[i+keyLen+1] = chTarget[i]^para1^para2;	
		para1 = chEncode[i+keyLen+1];
	}*/

	//int totalLen=keyLen+1+targetDTLen+END_DATA_FIELD_LEN;
	//printf("Target Data Length is %d\n", targetDTLen);
	//env->SetByteArrayRegion(encodedData, 0, END_DATA_FIELD_LEN, (jbyte*)&totalLen);
	//env->SetByteArrayRegion(encodedData, END_DATA_FIELD_LEN, keyLen+1+targetDTLen, (jbyte*)chEncode);	
	//char ZERO=0;
	//env->SetByteArrayRegion(encodedData, targetDTLen, 1, (jbyte*)&ZERO);
	//encodeDTLen=env->GetArrayLength(encodedData);
	//printf("Encode function's encode Data Length is %d\n", encodeDTLen);

	//free(chEncode);
	env->ReleaseByteArrayElements(key, (jbyte*)chKey, 0);
	env->ReleaseByteArrayElements(targetData, (jbyte*)chTarget, 0);
	//env->ReleaseByteArrayElements(encodedData, (jbyte*)chEncodedData, 0);
	env->SetByteArrayRegion(encodedData, 0, encodeDTLen, (jbyte*)chEncodedData);
	
	return 0;
}

/* Class:     EncodeContent
 * Method:    DecodeContent
 * Signature: ([B[B[B)I
 */
/*int internal_usc_DecodeContent
  (JNIEnv *env, jobject, jbyteArray key, int keyLen, jbyteArray encodedData, int encodeDTLen, jbyteArray decodedData)
{

	//int keyLen=env->GetArrayLength(key);
	//int encodeDTLen=env->GetArrayLength(encodedData);
	int decodeDTLen=env->GetArrayLength(decodedData);

	char *chKey = (char*)env->GetByteArrayElements(key,0);
 	char *chEncode = (char*)env->GetByteArrayElements(encodedData,0);

	//printf("keyLen in inner function is %d\n",keyLen);
	//printf("key value in inner function is %s\n", chKey);
	//printf("encodeDTLen len in inner function is %d\n", encodeDTLen);
	//printf("encoded data in inner function is %s\n", chEncode);

        char *chDecode = (char*)malloc(encodeDTLen);
        memset(chDecode, 0x00, encodeDTLen);

        if(chKey==NULL || chEncode==NULL)
		return -1;

        //If len of encode data is shorter than target we return -1
        if(decodeDTLen<encodeDTLen)
	        return -1;

        char para1=0;
        char para2=chKey[0];
	int startIndex=END_DATA_FIELD_LEN;
	for(int i=0; i<encodeDTLen; i++)
	{
 	       int indexOfKey = i%keyLen;
               para2 = chKey[indexOfKey];
               chDecode[i] = chEncode[i]^para1^para2;
               para1 = chEncode[i];
        }

	//printf("Encode Data Length is %d\n", encodeDTLen);
	env->SetByteArrayRegion( decodedData, 0, startIndex, (jbyte*)&encodeDTLen);
	env->SetByteArrayRegion( decodedData, startIndex, encodeDTLen, (jbyte*)chDecode);
        free(chDecode);
	//printf("115OK\n");
        env->ReleaseByteArrayElements(key, (jbyte*)chKey, 0);
	//printf("117OK\n");
        env->ReleaseByteArrayElements(encodedData, (jbyte*)chEncode, 0); 
        //printf("119OK\n");
	return 0;
}*/

//JNIEXPORT jint JNICALL usc_DecodeTotalContent(JNIEnv *env, jobject obj, jbyteArray key, jbyteArray encodedData, jbyteArray decodedData)
JNIEXPORT jint JNICALL Java_cn_yunzhisheng_casr_EncodeContent_DecodeTotalContent(JNIEnv *env, jobject obj, jbyteArray key, jbyteArray encodedData, jbyteArray decodedData)
{
	/*char* GenalInfo=(char*)env->GetByteArrayElements(encodedData,0);
	int itotalLen=0;
	memcpy((char*)&itotalLen, GenalInfo, END_DATA_FIELD_LEN);
	char* userIdInfo=GenalInfo+END_DATA_FIELD_LEN;
	int lenOfUserid=userIdInfo[0];
	int realEncodeDTLen=itotalLen-END_DATA_FIELD_LEN-1-lenOfUserid;
	int encodeDTLen=env->GetArrayLength(encodedData);
	
	if(itotalLen>=encodeDTLen)
		return -1;
	
	env->SetByteArrayRegion(key, 0, lenOfUserid, (jbyte*)userIdInfo+1);
	char endSym=0;
	env->SetByteArrayRegion(key, lenOfUserid, 1, (jbyte*)&endSym);
	
	int decodeDTLen=env->GetArrayLength(decodedData);
	char* chDecodedData=(char*)malloc(decodeDTLen);
	memset(chDecodedData, 0x00, decodeDTLen);*/
	
	int realDTLen=0;
	int realKeyLen=0;

	int lenOfUserid = env->GetArrayLength(key);
	char* userIdInfo = (char*)malloc(lenOfUserid);
	memset(userIdInfo, 0x00, lenOfUserid);
	char* chEncodedData=(char*)env->GetByteArrayElements(encodedData,0);
	int encodeDTLen=env->GetArrayLength(encodedData);
	int decodeDTLen=env->GetArrayLength(decodedData);
	char* chDecodedData = (char*)malloc(decodeDTLen);
	memset(chDecodedData, 0x00, decodeDTLen);
	
	//printf("EncodedData Len is %d\n", )

	int status=Encrypter::DecodeTotalContent( userIdInfo, &realKeyLen, chEncodedData, encodeDTLen, chDecodedData, decodeDTLen, &realDTLen);

        if(status==-1)
		return status;

	//Encrypter::DecodeContent((userIdInfo+1), lenOfUserid, userIdInfo+1+lenOfUserid, realEncodeDTLen, chDecodedData, decodeDTLen, &realDTLen);
      	//printf("DecodeTotalContent OK\n");	
	env->SetByteArrayRegion( decodedData, 0, realDTLen, (jbyte*)chDecodedData);
	env->SetByteArrayRegion( key, 0, realKeyLen, (jbyte*)userIdInfo);
	free(userIdInfo);
	free(chDecodedData);
	env->ReleaseByteArrayElements(encodedData, (jbyte*)chEncodedData, 0);
}

/**
 *Table of methods associated with a single class.
 */
#if 0
static JNINativeMethod gMethods[] = {
        { "EncodeTotalContent", "([B[B[B)I", (void*)usc_EncodeContent },
        //{ "DecodeContent", "([B[B[B)I", (void*)usc_DecodeContent },
	{ "DecodeTotalContent", "([B[B[B)I", (void*)usc_DecodeTotalContent },
	//{ "CheckKeyValid", "([B)I", (void*)usc_CheckKeyValid },
};

static int registerNativeMethods(JNIEnv* env, const char* className,JNINativeMethod* gMethods, int numMethods)
{
        jclass clazz;
        //printf("\nfinding class %s\n",className);
        clazz = env->FindClass(className);
        if (clazz == NULL) {
                //printf("\nclass not found!\n");
                return JNI_FALSE;
        }

        //printf("\nregister methods...\n");
        if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
                //printf("\nregister methods failed \n");
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
        //printf("jni loaded....");
        JNIEnv* env = NULL;
        jint result = -1;

        if (vm->GetEnv( (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return -1;
        }

        if (env == NULL)
                return -1;
 
        if (!usc_registerNatives(env)) {
                return -1;
        }
 
        /* success -- return valid version number */
        result = JNI_VERSION_1_6;
 
       	return result;
}
#endif