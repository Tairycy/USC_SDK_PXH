#pragma once
#include "Decode.h"


class CDecodeUscOpus: public CDecode
{
private:
public:
	CDecodeUscOpus(void);
	~CDecodeUscOpus(void);
	 bool isDecodePCM() { return false;};
	 int decode_frame( const unsigned char* spx, int spx_len, unsigned char* pcm_data, int pcm_buffer_size);
	 bool init();
	 int decode(unsigned char* raw_data, int rawDataLen,unsigned char* pcm_data, int pcm_buffer_size);
	 void destory();
};

