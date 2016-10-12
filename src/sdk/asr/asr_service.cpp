#include "asr_service.h"
#include "function.h"


asr_service::asr_service()
{
	m_sdkReq->Init(USC_ASR_SER);
}

asr_service::~asr_service()
{

}

int asr_service::Stop()
{
	int ret = m_sdkReq->EndRequest();
	SetLastError(m_sdkReq->GetLastError());
	return ret;
}