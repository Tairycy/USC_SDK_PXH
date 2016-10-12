#ifndef __TTS__SERVICE__
#define __TTS__SERVICE__

#include "service.h"
#include "uni_opus_decoder.h"
#include <string>


class tts_service : public UniServiceInterface
{
public:
    tts_service();
    ~tts_service();

    static const unsigned int SYNTH_PARAMS_LIMIT = 1024;
    static const unsigned int SYNTH_TEXT_LEN_LIMIT = 1024;

public:
    uni::Opus   	m_opus;			        	// opus
	std::string		m_strCodec;
	int				m_nStatus;

public:
	int				SetValueString(int id, const char* s);

public:

	// tts, send tts normal end request.
	int				TtsEndRequest(char* buf = NULL, int* bytesRead = 0);

	// tts, get last normal end response.
	int				Stop();

	// tts, get one response.
	int				RecvOneResponse();

};

#endif