#ifndef _ASR_SERVICE_CONTROL_BLOCK_H_
#define _ASR_SERVICE_CONTROL_BLOCK_H_

#include <string>
#include "BufferQueue.h"


#define KEY_MAX_SIZE 128
enum {
    INPUT_AUDIO_FORMAT_SPEEX_16K      = 1001,
    INPUT_AUDIO_FORMAT_PCM_16K        = 1002,
    INPUT_AUDIO_FORMAT_PCM_8K         = 1003,
    INPUT_AUDIO_FORMAT_AMRNB          = 1004,	
	INPUT_AUDIO_FORMAT_SPEEX_8K       = 1005,
	INPUT_AUDIO_FORMAT_SPEEX_OGG_16K  = 1007,
	INPUT_AUDIO_FORMAT_AMRNB_16K      = 1008,
	INPUT_AUDIO_FORMAT_USC_SPEEX_16K  = 1009,
	INPUT_AUDIO_FORMAT_USC_OPUS_16K   = 1010,
};

enum {
    USC_SAMPLE_RATE_8K                = 8000,
    USC_SAMPLE_RATE_16K               = 16000
};

#define GET_RESULT_INPUT_BUFFER_LEN        6400    // 500ms

#ifdef __cplusplus
extern "C" {
#endif

class asr_client;

// ~~~~~added by lqy begin~~~~~
class AsrNoiseFilter
{
private:

	// �Ƿ�Ϊ������ͷ
	bool is_speak_begin;

	bool is_use;

	int vad_cache_time;

public:

	AsrNoiseFilter();
	~AsrNoiseFilter();

	BufferQueue bf_queue;

	// �ж��Ƿ�Ϊ��ͷ
	bool isSpeakBegin();

	// ����������ͷ
	bool setSpeakBegin(bool flag);

	// ���û���ʱ��
	int setVadCacheTime(int time);

	// �õ�����ʱ��
	int getVadCacheTime();

	// �����Ƿ�����ǰ�Ͼ�
	bool setIsUse(bool flag);

	bool getIsUse();

};
// ~~~~~added by lqy end~~~~~


class AsrServiceControlBlock {
private:

	bool partial_result_enabled;
	bool asr_started;
	bool is_renew;

	int beginTime;
		
	// �Ƿ�ʹ��VAD�Ͼ�
	bool is_vad_enabled;
public:

	AsrNoiseFilter asr_noise_filter;
	std::string collected_info; 
	std::string lastResult;
	int frontSil;
	int backSil;
	int endTime;

	asr_client *asrService;

	int sampleRate;

	int result_buffer_size;
	int voice_type;
	int num;
	bool is_customed_address;

	bool isCustomedAddress();
	bool isOutMaxLength();
	bool isRenew() {
		return is_renew;
	}

	void setRenew(bool renew) {
		is_renew = renew;
	}

	void updateBeginTime();

	bool isAsrStarted() {
		return asr_started;
	}

		// �Ƿ�ʹ��VAD�Ͼ�
	bool isVadEnabled(){
		return 	is_vad_enabled;
	}

	// �Ƿ�ʹ��VAD�Ͼ�
	void setVadEnabled(bool enabled) {
		is_vad_enabled = enabled;
	}


	void setAsrStarted(bool started) {
		asr_started = started;
	}

	void setCustomedAddress(bool value) {
		is_customed_address = value;
	}

	void setServer(const char* ip,const unsigned short port);

	bool isStop();

	void setStop();

	void start();
	
	// ֹͣ��ǰʶ�𱣴��������¿�ʼʶ��
	int renew();

	void reset();

	void addDataLength( int length);

	bool setDecode(int type);

	void setBeginTime(int time);
	int getBeginTime();
	int getEndTime() ;

	~AsrServiceControlBlock();
	AsrServiceControlBlock();

	void setIsCustomAddress(int isCustom);            //设置是否自定义服务器地址
	int getCustomAddress();                           //获取是否自定义服务器地址
};


#ifdef __cplusplus
}
#endif

#endif