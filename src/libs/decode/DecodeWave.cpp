#include "DecodeWave.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DEC_FRAME_IN_SIZE  640
#define DEC_FRAME_OUT_SIZE  640


CDecodeWave::CDecodeWave(void): CDecode()
{
	dec_frame_out_size= DEC_FRAME_OUT_SIZE;
	dec_frame_in_size= DEC_FRAME_IN_SIZE;
	is_start = false;
}


CDecodeWave::~CDecodeWave(void)
{
	destory();
}
bool CDecodeWave::init() {
	if( CDecode::init()) {
		is_start = false;
		return true;
	}
	return false;
}


int CDecodeWave::decode_frame(const unsigned char* raw_data, int raw_len, unsigned char* pcm_data, int pcm_buffer_size)
{
	if(!is_start) {
		is_start = true;
		if (raw_len > 44 && raw_data[0] == 'R' && raw_data[1] == 'I' && raw_data[2] == 'F' && raw_data[3] == 'F')
		{
			raw_data += 44;
			raw_len -= 44;
		}
	}
	memcpy(pcm_data,raw_data,raw_len);	
    return raw_len;
}


int CDecodeWave::write(char* raw_data, int rawDataLen) {
	return 0;
}

int CDecodeWave::decode(char* raw_data, int rawDataLen, char* pcm_data, int pcm_buffer_size) {

	
	return 0;


}

void CDecodeWave::destory() {
	

	if(isInit()) {
	
	}
}
