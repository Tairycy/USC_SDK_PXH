#include "tts_service.h"
#include "url_code.h"
#include "http_def.h"
#include "errorNo.h"
#include "function.h"

tts_service::tts_service()
	: m_opus(uni::Opus::WB_MODE, false), m_nStatus(0)
{
    m_sdkReq->Init(USC_TTS_SER);
	m_strCodec = "opus";
    m_sdkReq->SetSerParamater("audioCodec", "opus");
    m_sdkReq->SetSerParamater("audioFormat", "audio/x-wav;");
}

tts_service::~tts_service()
{
}

int tts_service::SetValueString(int id, const char* s)
{
	return UniServiceInterface::SetValueString(id, s);
}

int tts_service::Stop()
{
	// socket reset
	//int ret = m_sdkReq->RecvOneResponse();
	int ret = m_sdkReq->CloseFd();
	// if no resp , because of there is no resp...
	SetLastError(m_sdkReq->GetLastError());
	if (ret == GET_NO_RESPONSE_ERROR)
		ret = 0;
	return ret;
}

int tts_service::TtsEndRequest(char* buf, int* bytesRead)
{
	int ret = m_sdkReq->TtsEndRequest();
	SetLastError(m_sdkReq->GetLastError());
	return ret;
}

int tts_service::RecvOneResponse()
{
	int ret = m_sdkReq->RecvOneResponse();
	SetLastError(m_sdkReq->GetLastError());
	return ret;
}
