#include "Decode.h"
#
#include <assert.h>



CDecode::CDecode(void)
{
	dec_frame_out_size= FRAME_BYTE_SIZE;
	dec_frame_in_size= FRAME_BYTE_SIZE;
	sample_rate = DECODE_SAMPLE_RATE_16K;

	init_flag = false;
	left[0]=0;
	left_len=0;
}


CDecode::~CDecode(void)
{
	destory();
}

bool CDecode::init() {

	destory();

	return true;
}



bool CDecode::isInit() {
	return init_flag;
}

void CDecode::destory() {
	

}



/*
int CDecode::write(char* raw_data, int rawDataLen) {

}
*/