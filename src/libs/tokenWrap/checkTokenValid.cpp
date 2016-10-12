#include "checkTokenValid.h"
#include "genToken.h"
#include "getTime.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
char validString[] = "yunzhish";
unsigned long delta = 10;

char checkTokenValid(const char* token){

	if(token==NULL)
		return -1;

	const char* shareSecretAddr = token+VERSIONLEN;
	char* ssBuffer = (char*)malloc(SSLEN+1);
	memset(ssBuffer, 0x00, SSLEN+1);
	memcpy(ssBuffer, shareSecretAddr, SSLEN);
	char result = 0;
	if(strcmp(ssBuffer, validString)==0){
		const char* timeAddr = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN;
		char* timeBuffer = (char*)malloc(TTLEN+1);
		memset(timeBuffer, 0x00, TTLEN+1);
		
		memcpy(timeBuffer, timeAddr, TTLEN);
//		unsigned long timeS = atol(timeBuffer);
		unsigned long timeS = *((unsigned long*)timeBuffer);

		
		unsigned long currentTime = getCurrentTimeS();
		//printf("checkTokenValid token time:%ld,current time:%ld\n", timeS, currentTime);
		if(currentTime>timeS-delta && currentTime<timeS+delta){
			result = 0;
		}
		else
			result = -1;

		free((void*)timeBuffer);
	}
	else{
		printf("validString not equal\n");
		free((void*)ssBuffer);
		return -1;
	}
	free((void*)ssBuffer);
	return result;
}
