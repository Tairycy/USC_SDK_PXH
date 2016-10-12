#pragma once

#define FRAME_BYTE_SIZE  640
#define DECODE_SAMPLE_RATE_16K  16000
#define DECODE_SAMPLE_RATE_8K  8000


class CDecode
{
protected:
	char left[FRAME_BYTE_SIZE];
	int left_len;
	bool init_flag;
	// ”Ô“Ù÷ ¡ø
	int quality;
	int sample_rate;
	int dec_frame_out_size;
	int dec_frame_in_size;

public:
	CDecode(void);
	virtual int get_frame_out_size(){ return dec_frame_out_size; }
	virtual int get_frame_in_size(){ return dec_frame_in_size; }
	void set_frame_in_size(int size){ dec_frame_in_size = size; }

	int get_quality(){ return quality; }
	void set_quality(int value){ quality = value; }

	int get_sample_rate(){ return sample_rate; }
	void set_sample_rate(int sample){ sample_rate = sample; }


	virtual ~CDecode(void);

	virtual bool isDecodePCM() = 0;
	virtual int decode_frame(const unsigned char* spx, int spx_len, unsigned char* pcm_data, int pcm_buffer_size) = 0;
	virtual bool isInit();
	//	virtual int write(char* spx, int spx_len);
	//	virtual int read(char* pcm_data, int pcm_size);
	//	virtual void reset();
	virtual bool init();
	virtual void destory();
};


