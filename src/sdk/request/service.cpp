#include "http_def.h"
#include "service.h"
#include "set_option_key.h"
#include "log.h"
#include "function.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int UniServiceInterface::Login()
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	int ret = m_sdkReq->Login();
	SetLastError(m_sdkReq->GetLastError());
	return ret;
}

int UniServiceInterface::Start(const char * textdata, unsigned int textlen)
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	int status = USC_OK;
	if (!m_sdkReq->IsLogin() && m_sdkReq->m_strToken.length() < TOKEN_LEN_MIN)
	{
		LOGMYW("SERVICE", "no login", 1);
		status = m_sdkReq->Login();
		if (status != USC_OK)
		{
			LOGMYE("SERVICE", "first login error!", 0);
			SetLastError(m_sdkReq->GetLastError());
			return status;
		}
	}
	status = m_sdkReq->StartRequest((char*)textdata, textlen);
	
	if (status < -90000 && status > -91200)
	{
		char err_msg[128] = { 0 };
		sprintf(err_msg, "first start error! error code : %d", status);
		SetLastError(m_sdkReq->GetLastError());
		LOGMYE("SERVICE", err_msg, 1);
		status = m_sdkReq->StartRequest((char*)textdata, textlen);
		SetLastError(m_sdkReq->GetLastError());
	}
	
	return status;
}

const char* UniServiceInterface::GetResult(int * errorRet, int * retLength)
{
	if (m_sdkReq == NULL)
		return "";
	return m_sdkReq->GetResult(errorRet, retLength);
}

int UniServiceInterface::QueryResult()
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	return m_sdkReq->HaveResult();
}

int UniServiceInterface::Probe(char*data, int data_len, bool is_last)
{
	char msg[128] = { 0 };
	sprintf(msg, "data_len : %d", data_len);
	LOGMYI("Probe", msg, 3);
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	int ret = m_sdkReq->PutRequest(data, data_len, is_last);
	SetLastError(m_sdkReq->GetLastError());
	return ret;
}

int UniServiceInterface::Cancel()
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	return m_sdkReq->CancelRequest();
}

int UniServiceInterface::Logout()
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	int ret = m_sdkReq->Logout();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int UniServiceInterface::SetSerParam(char* key, char* value)
{
	char msg[512] = { 0 };
	sprintf(msg, "key : %s, value : %s", key, value);
	LOGMYI("SetSerParam", msg, 3);
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	return m_sdkReq->SetSerParamater(key, value);
}

int UniServiceInterface::SetTraParam(char* key, char* value)
{
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	return m_sdkReq->SetTraParamater(key, value);
}

void UniServiceInterface::SetSleepTimeMS(int timeMS)
{
	if (m_sdkReq == NULL)
		return;
	m_sdkReq->SetSleepTime(timeMS);
}

int UniServiceInterface::SetValueString(int id, const char* s)
{
	int status = USC_OK;
	if (s == NULL)
		return OPTION_PARAM_ERROR;
	if (m_sdkReq == NULL)
		return NO_HANDLE_ERROR;
	switch (id)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~asr start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case OPT_ENGINE_PARAMETER:
		DealEngineParam(s);
		break;

	case ASR_MODLE_TYPE:
		m_sdkReq->SetSerParamater("modelType", s);
		break;

	case USC_OPT_SET_CANCEL:
		m_sdkReq->CancelRequest();
		break;

	case ASR_RESULT_PARAMETER:
	case ASR_RESULT_PARAMETER_2:
		m_sdkReq->SetSerParamater("variable", "true");
		break;

	case ASR_PUNCTUATION_ENABLE:
		m_sdkReq->SetSerParamater("punctuated", s);
		break;

	case ASR_RESULT_FORMAT_ID:
		m_sdkReq->SetSerParamater("textFormat", s);
		break;

	case ASR_LANGUAGE_SELECT:
		m_sdkReq->SetSerParamater("lang", s);
		break;
	case OPT_LOG_FILE:
		LOG::SetLogWriteFile(s);
		break;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~asr end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case ASR_TRAFFIC_PARAMETER:
		m_sdkReq->SetSerParamater("textFormat", "json");
		status = m_sdkReq->SetOptionValue(ASR_TRAFFIC_PARAMETER, s);
		break;

	case OPT_COLLECTED_INFO:
	case OPT_USER_ID:
	case OPT_APP_KEY:
	case OPT_IMEI_ID:
	case OPT_UDID_ID:
	case OPT_REQUEST_TIMEOUT:
	case OPT_REQUEST_SLEEP_TIME:
	case OPT_USE_MD5:
	case OPT_SECRET:
	case OPT_CRYPT_VERSION:
	case OPT_SET_TOKEN:
		status = m_sdkReq->SetOptionValue(id, s);
		break;

	default:
		status = OPTION_ID_ERROR;
		break;
	}
	return status;
}

int UniServiceInterface::SetValueInt(int id, int value)
{
	int status = 0;
	char tmp[64] = { 0 };
	memset(tmp, 0x00, sizeof(tmp));
	sprintf(tmp, "%d", value);
	switch (id)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~asr start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case ASR_PARTIAL_RESULT_ENABLED:
	case ASR_WAITING_RESULT_TIMEOUT_ID:
		if (value>SERVICE_TIMEOUT_LIMIT || value<0)
		{
			status = OPTION_PARAM_ERROR;
		}
		else
		{
			sprintf(tmp, "%d", value);
			status = m_sdkReq->SetOptionValue(OPT_REQUEST_TIMEOUT, tmp);
		}
		break;
	case OPT_LOG_SWITCH:
		LOG::SetLogLevel(value);
		break;
	case OPT_LOG_WRITE:
		LOG::SetLogWrite(value);
		break;
	case SERVICE_STATUS_SELECT:
		status = m_sdkReq->SetServiceStatus(value);
		break;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~asr end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case ASR_REQ_SPEAKER_INFO:
		//status = asr_session->setSessionAttrs(SERV_SUP_ATTR::SSUP_REQ_SPEAKER_INFO, tmp);
		break;
	case OPT_DEVICE_OS:
		//status = m_sdkReq->SetOptionValue(OPT_DEVICE_OS, tmp);
		break;
	case OPT_CARRIER:
		//status=asr_session->setSessionAttrs(SERV_SUP_ATTR::SSUP_OPT_CARRIER, tmp);
		break;
	case OPT_NETWORK_TYPE:
		//status=asr_session->setSessionAttrs(SERV_SUP_ATTR::SSUP_OPT_NETWORK_TYPE, tmp);
		break;
	case ASR_SCENE_CONTEXTID:
		//status = asr_session->setSessionAttrs(SERV_SUP_ATTR::SSUP_SCENE_CONTEXTID, tmp);
		break;
	default:
		return OPTION_ID_ERROR;
	}
	return status;
}

const char* UniServiceInterface::GetOptionValue(int id)
{
	if (m_sdkReq == NULL)
		return "";
	switch (id)
	{
	case OPT_LAST_SOCKET_ERROR:
		memset(m_cLastError, 0, sizeof(m_cLastError));
		sprintf(m_cLastError, "%d", GetLastError());
		return m_cLastError;
	default:
		return m_sdkReq->GetOptionValue(id);
	}
}

void UniServiceInterface::SetServer(const char* ip, const unsigned short port)
{
	if (m_sdkReq == NULL)
		return;
	char c_port[20] = { 0 };
	sprintf(c_port, "%d", port);
	m_sdkReq->SetAddress((char*)ip, c_port);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool UniServiceInterface::DealEngineParam(std::string param)
{
	int begin_flag = 0, end_flag;
	while ((end_flag = param.find("=", begin_flag)) > 0)
	{
		std::string header = param.substr(begin_flag, end_flag - begin_flag);
		begin_flag = end_flag + 1;
		if ((end_flag = param.find(";", begin_flag)) < 0)
			break;
		std::string value = param.substr(begin_flag, end_flag - begin_flag);
		begin_flag = end_flag + 1;

		m_sdkReq->SetSerParamater(header.c_str(), value.c_str());
	}
	return true;
}

void UniServiceInterface::AddLog(char* tag, char* msg)
{
	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~