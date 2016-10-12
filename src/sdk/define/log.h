#ifndef __LOG_H__
#define __LOG_H__

struct SUCC_CODE{
    static const int ASRCLIENT_RECOGNIZER_OK = 0;
    static const int ASRCLIENT_CREATE_SERVICE_OK = 0;
    static const int ASRCLIENT_START_SESSION_OK = 0;
    static const int ASRCLIENT_RECOGNIZER_NO_RESULT = 1;
    static const int ASRCLIENT_RECOGNIZER_PARTIAL_RESULT = 2;
};

#ifdef ANDROID_ENV
#include <android/log.h>
#define LOGD(tag, msg) __android_log_print(ANDROID_LOG_DEBUG, tag, msg)
#define LOGI(tag, msg) __android_log_print(ANDROID_LOG_INFO, tag, msg)
#define LOGW(tag, msg) __android_log_print(ANDROID_LOG_WARN, tag, msg)
#define LOGE(tag, msg) __android_log_print(ANDROID_LOG_ERROR, tag, msg)
#else
#define LOGD(tag, msg) fprintf(stderr, "%s\t: %s\n", tag, msg)
#define LOGI(tag, msg) fprintf(stderr, "%s\t: %s\n", tag, msg)
#define LOGW(tag, msg) fprintf(stderr, "%s\t: %s\n", tag, msg)
#define LOGE(tag, msg) fprintf(stderr, "%s\t: %s\n", tag, msg)
#endif


#define LOGMYD(tag, msg, level) LOG::LOGLQY(tag,msg, level, 2)
#define LOGMYI(tag, msg, level) LOG::LOGLQY(tag,msg, level, 3)
#define LOGMYW(tag, msg, level) LOG::LOGLQY(tag,msg, level, 4)
#define LOGMYE(tag, msg, level) LOG::LOGLQY(tag,msg, level, 5)

class LOG
{
public:
	static void LOGLQY(char* tag, char* msg, int level, int logLevel);
	static void SetLogLevel(int level);
	static void SetLogWrite(int isWrite);
	static void SetLogWriteFile(const char * file);
};

#endif	// __LOG_H__


