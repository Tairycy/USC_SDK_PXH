#ifndef __ASR_CLIENT_INCLUDED__
#define __ASR_CLIENT_INCLUDED__

#include "asr_service.h"
#include "LibVad.h"
#include "Opuswrapper.h"

#define ONE_SECOND 32000

class asr_client
{
public:
	asr_client();
	~asr_client();

public:
	Opus				m_opus;
	asr_service			m_serAsr;
#ifdef USE_VAD
	usc::LibVAD			m_vad;
#endif

public:
	int					usc_volume;

public:
	static const int	PACKAGE_LEN_THRESHOLD = 9600;	//300ms
	int					package_len_threshold;
	
	char				audioBuf[3200*3+10];
	int					bufLen;
	int					bufferedPcmLen;
	int					totalPcmLen;
	int					vadTimeout;
	int					maxSpeechSecond;

	int					compress;
	int					enableVAD;

	bool				m_bUseDecode;

	//std::string			m_strAns;

public:

	void				Reset();
	void				SetServer(const char* ip, const unsigned short port);

	int					Login();
	int					Start();
	int					Recognizer(char*pcm, int pcm_len);
	int					Stop();
	int					Logout();

	int					Cancel();

	const char*			GetResult();
	int					QueryResult();

	int					SendRemainData();
	int					DoCompress(char* pcm, int pcm_len);
	int					IsActive(char* pcm, int pcm_len);
	int					ResumeSession(char* pcm, int pcm_len);

	int					setVadSilTime(int frontSil, int backSil);
	int					setVadParas(int id, const void *value);

	int					SetValueInt(int id, int value);
	int					SetValueString(int id, const char* s);

	const char*			GetOptionValue(int id);
	int					SetSerParam(char* key, char* value);
	int					SetTraParam(char* key, char* value);

	int					GetLastError();
};

asr_client* asrCreateAsrService(const char* ip=NULL, const unsigned short port=0);
void asrDestroyAsrService(void* asrService);

#endif