#include "genToken.h"
#include "getTime.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------------------------------------
 *|version|secrect|random|usr info|recv|time|app key len|app key |usr id len|usr id  |imei len|imei    |crc|
 *----------------------------------------------------------------------------------------------------------
 *|   2   |   8   |  4   |   4    | 4  | 8  |     1     |var(127)|    1     |var(127)|   1    |var(127)| 4 |
 *----------------------------------------------------------------------------------------------------------
 */
/*
const char* genToken(char version, const char* appKey, const char* usrId, const char* imei, unsigned int* len)
{
    if (NULL==appKey||NULL==usrId||NULL==imei||NULL==len)
	{
        return NULL;
    }

    int appKeyLen = strlen(appKey);
    int usrIdLen = strlen(usrId);
    int imeiLen = strlen(imei);

    if (appKeyLen>127||usrIdLen>127||imeiLen>127)
	{
        return NULL;
    }

	int origLen = G_MIN_TOKEN_LENGTH+appKeyLen+usrIdLen+imeiLen;
	int totalNum = (origLen%POSRESV==0)?origLen:(origLen/POSRESV+1)*POSRESV;
	//printf("totalNum is %d\n", totalNum);
	char* token = (char*)calloc(totalNum, sizeof(char));

	char* versionBufferT = token;
	versionBufferT[0] = 0;
	versionBufferT[1] = version;

	char* shareSecretBufferT = token+VERSIONLEN;
	shareSecretBufferT[0] = 'y';
	shareSecretBufferT[1] = 'u';
	shareSecretBufferT[2] = 'n';
	shareSecretBufferT[3] = 'z';
	shareSecretBufferT[4] = 'h';
	shareSecretBufferT[5] = 'i';
	shareSecretBufferT[6] = 's';	
	shareSecretBufferT[7] = 'h';

	char* randNumT = token+VERSIONLEN+SSLEN;
	unsigned long currentTimeM = getCurrentTimeM();	
	int genRand = 0;
#if defined(WIN32) || defined(_WIN32)
	srand(currentTimeM);
	genRand = rand();	
#else
	genRand = rand_r((unsigned int*)&currentTimeM);	
#endif
#if defined(WIN32) || defined(_WIN32)
	_snprintf(randNumT, RANDLEN, "%d", genRand);
#else
	snprintf(randNumT, RANDLEN, "%d", genRand);
#endif
	
	char* userInfoT = token+VERSIONLEN+SSLEN+RANDLEN;
	userInfoT[USERINFOLEN-1] = 1;	

	char* resvT = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN;

	char* timeStampT = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN;
	unsigned long currentTimeS = getCurrentTimeS();
#if defined(WIN32) || defined(_WIN32)
	//_snprintf(timeStamp,TTLEN,"%ld",currentTimeS);
	memcpy(timeStampT, (unsigned long*)&currentTimeS, sizeof(unsigned long));
#else
	//snprintf(timeStamp,TTLEN,"%ld",currentTimeS);
	memcpy(timeStampT, (unsigned long*)&currentTimeS, sizeof(unsigned long));
#endif
	char* appKeyLenT = timeStampT+TTLEN;
        *appKeyLenT = (char)appKeyLen;

        char* appKeyT = appKeyLenT+APP_KEY_LEN_LEN;
        memcpy(appKeyT, appKey, *appKeyLenT);

	char* usrIdLenT = appKeyT+*appKeyLenT;
        *usrIdLenT = (char)usrIdLen;

        char* usrIdT = usrIdLenT+USR_ID_LEN_LEN;
        memcpy(usrIdT, usrId, *usrIdLenT);

	char* imeiLenT = usrIdT+*usrIdLenT;
        *imeiLenT = (char)imeiLen;

        char* imeiT = imeiLenT+IMEI_LEN_LEN;
        memcpy(imeiT, imei, *imeiLenT);

	*len = totalNum;

	return token;
}
*/
static void printArray(void* src, unsigned int inLen)
{
	char* dst = (char*)src;
	for(int i=0; i<inLen; i++)
	{
        printf("%x\t",dst[i]);
    }       
    cout<<endl;
}

const char* genTokenUsingTimestamp(const char* origToken, unsigned int* len, unsigned long timeStamp)
{
	int origLen = *len;
    int totalNum = (origLen%POSRESV==0)?origLen:(origLen/POSRESV+1)*POSRESV;

	if(totalNum<*len)
		return NULL;
#ifdef DEBUG
    printf("len %d, origLen %d, totalNum is %d\n", *len, origLen, totalNum);	
    fflush(stdout);
    printf("origin token:\n");
    printArray((void *)origToken, origLen);
    fflush(stdout);
#endif
	char* token = (char*)calloc(totalNum, sizeof(char));
	memcpy(token, origToken, *len); 

	char* timeStampAddr = NULL;
	timeStampAddr = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN;
#if defined(WIN32) || defined(_WIN32)
	//_snprintf(timeStampAddr,TTLEN,"%ld",timeStamp);
	memcpy(timeStampAddr, (unsigned long*)&timeStamp, sizeof(unsigned long));
#else
	//snprintf(timeStampAddr,TTLEN,"%ld",timeStamp);
	memcpy(timeStampAddr, (unsigned long*)&timeStamp, sizeof(unsigned long));
#endif
#ifdef DEBUG
    printf("update token:\n");
    printArray((void *)origToken, origLen);
    fflush(stdout);
#endif
	*len = totalNum;
	return token;
}
/*
const char* genTokenMorePara(char version, const char* appKey, const char* usrID, const char* imei, unsigned int* len)
{
	int origLen = VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN+TTLEN+CRCLEN;
	int totalNum = (origLen%POSRESV==0)?origLen:(origLen/POSRESV+1)*POSRESV;
	//printf("totalNum is %d\n", totalNum);
	char* token = (char*)malloc(totalNum);
	memset(token, 0x00, totalNum);

	char* versionBuffer = token;
	versionBuffer[0] = 0;
	versionBuffer[1] = version;

	char* shareSecretBuffer = token+VERSIONLEN;
	shareSecretBuffer[0] = 'y';
	shareSecretBuffer[1] = 'u';
	shareSecretBuffer[2] = 'n';
	shareSecretBuffer[3] = 'z';
	shareSecretBuffer[4] = 'h';
	shareSecretBuffer[5] = 'i';
	shareSecretBuffer[6] = 's';	
	shareSecretBuffer[7] = 'h';

	char* randNum = token+VERSIONLEN+SSLEN;
	unsigned long currentTimeM = getCurrentTimeM();	
	int genRand = 0;
#if defined(WIN32) || defined(_WIN32)
	srand(currentTimeM);
	genRand = rand();	
#else
	genRand = rand_r((unsigned int*)&currentTimeM);	
#endif
#if defined(WIN32) || defined(_WIN32)
	_snprintf(randNum, RANDLEN, "%d", genRand);
#else
	snprintf(randNum, RANDLEN, "%d", genRand);
#endif
	
	char* userInfo = token+VERSIONLEN+SSLEN+RANDLEN;
	userInfo[USERINFOLEN-1] = 1;	

	char* resv = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN;
	memset(resv,0x00,RESVLEN);

	char* timeStamp = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN;
	memset(timeStamp, 0x00, TTLEN);
	unsigned long currentTimeS = getCurrentTimeS();
#if defined(WIN32) || defined(_WIN32)
	//_snprintf(timeStamp,TTLEN,"%ld",currentTimeS);
	memcpy(timeStamp, (unsigned long*)&currentTimeS, sizeof(unsigned long));
#else
	//snprintf(timeStamp,TTLEN,"%ld",currentTimeS);
	memcpy(timeStamp, (unsigned long*)&currentTimeS, sizeof(unsigned long));
#endif
	
	char* crcBuffer = token+VERSIONLEN+SSLEN+RANDLEN+USERINFOLEN+RESVLEN+TTLEN;
	memset(crcBuffer, 0x00, CRCLEN);

	*len = totalNum;

	return token;
}
*/