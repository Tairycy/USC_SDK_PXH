#include "function.h"
#include "getTime.h"
#include "md5.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#ifdef LINUX
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h> 
#elif defined (WIN32)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <objbase.h>
#elif defined (iOS)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define GUID_LEN 128

int GetRandomNum()
{
	int num;
#if defined (__ANDROID__)
	num = getCurrentTimeM();
#else
	num = rand();
#endif
	return num;
}

int TransError2User(int errorCode)
{
	int errorRet;
	errorRet = -1;
	if (errorCode < 100 || errorCode > 999)
		return -91700;
	int head = errorCode / 100;
	int tail = errorCode % 100;
	switch (head)
	{
	case 4:
		if (tail <= 18)
			errorRet = -91700 - tail;
		else
			errorRet = -91719;
		break;
	case 5:
		if (tail <= 78)
			errorRet = -91720 - tail;
		else
			errorRet = -91799;
		break;
	case 6:
		if (tail <= 98)
			errorRet = -91800 - tail;
		else
			errorRet = -91899;
		break;
	default:
		errorRet = -91880 - head;
		break;
	}
	return errorRet;
}

void usc_sleep(int milliseconds) {
#ifdef WIN32
	Sleep(milliseconds);
#endif
}

std::string GetSystemTimeS_Y_M_D()
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::string new_time = asctime(timeinfo);
	if (new_time.length() > 0)
		new_time.erase(new_time.length() - 1, 1);
	return new_time;
}

std::string GetSystemTimeS()
{
	std::string new_time = GetSystemTimeS_Y_M_D();
	if (new_time.length() > 5)
	{
		new_time.erase(new_time.length() - 5, 5);
		new_time.erase(0, 4);
	}
	return new_time;
}

std::string GetSystemTimeTick()
{
	std::string new_time = GetSystemTimeS();
	char tmp[128] = { 0 };
	sprintf(tmp, "%d", getCurrentTimeM());
	std::string st_tmp = tmp;
	new_time += ".";
	new_time += st_tmp.substr(st_tmp.length() - 3, 3);
	return new_time;
}

std::string GetUuid()
{
	std::string ret = "";
	char buffer[GUID_LEN] = { 0 };
#ifdef WIN32
	GUID guid;
	if (CoCreateGuid(&guid))
	{
		fprintf(stderr, "create guid error\n");
		return "";
	}
	_snprintf(buffer, sizeof(buffer),
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
#elif defined (iOS)
	struct timespec time_start = { 0, 0 }, time_end = { 0, 0 };
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	mts.tv_sec = mts.tv_sec;
	mts.tv_nsec = mts.tv_nsec;
	sprintf(buffer, "%llus%lluns", time_start.tv_sec, time_start.tv_nsec);
#else
	struct timespec time_start = { 0, 0 }, time_end = { 0, 0 };
	clock_gettime(CLOCK_REALTIME, &time_start);
	sprintf(buffer, "%llus%lluns", time_start.tv_sec, time_start.tv_nsec);
#endif
	ret = buffer;
	return ret;
}

int GetMd5(char* str, char* decrypt_ans)
{
	int i;
	unsigned char decrypt[160];
	memset(decrypt, 0, sizeof(decrypt));
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)str, strlen((char *)str));
	MD5Final(&md5, (unsigned char*)decrypt);
	memset(decrypt_ans, 0, sizeof(decrypt_ans));
	for (i = 0; i<16; i++)
	{
		sprintf((char*)decrypt_ans, "%s%02x", decrypt_ans, decrypt[i]);
	}
	return 0;
}

int GetBitOfInt(unsigned int value, int bit)
{
	unsigned int tmp = value;
	tmp = tmp << (32 - bit - 1);
	tmp = tmp >> (32 - bit - 1);
	tmp = tmp >> (bit);
	return tmp;
}

