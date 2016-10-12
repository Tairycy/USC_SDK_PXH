#include "asr_client.h"
#include "set_option_key.h"
#include <iostream>

static int TranslateInt(const char* value);
static int ReplaceString(std::string& param, std::string pat, std::string change);

asr_client::asr_client()
	:m_opus(NULL), m_bUseDecode(true)
{
	m_serAsr.SetSerParam("sampleRate", "16k");
	m_serAsr.SetSerParam("audioCodec", "opus");
	m_opus.change_mode(Opus::WB_MODE, Opus::WB_MODE);

	package_len_threshold = PACKAGE_LEN_THRESHOLD;
	//Reset();
	vadTimeout = 300;
	maxSpeechSecond = 60;

	//m_strAns = "";

#ifdef DETECT_SPEECH_START
	dataBuf = NULL;
	dataLen = 0;
	dataBuf2 = NULL;
	dataLen2 = 0;
#endif
}

void asr_client::Reset()
{
#ifdef USE_VAD
	m_vad.init();
#endif

	usc_volume = 0;
	compress = 8;
	enableVAD = 0;

	audioBuf[0] = 0;
	bufLen = 0;
	bufferedPcmLen = 0;
	totalPcmLen = 0;
}

asr_client::~asr_client()
{

}

void asr_client::SetServer(const char* ip, const unsigned short port)
{
	m_serAsr.SetServer(ip, port);
}

int asr_client::Login()
{
	return m_serAsr.Login();
}

int asr_client::Logout()
{
	return m_serAsr.Logout();
}

int asr_client::Cancel()
{
	return m_serAsr.Cancel();
}

int asr_client::Start()
{
	m_opus.reset();
	Reset();
	return m_serAsr.Start();
}

const char* asr_client::GetResult()
{
	int errorCode = 0;
	int length = 0;
	return m_serAsr.GetResult(&errorCode, & length);
}

int asr_client::QueryResult()
{
	int tmp = m_serAsr.QueryResult();
	if (tmp > 0)
		return 2;
	return 0;
}

int asr_client::SendRemainData()
{
	if (bufLen > 0)
	{
		int status = m_serAsr.Probe(audioBuf, bufLen, true);
		return status;
	}
	return 0;
}

int asr_client::Stop()
{
	int err = SendRemainData();
	if (err < 0)
	{
		char err_msg[128];
		sprintf(err_msg, "asrclient: send remain error = %d", err);
		LOGMYE("ASR-JNI", err_msg, 0);
		return err;
	}
	err = m_serAsr.Stop();
	if (err != 0)
	{
		char err_msg[128];
		sprintf(err_msg, "asrclient: stop error = %d", err);
		LOGMYE("ASR-JNI", err_msg, 0);
		return err;
	}
#ifdef USE_VAD
		m_vad.reset();
#endif
	return err;
}

int asr_client::DoCompress(char* pcm, int pcm_len)
{
	int codec_len = 0;
	int encode = 0;
	if (m_bUseDecode)
		encode = m_opus.encode(pcm, pcm_len, (unsigned char*)audioBuf + bufLen, &codec_len);
	else{
		codec_len = pcm_len;
		memcpy(audioBuf + bufLen, pcm, pcm_len);
	}
	char msg[128] = { 0 };
	sprintf(msg, "pcm_len : %d, data_len : %d", pcm_len, codec_len);
	LOGMYI("DoCompress", msg, 3);
	if (encode < 0)
		return -1;
	bufLen += codec_len;
	return bufLen;
}

int asr_client::IsActive(char* pcm, int pcm_len)
{
#ifdef USE_VAD
		return m_vad.process((short*)pcm, pcm_len / sizeof(short), &usc_volume);
#endif
	return USC_VAD_HOLDING;
}

int asr_client::Recognizer(char*pcm, int pcm_len)
{
	int rt = 0;
	if (totalPcmLen > maxSpeechSecond*ONE_SECOND)
	{
		LOGMYE("ASR-JNI", "asrclient: max speech", 1);
		return MAX_SPEECH_TIMEOUT;
	}
	totalPcmLen += pcm_len;
	rt = ResumeSession(pcm, pcm_len);
	if (rt < 0)
	{
		char err_msg[128] = { 0 };
		sprintf(err_msg, "asrclient: resume error = %d", rt);
		LOGMYE("ASR-JNI", err_msg, 0);
	}
	return rt;
}

int asr_client::ResumeSession(char* pcm, int pcm_len)
{
	int ret = 0;
	int buffer_len = DoCompress(pcm, pcm_len);
	if (buffer_len < 0)
		return COMPRESS_PCM_ERROR;

	bufferedPcmLen += pcm_len;
	//printf("bufferedPcmLen : %d, package_len_threshold : %d\n", bufferedPcmLen, package_len_threshold);
	if (bufferedPcmLen >= package_len_threshold)
	{
		ret = m_serAsr.Probe(audioBuf, bufLen);
		audioBuf[0] = 0;
		bufLen = 0;
		bufferedPcmLen = 0;
	}
	return ret;
}

int asr_client::SetValueInt(int id, int value)
{
	int status = 0;
	char tmp[64] = { 0 };
	memset(tmp, 0x00, sizeof(tmp));
	sprintf(tmp, "%d", value);
	switch (id)
	{
	case ASR_VAD_ENABLED:
		if (value >= 0 && value <= 1)
			enableVAD = value;
		break;
	case ASR_VAD_TIMEOUT_ID:
		if (value >= 2000 && value <= 10000)
			vadTimeout = value / 10;
		break;
	case ASR_MAX_SPEECH_TIMEOUT_ID:
		if (value >= 10 && value <= 600)
			maxSpeechSecond = value;
		break;
	case ASR_PCM_COMPRESS_ID:
		if (value >= 0 && value <= 10)
			compress = value;
		break;
	case ASR_NET_PACKAGE_SIZE:
		package_len_threshold = value;
		break;
	case ASR_OPT_USE_DECODE:
		m_bUseDecode = (bool)value;
		break;
	case ASR_OPUS_BITRATE:
		m_opus.set_bitrate(value);
		break;
	default:
		status = m_serAsr.SetValueInt(id, value);
		break;
	}
	return status;
}

int asr_client::SetValueString(int id, const char* s)
{
	int status = USC_OK;
	int type;
	if (s == NULL)
		return OPTION_PARAM_ERROR;
	std::string tmp;
	int flag;
	switch (id)
	{
	case ASR_AUDIO_ENCODE_MTD:
		if (strcmp("opus", s) == 0)
		{
			m_serAsr.SetSerParam("sampleRate", "16k");
			m_serAsr.SetSerParam("audioCodec", "opus");
		}
		else if (strcmp("mp3", s) == 0)
		{
			m_serAsr.SetSerParam("sampleRate", "16k");
			m_serAsr.SetSerParam("audioFormat", "audio/mpeg");
			m_serAsr.SetSerParam("audioCodec", "raw");
		}
		else if (strcmp("pcm", s) == 0)
		{
			m_serAsr.SetSerParam("sampleRate", "16k");
			m_serAsr.SetSerParam("audioFormat", "pcm");
			m_serAsr.SetSerParam("audioCodec", "opus");
		}
		else
		{
			m_serAsr.SetSerParam("sampleRate", "8k");
			m_serAsr.SetSerParam("audioCodec", "opus-nb");
		}
		type = TranslateInt(s);
		status = m_opus.change_mode(Opus::WB_MODE, type);
		break;

	case ASR_AUDIO_ENCODE_MTD8K:
		/* the sample rate is restricted to 8K */
		if (strcmp("pcm", s) == 0)
		{
			m_serAsr.SetSerParam("sampleRate", "8k");
			m_serAsr.SetSerParam("audioFormat", "pcm");
			m_serAsr.SetSerParam("audioCodec", "opus");
		}
		else
		{
			m_serAsr.SetSerParam("sampleRate", "8k");
			m_serAsr.SetSerParam("audioCodec", "opus-nb");
		}
		status = m_opus.change_mode(Opus::NB_MODE, Opus::NB_MODE);
		break;
	case OPT_ENGINE_PARAMETER:
	case OPT_ENGINE_PARAMETER_2:
		tmp = s;
		tmp += ";";
		ReplaceString(tmp, "\n", ";");
		ReplaceString(tmp, ":", "=");
		flag = tmp.rfind("sampleRate");
		if (flag >= 0)
		{
			int eg_flag = tmp.find("=", flag);
			if (eg_flag < flag)
				return OPTION_PARAM_ERROR;
			int end_flag = tmp.find(";", flag);
			if (end_flag < eg_flag)
				return OPTION_PARAM_ERROR;
			eg_flag++;
			std::string value = tmp.substr(eg_flag, end_flag - eg_flag);
			if (value.compare("16k") == 0)
			{
				SetValueString(ASR_AUDIO_ENCODE_MTD, "opus");
			}
			else if (value.compare("16kto8k") == 0)
			{
				SetValueString(ASR_AUDIO_ENCODE_MTD, "opus-nb");
			}
			else if (value.compare("8k") == 0)
			{
				SetValueString(ASR_AUDIO_ENCODE_MTD8K, "");
			}
			tmp.erase(flag, end_flag - flag + 1);
		}
		m_serAsr.DealEngineParam(tmp);
		break;
	default:
		status = m_serAsr.SetValueString(id, s);
		break;
	}
	return status;
}

const char* asr_client::GetOptionValue(int id)
{
	return m_serAsr.GetOptionValue(id);
}

int asr_client::SetSerParam(char* key, char* value)
{
	return m_serAsr.SetSerParam(key, value);
}

int asr_client::SetTraParam(char* key, char* value)
{
	return m_serAsr.SetTraParam(key, value);
}

int asr_client::setVadSilTime(int frontSil, int backSil)
{
#ifdef USE_VAD
	m_vad.setVadSilTime(frontSil, backSil);
#endif
	return 0;
}

int asr_client::setVadParas(int id, const void *value)
{
#ifdef USE_VAD
	m_vad.setparas(id, value);
#endif
	return 0;
}

int asr_client::GetLastError()
{
	return m_serAsr.GetLastError();
}

asr_client* asrCreateAsrService(const char* ip, const unsigned short port)
{
	asr_client* asr_service = new asr_client();
	if (ip != NULL && port != 0)
		asr_service->SetServer(ip, port);
	if (asr_service == NULL)
	{
		LOGMYE("ASR-CLIENT", "ASR memory allocation error", 0);
		return NULL;
	}
	return asr_service;
}

void asrDestroyAsrService(void* asrService)
{
#ifdef win32
	wsacleanup();
#endif
	if (asrService == NULL)
		return;
	asr_client* asr_service = (asr_client*)asrService;
	delete asr_service;
}


static int TranslateInt(const char* value)
{
	int result = 0;
	if (!strcmp(value, "opus"))
	{
		result = Opus::WB_MODE;
	}
	else if (!strcmp(value, "opus-nb"))
	{
		result = Opus::NB_MODE;
	}
	else if (!strcmp(value, "speex"))
	{
		result = 0;
	}
	return result;
}

static int ReplaceString(std::string& param, std::string pat, std::string change)
{
	std::string::size_type pos = 0;
	while ((pos = param.find(pat)) != std::string::npos)
	{
		param.replace(pos, pat.size(), change);
	}
	return 0;
}
