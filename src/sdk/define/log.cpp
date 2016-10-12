#include "log.h"
#include <time.h>
#include "function.h"
#include <stdio.h>

static int log_level = 0;
static unsigned int log_switch = 3;
static std::string log_file = "uni_log.txt";

#ifdef ANDROID_ENV
#include <android/log.h>
#endif

void PrintLog(char* tag, char* msg, int logLevel)
{
	switch (logLevel)
	{
	case 3:
		LOGI(tag, msg);
		break;
	case 5:
		LOGE(tag, msg);
		break;
	case 4:
		LOGW(tag, msg);
		break;
	case 2:
		LOGD(tag, msg);
		break;
	default:
		break;
	}
}

const char* LogChar(int logLevel)
{
	switch (logLevel)
	{
	case 3:
		return "INFO - ";
	case 5:
		return "ERRO - ";
	case 4:
		return "WARN - ";
	case 2:
		return "DEBU - ";
	default:
		return "";
	}
}

void LOG::LOGLQY(char* tag, char* msg, int level, int logLevel)
{
	if (level <= log_level && log_switch > 0)
	{
#ifdef ANDROID_ENV
		std::string tag_str = tag;
		std::string msg_str = msg;
#else
		std::string msg_str = LogChar(logLevel);
		msg_str += tag;
		msg_str += '\t';
		msg_str += msg;
		std::string tag_str = GetSystemTimeTick();
#endif
		if (GetBitOfInt(log_switch, 0))
		{
			PrintLog((char*)tag_str.c_str(), (char*)msg_str.c_str(), logLevel);
		}
		if (GetBitOfInt(log_switch, 1))
		{
			FILE* flog = fopen(log_file.c_str(), "a+");
			if (flog != NULL)
			{
				std::string AllMsg = "";
				AllMsg += tag_str + "\t" + msg_str + "\n";
				fprintf(flog, "%s", AllMsg.c_str());
				fclose(flog);
			}
		}
	}
}

void LOG::SetLogLevel(int level)
{
	log_level = level;
}

void LOG::SetLogWrite(int isWrite)
{
	log_switch = (unsigned int)isWrite;
}

void LOG::SetLogWriteFile(const char * file)
{
	if (file != NULL)
		log_file = file;
}

