#include "DecodeUscOpus.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

CDecodeUscOpus::CDecodeUscOpus(void): CDecode()
{
   	dec_frame_out_size= 20;
	dec_frame_in_size= 20;
	sample_rate = DECODE_SAMPLE_RATE_16K;
}

CDecodeUscOpus::~CDecodeUscOpus(void) {
	destory();
}

bool CDecodeUscOpus::init() {
	CDecode::init();

	init_flag = true;

	return isInit();
}


int CDecodeUscOpus::decode_frame(const unsigned char* raw, int raw_len, unsigned char* out_data, int out_buffer_size)
{
	if(!isInit()) {
		printf("CDecodeUscOpus::decode_frame error !isInit() \n");
		return -1;
	}

	int i=0;
	unsigned char frame[FRAME_BYTE_SIZE];

	char* left_spx =(char*)left;
	for(i=0;i<left_len;i++) {
		frame[i]=left_spx[i];
	}
	for(int j=0;j<raw_len;j++){
		frame[i+j]=raw[j];
		left_spx[i+j] = raw[j];
	}
	left_len += raw_len;	
	if( left_len <= sizeof(int)) {
		return 0;
	}

	short *speexSize = (short*)frame;
	dec_frame_in_size = *speexSize + sizeof(short);
	if(dec_frame_in_size <= 0 || dec_frame_in_size > 250) {
		printf("CDecodeUscOpus::decode_frame data error \n");
		return -3;
	}


	if(left_len < (dec_frame_in_size)) {
		return 0;
	}	

	if( (dec_frame_in_size) > out_buffer_size) {
		printf("CDecodeUscOpus::decode_frame error left_len > out_buffer_size \n");
		return -2;
	}
	out_buffer_size = dec_frame_in_size;
	memcpy(out_data, frame, dec_frame_in_size);
	left_len -= dec_frame_in_size;
	memcpy(left_spx, &frame[dec_frame_in_size], left_len);

	if(left_len >= sizeof(short)) {
		speexSize = (short*)left_spx;
		dec_frame_in_size = *speexSize + sizeof(short);
		if(dec_frame_in_size <= 0 || dec_frame_in_size > 320) {
			dec_frame_in_size = 30;
			printf("CDecodeUscOpus::decode_frame data error \n");
			return -3;
		}
	}

	return out_buffer_size;
}


int CDecodeUscOpus::decode(unsigned char* raw_data, int rawDataLen, unsigned char* out_data, int out_size) {

	
	return 0;

}

void CDecodeUscOpus::destory() {
	
	CDecode::destory();

	if(isInit()) {
		init_flag = false;
	}
}
