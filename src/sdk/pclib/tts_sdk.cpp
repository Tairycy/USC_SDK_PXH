#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tts_sdk.h"
#include "errorNo.h"
#include "set_option_key.h"
#include "http_def.h"
#include "tts_client.h"
#include "log.h"
#include "function_pc.h"
#include "Version.h"

#ifdef LINUX
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>  
#include <sys/ioctl.h> 
#elif defined (WIN32)
#include <windows.h>
#include <wincon.h>
#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")
#endif

void SetImei(TTSHANDLE handle)
{
	if (handle != 0)
	{
		char mac_buffer[256];
		memset(mac_buffer, 0, sizeof(mac_buffer));
		if (0 == get_mac(mac_buffer, sizeof(mac_buffer)))
		{
			tts_set_option_id(handle, OPT_IMEI_ID, mac_buffer);
			tts_set_option_id(handle, OPT_USER_ID, mac_buffer);
			tts_set_option_id(handle, OPT_UDID_ID, mac_buffer);
		}
	}
}

#ifdef __cplusplus
extern "C"{
#endif


    //公有云创建合成接口实例
    DLL_PUBLIC
    int usc_tts_create_service(TTSHANDLE* handle)
    {
		InitNetwork();
        int ret = tts_create_service(handle);
		if (ret == 0)
			SetImei(*handle);
		return ret;
    }

    //私有云创建合成接口实例
    DLL_PUBLIC
    int usc_tts_create_service_ext(TTSHANDLE* handle, const char *host, const char *port)
    {
		InitNetwork();
		int ret = tts_create_service_ext(handle, host, port);
		if (ret == 0)
			SetImei(*handle);
		return ret;
    }

	// 开始合成
    DLL_PUBLIC
    int usc_tts_start_synthesis(TTSHANDLE handle)
	{
		char mac_buffer[256];
		memset(mac_buffer, 0, sizeof(mac_buffer));
		if (0 == get_client_info(mac_buffer, sizeof(mac_buffer), atoi(usc_tts_get_option(handle, OPT_LAST_SOCKET_ERROR, NULL)), "tts"))
		{
			tts_set_option_id(handle, OPT_COLLECTED_INFO, mac_buffer);
		}
        return tts_start_synthesis(handle);
    }

    //上传文本数据进行合成
    DLL_PUBLIC
    int usc_tts_text_put(TTSHANDLE handle, const char * textdata, unsigned int textlen)
	{
        return tts_text_put(handle, textdata, textlen);
    }

    //获取合成结果
    DLL_PUBLIC
    const void* usc_tts_get_result(TTSHANDLE handle, unsigned int* audioLen, int* synthStatus, int* errorCode)
    {
		return tts_get_result(handle, audioLen, synthStatus, errorCode);
    }

    // 停止合成
    DLL_PUBLIC
    int usc_tts_stop_synthesis(TTSHANDLE handle)
    {
		return tts_stop_synthesis(handle);
    }

    //释放合成接口实例
    DLL_PUBLIC
    int usc_tts_release_service(TTSHANDLE handle)
    {
        return tts_release_service(handle);
    }

	//得到扩展属性
    DLL_PUBLIC
    const char* usc_tts_get_option(TTSHANDLE handle, int option_id, int* errorCode)
    {
		return tts_get_option(handle, option_id, errorCode);
    }
    
	//得到版本号
    DLL_PUBLIC
    const char* usc_tts_get_version(TTSHANDLE handle, int* errorCode)
    {
		return USC_SDK_VERSION;
    }
    
	// 设置参数
    DLL_PUBLIC
    int usc_tts_set_option(TTSHANDLE handle, const char* key, const char* value)
    {
		if (key == NULL || strlen(key) >= 100)
			return OPTION_ID_ERROR;
		else if (value == NULL || strlen(value) >= 1000)
			return OPTION_PARAM_ERROR;
		int ret;
		const char* tmp;
		if ((tmp=strstr(key, "ttsid")) != NULL)
			ret = tts_set_option_id(handle, atoi(tmp+5), value);
		else
			ret = tts_set_option(handle, key, value);
		return ret;
    }

	DLL_PUBLIC
	int usc_tts_cancel(TTSHANDLE handle)
	{
		return tts_cancle(handle);
	}

#ifdef __cplusplus
}
#endif