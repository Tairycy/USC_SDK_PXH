#ifndef _OPUS_WRAPPER_H
#define _OPUS_WRAPPER_H


#include <stdio.h>
#include "opus.h"

// #define OPUS_SAVE_PCM

#ifdef __cplusplus
extern "C" {
#endif

enum { WB_FRAME_SIZE=320, NB_FRAME_SIZE=160 };

class Opus{
	public:
		static const int WB_MODE = 1;
		static const int NB_MODE = 2;
	
	public:
		Opus( int in_mode=WB_MODE,int out_mode=WB_MODE );
		~Opus();
		
		int set_mode(int in_mode,int out_mode);
		int change_mode(int in_mode,int out_mode);

		// 2014-03-05 zhangkaijun
		void set_bitrate(int bitrate);

#ifdef _ENCODE_ONLY_
		int encode(char* in,int len,unsigned char* opus,int* opus_len);
		//int encode(char* in,int len,vector<unsigned char>& opus);
		int encode_frame(char* in,unsigned char* cbits);
#else
		int decode(char* in,int len,short** speech,int* speech_len);
		int decode_frame(char* in,int* len,short* frame,int* frame_len);
#endif
		
		void reset();
	private:
		// 2014-03-05 zhangkaijun
		int bitrate_16;

#ifdef OPUS_SAVE_PCM
		FILE *file_log_pcm;
#endif
		//int sample_rate;
		int frame_size;
		int buf_len;
		char buffer[WB_FRAME_SIZE*2];
		int current_in_mode; 
		int current_out_mode; 
#ifdef _ENCODE_ONLY_
		OpusEncoder* enc;
#else
		OpusDecoder* dec;
		short dec_head_size;
#endif
		void release_opus();
};

#ifdef __cplusplus
}
#endif

#endif

