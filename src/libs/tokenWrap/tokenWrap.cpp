#include "tokenWrap.h"
#include "cBase64.h"
#include "genToken.h"
#include "AES.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
using namespace std;
#ifdef PRINT_LOG
#include <string>
#include <iostream>
#endif

#ifdef TEST_TOKENWRAP

static void printArray(void* src, unsigned int inLen){
	char* dst = (char*)src;
	for(int i=0; i<inLen; i++){
                printf("%x\t",dst[i]);
        }       
        cout<<endl;
}
#endif

static void replaceOne(void* src, unsigned int inLen){
	char tmp = 0;
	char* dst = (char*)src;
#ifdef TEST_TOKENWRAP
	printArray(src, inLen);	
#endif
	for(int i=0; i<inLen/2; i++){
		tmp = dst[2*i];
		dst[2*i] = dst[2*i+1];
		dst[2*i+1] = tmp;
	}
#ifdef TEST_TOKENWRAP
        printArray(src, inLen);
#endif
}

static void uniOne(void* src, unsigned int inLen){
	char* dst = (char*)src;
	char* tmp = (char*)malloc(inLen);
	memcpy(tmp, src, inLen);

#ifdef TEST_TOKENWRAP
        printArray(src, inLen);
#endif
	
	int time = (inLen%2==0)?inLen/2:(inLen/2+1);
	for(int i=0; i<time; i++){
		dst[2*i] = tmp[i];
		if(2*i+1<inLen)
			dst[2*i+1] = tmp[i+time];
	}

#ifdef TEST_TOKENWRAP
        printArray(dst, inLen);
#endif

	free(tmp);
}

static void revOne(void* src, unsigned int inLen){
	char* dst = (char*)src;
	char* tmp = (char*)malloc(inLen);
	memcpy(tmp, src, inLen);

#ifdef TEST_TOKENWRAP
        printArray(src, inLen);
#endif

	int time = (inLen%2==0)?inLen/2:(inLen/2+1);
        for(int i=0; i<time; i++){
                dst[i] = tmp[2*i];
                if(2*i+1<inLen)
                        dst[i+time] = tmp[2*i+1];
        }

#ifdef TEST_TOKENWRAP
        printArray(dst, inLen);
#endif

	free(tmp);
}

static const char encryString[]="yunzhisheng.cn";

static void easyEncrypt(void* src, int inLen){

	unsigned char key[] =
        {
                0x2b, 0x7e, 0x15, 0x16,
                0x28, 0xae, 0xd2, 0xa6,
                0xab, 0xf7, 0x15, 0x88,
                0x09, 0xcf, 0x4f, 0x3c
        };
	AES aes(key);
	aes.Cipher(src, inLen);	
}

static void easyDecrypt(void* src, int inLen){

	unsigned char key[] =
        {
                0x2b, 0x7e, 0x15, 0x16,
                0x28, 0xae, 0xd2, 0xa6,
                0xab, 0xf7, 0x15, 0x88,
                0x09, 0xcf, 0x4f, 0x3c
        };
        AES aes(key);
	aes.InvCipher(src, inLen);
}


static void easyEncryptWithKey(void* src, int inLen, const char* inkey){

	unsigned char* key = (unsigned char*)malloc(16+1);
	memset(key, 0x00, 16+1);
	memcpy(key, inkey, 16);
	AES aes(key);
	aes.Cipher(src, inLen);	
	free((void*)key);
}

static void easyDecryptWithKey(void* src, int inLen, const char* inkey){

	unsigned char* key = (unsigned char*)malloc(16+1);
        memset(key, 0x00, 16+1);
        memcpy(key, inkey, 16);
        AES aes(key);
	aes.InvCipher(src, inLen);
	free((void*)key);
}

const char* TokenWrap::encyptToken(const char* origToken, unsigned int inLen, const char* secret){
#ifdef PRINT_LOG
	cout<<"TokenWrap::encyptToken called"<<endl;
#endif
	if(inLen%POSRESV!=0)
		return NULL;	

	if(origToken==NULL || inLen==0)
		return NULL;

        void* tmp = malloc(inLen);
	memcpy(tmp, origToken, inLen);
	
	//one. replace
	replaceOne(tmp, inLen);		
	//two. uni
	uniOne(tmp, inLen);

	//three replace 
	replaceOne(tmp, inLen);
	uniOne(tmp, inLen);

	easyEncrypt(tmp, inLen);

#ifdef TEST_TOKENWRAP
        printArray(tmp, inLen);
#endif

	if(secret && strlen(secret)>=POSRESV){
		easyEncryptWithKey(tmp, inLen, secret);
	}

	char* reString = NULL;
	CBase64::CreateMatchingEncodingBuffer (inLen, &reString);	
	CBase64::EncodeBuffer((char*)tmp, inLen, reString);
	
	free(tmp);
#ifdef PRINT_LOG
	cout<<"orig len is "<<inLen<<endl;
#endif
	return reString;
}

const char* TokenWrap::decyptToken(const char* token, unsigned int* inLen, const char* secret){

#ifdef PRINT_LOG
	cout<<"TokenWrap::decyptToken called"<<endl;
#endif


	if(token==NULL || inLen==0)
		return NULL;

	char* decodedToken = NULL;
	int reLen = CBase64::CreateMatchingDecodingBuffer((char*)token, &decodedToken);
	reLen--;
#ifdef PRINT_LOG
	cout<<"!!!!!!!!decodeLen is "<<reLen<<endl;
#endif

	if(reLen%POSRESV!=0){
#ifdef PRINT_LOG
		cout<<"decode token len is invalid"<<endl;
#endif
		free(decodedToken);
		return NULL;
	}

	CBase64::DecodeBuffer((char*)token, decodedToken);

	if(secret && strlen(secret)>=POSRESV){
                easyDecryptWithKey(decodedToken, reLen, secret);
#ifdef PRINT_LOG
		cout<<"call easyDecryptWithKey"<<endl;
#endif
        }

	easyDecrypt(decodedToken, reLen);

	revOne(decodedToken, reLen);	
	replaceOne(decodedToken, reLen);

	revOne(decodedToken, reLen);
        replaceOne(decodedToken, reLen);

	*inLen = reLen;

	return decodedToken;

}

unsigned long TokenWrap::getTokenTimeStamp(const char* tokenBase64, unsigned int inLen, const char* secret){
	
	unsigned int len = inLen;
	const char* rawToken = TokenWrap::decyptToken(tokenBase64, &len, secret);

	if(len%POSRESV!=0){
		free((void*)rawToken);
		return (unsigned long)NULL;
	}

	char* timeStamp = (char*)rawToken+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN;
	char* tsBuffer = (char*)malloc(TTLEN+1);
	memset(tsBuffer, 0x00, TTLEN+1);
	memcpy(tsBuffer, timeStamp, TTLEN);

	//unsigned long reValue = atol(tsBuffer);
	unsigned long reValue = *((unsigned long*)tsBuffer);

	free((void*)tsBuffer);
	free((void*)rawToken);

	return reValue;
}

