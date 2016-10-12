#pragma once

#include "Decode.h"

class CDecode;
class CDecodeWave: public CDecode
{
private:
	bool is_start;
public:
	CDecodeWave(void);
	~CDecodeWave(void);

	 bool isDecodePCM() { return true;};
	int write(char* spx, int spx_len);
	int read(short* pcm_data, int pcm_size);
	 int decode_frame(const unsigned char* raw_data, int rawDataLen, unsigned char* pcm_data, int pcm_buffer_size);
	int decode(char* raw_data, int rawDataLen, char* pcm_data, int pcm_buffer_size);
	 bool init();
	 void destory();
};

