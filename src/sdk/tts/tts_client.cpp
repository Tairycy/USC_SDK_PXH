#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tts_client.h"
#include "errorNo.h"
#include "http_def.h"
#include "tts_service.h"
#include "log.h"
#include "set_option_key.h"

using namespace std;

#ifdef __cplusplus
extern "C"{
#endif

// 公有云创建合成接口实例    
int tts_create_service(TTSHANDLE* handle)
{
	tts_service* tts_request = new tts_service();
	tts_request->SetServer(U_HTTP_HOST, atoi(U_HTTP_PORT));
	// tts sleep time default is 80 ms.
	tts_request->SetSleepTimeMS(80);
    *handle = (TTSHANDLE)tts_request;
    return USC_OK;
}

// 私有云创建合成接口实例    
int tts_create_service_ext(TTSHANDLE* handle, const char *host, const char *port)
{
    if (host == NULL || port == NULL)
		return SER_IP_ADDRESS_ERROR;
	tts_service* tts_request = new tts_service();
	tts_request->SetServer(host, atoi(port));
	// tts sleep time default is 80 ms.
	tts_request->SetSleepTimeMS(80);
	*handle = (TTSHANDLE)tts_request;
	return USC_OK;
}

int tts_start_synthesis(TTSHANDLE handle)
{
    if (0 == handle)
    {
        return NO_HANDLE_ERROR;
    }
	tts_service* tts_request = (tts_service*)handle;
    return tts_request->Login();
}

// 上传文本数据进行合成    
int tts_text_put(TTSHANDLE handle, const char * textdata, unsigned int textlen)
{
    if (0 == handle)
    {
        return NO_HANDLE_ERROR;
    }
    if (textdata == NULL || textlen == 0)
    {
        return TEXT_NULL_ERROR;
    }
    if (textlen >= tts_service::SYNTH_TEXT_LEN_LIMIT || strlen(textdata) >= tts_service::SYNTH_TEXT_LEN_LIMIT)
    {
        return TEXT_TOO_LONG_ERROR;
    }
    int status = -1;
    tts_service* tts_request = (tts_service*)handle;
	tts_request->m_nStatus = 0;
	status = tts_request->Start(textdata, textlen);
	if (tts_request->m_sdkReq->m_nProbeNum4TTS < 0 && status >= 0){
		status = tts_request->RecvOneResponse();
		if (status >= 0)
			status = 0;
	}
    return status;
}

// 获取合成结果    
const void* tts_get_result(TTSHANDLE handle, unsigned int* audioLen, int* synthStatus, int* errorCode)
{
	//bool isLast = false;
	//if (*synthStatus == 1)
	//	isLast = true;
    *audioLen = 0;
    *errorCode = USC_OK;
    *synthStatus = USC_SYNTH_PROCESS_ERROR;
    if (handle == 0)
    {
        *errorCode = NO_HANDLE_ERROR;
        return NULL;
    }
    tts_service* tts_request = (tts_service*)handle;
	int status = 0;

	switch (tts_request->m_nStatus)
	{
	case 0:
		status = tts_request->Probe();
		if (status == 2)
			tts_request->m_nStatus = 1;
		if (status != 4)
			status = 1;
		break;
	case 1:
		status = tts_request->TtsEndRequest();
		tts_request->m_nStatus = 2;
		break;
	case 2:
		status = tts_request->RecvOneResponse();
		break;
	default:
		status = -671;
		break;
	}
	// 4 is cancel
    if (status < 0 || status == 4)
    {
        *errorCode = status;
        return NULL;
    }
    const char* reData = NULL;
    int getLen = 0;

	if ((getLen = tts_request->QueryResult()) > 0)
    {
        unsigned int dstLen = 0;
        char *dstBuf = NULL;
        int errNo = 0;
        unsigned int dstBytesLen = 0;

		char * getCh = (char*)tts_request->GetResult(&errNo, &getLen);
        if (errNo != USC_OK)
        {
            char temp[128];
            sprintf(temp,"get result error code= %d", errNo);
            LOGMYE("TTS-CLIENT", temp, 0);
            *errorCode = errNo;
            return NULL;
        }
		if (tts_request->m_strCodec.compare("opus") == 0)
		{
			errNo = tts_request->m_opus.decode(getCh, getLen, &(dstBuf), audioLen);
			if (uni::Opus::SUCCESS != errNo)
			{
				char temp[128];
				sprintf(temp, "decode error code= %d, getLen= %d, audioLen=%d.", errNo, getLen, *audioLen);
				LOGMYE("TTS-CLIENT", temp, 0);
				*errorCode = DECODE_ERROR;
				return NULL;
			}
		}
		else
		{
			*audioLen = getLen;
			dstBuf = getCh;
		}
        if (NULL != dstBuf)
        {
            reData = dstBuf;
        }
    }
    *synthStatus = status;
    return reData;
}

// 停止合成    
int tts_stop_synthesis(TTSHANDLE handle)
{
    if (0 == handle)
        return NO_HANDLE_ERROR;
    tts_service* tts_request = (tts_service*)handle;
    int status = tts_request->Stop();
    if (status < 0)
    {
        return status;
    }
    return USC_OK;
}

// 释放合成接口实例    
int tts_release_service(TTSHANDLE handle)
{
    if (0 == handle)
        return NO_HANDLE_ERROR;
    tts_service* tts_request = (tts_service*)handle;
    int status = 0;
    if (tts_request != NULL)
    {
        status = tts_request->Logout();
        delete tts_request;
        tts_request = NULL;
    }
    if (status < 0)
    {
        return status;
    }
    return USC_OK;
}

const char* tts_get_option(TTSHANDLE handle, int id, int* errorCode)//得到扩展属性
{
    if (0 == handle) {
        *errorCode = NO_HANDLE_ERROR;
        return "";
    }        
    tts_service* tts_request = (tts_service*)handle;
	// 手机端接口，获取最后一次结果
	if (id == USC_GET_TTS_LAST_RESULT)
	{
		unsigned int len = 0;
		int ret = 0, synthStatus = 1;
		const char* tmp = (const char*)tts_get_result(handle, &len, &synthStatus, &ret);
		if (ret == 0)
			return tmp;
		else
			return "";
	}
	return tts_request->GetOptionValue(id);
}

const char* tts_get_version(TTSHANDLE handle, int* errorCode)//得到版本号
{
	return V_SDK_VERSION;
}

int tts_set_option(TTSHANDLE handle, const char* key, const char* value)
{
    if (0 == handle) {
        return NO_HANDLE_ERROR;
    }
    tts_service* tts_request = (tts_service*)handle;
    if (strcmp(key, "appkey") == 0)
    {
		return tts_request->SetValueString(OPT_APP_KEY, value);
    }
	else if (strcmp(key, "audioCodec") == 0)
	{
		tts_request->m_strCodec = value;
	}
	else if (strcmp(key, "slp") == 0)
	{
		return tts_request->SetValueString(OPT_REQUEST_SLEEP_TIME, value);
	}
	else if (strcmp(key, "md5") == 0)
	{
		return tts_request->SetValueString(OPT_USE_MD5, value);
	}
	else if (strcmp(key, "secret") == 0)
	{
		return tts_request->SetValueString(OPT_SECRET, value);
	}
	else if (strcmp(key, "cryptVersion") == 0)
	{
		return tts_request->SetValueString(OPT_CRYPT_VERSION, value);
	}
	else if (strcmp(key, "logLevel") == 0)
	{
		int int_value = atoi(value);
		return tts_request->SetValueInt(OPT_LOG_SWITCH, int_value);
	}
	else if (strcmp(key, "logOutput") == 0)
	{
		return tts_request->SetValueString(OPT_LOG_FILE, value);
	}
	else if (strcmp(key, "logSwitch") == 0)
	{
		int int_value = atoi(value);
		return tts_request->SetValueInt(OPT_LOG_WRITE, int_value);
	}
	else if (strcmp(key, "token") == 0)
	{
		return tts_request->SetValueString(OPT_CRYPT_VERSION, value);
	}
	else if (strcmp(key, "cancel") == 0)
	{
		return tts_request->SetValueString(USC_OPT_SET_CANCEL, value);
	}
	return tts_request->SetSerParam((char*)key, (char*)value);
}

int tts_set_option_id(TTSHANDLE handle, int key, const char* value)
{
    if (0 == handle) {
        return NO_HANDLE_ERROR;
    }
    tts_service* tts_request = (tts_service*)handle;
	return tts_request->SetValueString(key, value);
}

int tts_cancle(TTSHANDLE handle)
{
	if (0 == handle) {
		return NO_HANDLE_ERROR;
	}
	tts_service* tts_request = (tts_service*)handle;
	return tts_request->Cancel();
}

#ifdef __cplusplus
}
#endif