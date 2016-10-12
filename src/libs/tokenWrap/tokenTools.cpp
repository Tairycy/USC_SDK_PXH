#include "tokenTools.h"
#include "getTime.h"
#include "tokenWrap.h"
#include "genToken.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
const unsigned int TOKENLEN = 1024;

struct TokenInfoS{
	unsigned long serverTimeStamp;
	unsigned long clientStartTS;
	char tokenFromServer[TOKENLEN];
	unsigned int tokenLen;
};

typedef struct TokenInfoS TokenInfo;

int createTokenService(TNHANDLE* handle, const char* token, const char* appKey, unsigned long time){

	// LOGD("TTS","createTokenService called");
	if(token == NULL)
		return -1;	

	TokenInfo* info = (TokenInfo*)malloc(sizeof(TokenInfo));
	memset(info, 0x00, sizeof(TokenInfo));

	unsigned int inLen = strlen(token);
	const char* rawToken = TokenWrap::decyptToken(token, &inLen, appKey);

	if(rawToken==0){
		free((void*)info);
		free((void*)rawToken);
		return -1;
	}

	info->tokenLen = inLen;

	if(inLen<TOKENLEN){
		memcpy(info->tokenFromServer, rawToken, inLen);
	}
	else{
		free((void*)info);
		free((void*)rawToken);
		return -1;
	}

	info->serverTimeStamp = TokenWrap::getTokenTimeStamp(token, strlen(token), appKey);
	if (time == 0)
		info->clientStartTS = getCurrentTimeS();
	else
		info->clientStartTS = time;

    // char temp[128];
    // sprintf(temp,"TK createTokenService:clientStartTS=%ld,serverTimeStamp=%ld\n", info->clientStartTS,info->serverTimeStamp);
	// LOGI("TTS",temp);

	free((void*)rawToken);
	*handle = (TNHANDLE)info;

	return 0;
}

const char* genNewToken(TNHANDLE inHandle, unsigned int* len){

	TokenInfo* handle = (TokenInfo*)inHandle;

	if(handle == NULL) {
		return NULL;
	}

	unsigned long currentTimeS = getCurrentTimeS();

	unsigned int inLen = handle->tokenLen;

	unsigned long reCalTimeS = currentTimeS - handle->clientStartTS + handle->serverTimeStamp;

	const char* newToken = genTokenUsingTimestamp(handle->tokenFromServer, &inLen, reCalTimeS);

	const char* reToken = TokenWrap::encyptToken( newToken, inLen, NULL);
	if (newToken != NULL)
		free((void*)newToken);
	// LOGI("TTS", "genNewToken encyptToken");
	return reToken; 
}

int releaseTokenService(TNHANDLE inHandle){

	// LOGI("TTS", "releaseTokenService called\n");
	TokenInfo* handle = (TokenInfo*)inHandle;
	if(handle)
		free(handle);
	return 0;
}

