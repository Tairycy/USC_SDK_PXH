#include "getTime.h"
#include <stdio.h>
#include "log.h"

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

unsigned long GetTickCount_()
{
#if defined(WIN32) || defined(_WIN32)
    return GetTickCount();

#elif defined(iOS)
    time_t tm;
    time(&tm);
    return tm * 1000;

#elif defined(ANDROID_ENV)

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else 
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);

#endif
	return 0;
}

unsigned long getCurrentTimeM()
{
	return GetTickCount_();
}

unsigned long getCurrentTimeS()
{
	return GetTickCount_()/1000;
}

void sleepMS(unsigned int time)
{
	if (time > 10000000)
		return ;
#ifdef WIN32
	Sleep(time);
#else
	usleep(time * 1000);
#endif
}