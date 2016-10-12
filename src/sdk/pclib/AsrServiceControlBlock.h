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

	// 是否为声音开头
	bool is_speak_begin;

	bool is_use;

	int vad_cache_time;

public:

	AsrNoiseFilter();
	~AsrNoiseFilter();

	BufferQueue bf_queue;

	// 判断是否为开头
	bool isSpeakBegin();

	// 设置声音开头
	bool setSpeakBegin(bool flag);

	// 设置缓存时间
	int setVadCacheTime(int time);

	// 得到缓存时间
	int getVadCacheTime();

	// 设置是否启用前断句
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
		
	// 是否使用VAD断句
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

		// 是否使用VAD断句
	bool isVadEnabled(){
		return 	is_vad_enabled;
	}

	// 是否使用VAD断句
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
	
	// 停止当前识别保存结果，重新开始识别
	int renew();

	void reset();

	void addDataLength( int length);

	bool setDecode(int type);

	void setBeginTime(int time);
	int getBeginTime();
	int getEndTime() ;

	~AsrServiceControlBlock();
	AsrServiceControlBlock();

	void setIsCustomAddress(int isCustom);            //璁剧疆鏄惁鑷畾涔夋湇鍔″櫒鍦板潃
	int getCustomAddress();                           //鑾峰彇鏄惁鑷畾涔夋湇鍔″櫒鍦板潃
};


#ifdef __cplusplus
}
#endif

#endif