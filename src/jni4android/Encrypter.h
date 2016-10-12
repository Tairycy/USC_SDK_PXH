#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 

#define END_DATA_FIELD_LEN 4
#define KEY_DATA_FIELD_LEN 1

class Encrypter{
public:
 static int EncodeContent
  (char* key, int keyLen, char* targetData, int targetDTLen, char* encodedData, int encodeDTLen)
{
	
	//printf("Enter EncodeContent\n");
	//printf("key value is %s\n",key);
	//printf("key len is %d\n", keyLen);
	//printf("targetData value is %s\n",targetData);
        //printf("targetData len is %d\n",targetDTLen);
	//printf("encodedData len is %d\n",encodeDTLen);
	//int keyLen=env->GetArrayLength(key);
	//int targetDTLen=env->GetArrayLength(targetData);
	//int encodeDTLen=env->GetArrayLength(encodedData);
	char *chKey = key;
	char *chTarget = targetData;
	//printf("target is %s\n", chTarget);
	char *chEncode = (char*)malloc(encodeDTLen);
	memset(chEncode, 0x00, encodeDTLen);
	//(char*)env->GetByteArrayElements(encodedData,0);
	
	//If key or target data is NULL, we return -1
	if(chKey==NULL || chTarget==NULL)
		return -1;
	
	//If len of encode data is shorter than target we return -1
	if(encodeDTLen<targetDTLen)
		return -1;

	//add user id in the front of encoded data
	chEncode[0]=(char)keyLen;
	memcpy(chEncode+1, chKey, keyLen);

	//printf("keyLen is %d\n",keyLen);
	//printf("key embedded is %s\n",chEncode+1);	

	char para1=0;
	char para2=chKey[0];
	for(int i=0;i<targetDTLen;i++)
	{
		int indexOfKey = i%keyLen;
		para2 = chKey[indexOfKey];
		chEncode[i+keyLen+1] = chTarget[i]^para1^para2;	
		para1 = chEncode[i+keyLen+1];
	}

	int totalLen=keyLen+1+targetDTLen+END_DATA_FIELD_LEN;
	//printf("Target Data Length is %d\n", targetDTLen);
	//env->SetByteArrayRegion(encodedData, 0, END_DATA_FIELD_LEN, (jbyte*)&totalLen);
	//env->SetByteArrayRegion(encodedData, END_DATA_FIELD_LEN, keyLen+1+targetDTLen, (jbyte*)chEncode);	
	int nwSeqLen = htonl(totalLen);
	memcpy(encodedData, (char*)&nwSeqLen,END_DATA_FIELD_LEN);
	memcpy(encodedData+END_DATA_FIELD_LEN, chEncode, keyLen+1+targetDTLen);
	//char ZERO=0;
	//env->SetByteArrayRegion(encodedData, targetDTLen, 1, (jbyte*)&ZERO);
	//encodeDTLen=env->GetArrayLength(encodedData);
	//printf("Encode function's encode Data Length is %d\n", encodeDTLen);

	free(chEncode);
	//env->ReleaseByteArrayElements(key, (jbyte*)chKey, 0);
	//env->ReleaseByteArrayElements(targetData, (jbyte*)chTarget, 0);
	
	return 0;
}

static int DecodeContent
  (char* key, int keyLen, char* encodedData, int encodeDTLen, char*  decodedData, int decodeDTLen, int* realDTLen)
{

	//int keyLen=env->GetArrayLength(key);
	//int encodeDTLen=env->GetArrayLength(encodedData);
	//int decodeDTLen=env->GetArrayLength(decodedData);

	//char *chKey = (char*)env->GetByteArrayElements(key,0);
 	//char *chEncode = (char*)env->GetByteArrayElements(encodedData,0);
	char* chKey=key;
	char* chEncode=encodedData;

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
	//int startIndex=END_DATA_FIELD_LEN;
	for(int i=0; i<encodeDTLen; i++)
	{
 	       int indexOfKey = i%keyLen;
               para2 = chKey[indexOfKey];
               chDecode[i] = chEncode[i]^para1^para2;
               para1 = chEncode[i];
        }

	//printf("Encode Data Length is %d\n", encodeDTLen);
	//env->SetByteArrayRegion( decodedData, 0, startIndex, (jbyte*)&encodeDTLen);
	//env->SetByteArrayRegion( decodedData, startIndex, encodeDTLen, (jbyte*)chDecode);
	memcpy( decodedData, (char*)&encodeDTLen, END_DATA_FIELD_LEN);
	memcpy( decodedData+END_DATA_FIELD_LEN, chDecode, encodeDTLen);
	*realDTLen=encodeDTLen+END_DATA_FIELD_LEN;
        free(chDecode);
	//printf("115OK\n");
        // env->ReleaseByteArrayElements(key, (jbyte*)chKey, 0);
	//printf("117OK\n");
        //  env->ReleaseByteArrayElements(encodedData, (jbyte*)chEncode, 0); 
        //printf("119OK\n");
	return 0;
}

static int DecodeTotalContent( char* key, int* keyLen, char* encodedData, int encodeDTLen, char* decodedData, int decodeDTLen, int* realDTLen)
{
        //char* GenalInfo=(char*)env->GetByteArrayElements(encodedData,0);
        char* GenalInfo=encodedData;
	int itotalLen=0;
	int nwSeqLen=0;
        memcpy((char*)&nwSeqLen, GenalInfo, END_DATA_FIELD_LEN);
	itotalLen=ntohl(nwSeqLen);
	//itotalLen = nwSeqLen;
        char* userIdInfo=GenalInfo+END_DATA_FIELD_LEN;
        char lenOfUserid=userIdInfo[0];
        int realEncodeDTLen=itotalLen-END_DATA_FIELD_LEN-1-lenOfUserid;
        //int encodeDTLen=env->GetArrayLength(encodedData);
        
        if(itotalLen>=encodeDTLen)
                return -1;

	//printf("tatol input len is %d\n", itotalLen);
	//printf("userid len is %d\n", lenOfUserid);
	//printf("encoded data len is %d\n", realEncodeDTLen);
	//printf("decoded data len of storage is %d\n", decodeDTLen);        
        
        //env->SetByteArrayRegion(key, 0, lenOfUserid, (jbyte*)userIdInfo+1);
        memcpy(key+KEY_DATA_FIELD_LEN, userIdInfo+1, lenOfUserid);
	//char endSym=0;
        //env->SetByteArrayRegion(key, lenOfUserid, 1, (jbyte*)&endSym);
        memcpy(key, (char*)&lenOfUserid, KEY_DATA_FIELD_LEN);
	*keyLen=lenOfUserid+KEY_DATA_FIELD_LEN;        

        //int decodeDTLen=env->GetArrayLength(decodedData);
        //char* chDecodedData=(char*)malloc(decodeDTLen);
        //memset(chDecodedData, 0x00, decodeDTLen);
        
        Encrypter::DecodeContent((userIdInfo+1), lenOfUserid, userIdInfo+1+lenOfUserid, realEncodeDTLen, decodedData, decodeDTLen, realDTLen);
        
        //env->SetByteArrayRegion( decodedData, 0, realDTLen, (jbyte*)chDecodedData);
        //memcpy(decodedData, chDecodedData, realDTLen);
        //free(chDecodedData);
        //env->ReleaseByteArrayElements(encodedData, (jbyte*)GenalInfo, 0);
        return 0;
} 

};

