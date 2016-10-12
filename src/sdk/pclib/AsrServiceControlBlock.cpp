
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "asr_client.h"
#include "libusc.h"
#include "AsrServiceControlBlock.h"
#include "function_pc.h"

#ifdef LINUX
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>  
#include <sys/ioctl.h> 
#elif defined (WIN32)

#include <windows.h>
#include <wincon.h>
#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")

#endif

#ifdef __cplusplus
extern "C" {
#endif

AsrServiceControlBlock::AsrServiceControlBlock():asrService(NULL),asr_started(false),beginTime(0),endTime(0),sampleRate(USC_SAMPLE_RATE_16K) {

	//asr_noise_filter = new AsrNoiseFilter();
	is_vad_enabled = false;// true;
	setDecode(INPUT_AUDIO_FORMAT_PCM_16K);
	result_buffer_size = 0;
	is_renew = false;
	frontSil = 3000;
	backSil = 1000;
	is_customed_address = false;
	collected_info = "";
	num = 0;
}

bool AsrServiceControlBlock::isStop() {
	return !asr_started;
}

bool AsrServiceControlBlock::isCustomedAddress() {
	return is_customed_address;
}

void AsrServiceControlBlock::setStop() {

	setAsrStarted(false);
}

void AsrServiceControlBlock::start() {

	this->reset();
	setAsrStarted(true); 
}


bool AsrServiceControlBlock::setDecode(int type) {
		
	if(type == INPUT_AUDIO_FORMAT_PCM_16K){
			
		sampleRate = USC_SAMPLE_RATE_16K;
		voice_type = type;
	}
	else  if(type == INPUT_AUDIO_FORMAT_PCM_8K){

		sampleRate = USC_SAMPLE_RATE_8K;
		voice_type = type;
	}else if( type ==  INPUT_AUDIO_FORMAT_USC_OPUS_16K) {

		sampleRate = USC_SAMPLE_RATE_16K;
		voice_type = type;
	}
	else  {
		sampleRate = USC_SAMPLE_RATE_16K;
		voice_type = type;
	}

	// ÉèÖÃ¶Ï¾äÊ±¼ä
	asr_noise_filter.bf_queue.set_buffer_byte_size(sampleRate * sizeof(short) / 1000 * asr_noise_filter.getVadCacheTime());

	return false;
}

void AsrServiceControlBlock::addDataLength( int length)
{
	endTime += length;
	result_buffer_size += length;
}

void AsrServiceControlBlock::updateBeginTime() {

	if( isRenew()) {
		setBeginTime(endTime + sampleRate*sizeof(short)); // 1s data 
		setRenew(false);
	}
}

void AsrServiceControlBlock::setServer(const char* ip,const unsigned short port) {

	if(ip == NULL) {
		return;
	}
	if (asrService != NULL)
		asrService->SetServer(ip, port);
}

void AsrServiceControlBlock::setBeginTime(int time) {
	beginTime = time;
}

int AsrServiceControlBlock::getBeginTime() {
	int sec = sampleRate*sizeof(short); // 1s data 
	int begin = beginTime - sec;
	if(begin < 0) {
		begin = 0;
	}

	return begin/(sampleRate/1000*sizeof(short)); // ms
}
int AsrServiceControlBlock::getEndTime() {
	return endTime/(sampleRate/1000*sizeof(short)); // ms
}

void AsrServiceControlBlock::reset() {
	
	if(false == isRenew()) {
		beginTime = 0;
		endTime = 0;
	}

	result_buffer_size = 0;
	setAsrStarted(false);

}

AsrServiceControlBlock::~AsrServiceControlBlock()
{
	//delete asr_noise_filter;
}
void AsrServiceControlBlock::setIsCustomAddress(int isCustom)
{
	//num = 8;
	//this->isCustomedAddress = isCustom;
	
	printf("set Value is successful");
}

int AsrServiceControlBlock::getCustomAddress()
{
	return 0;
}

//~~~~~~~~~~~~~AsrNoiseFilter~~~~~~~start~~~~~~~~~~~~~~

AsrNoiseFilter::AsrNoiseFilter(void)
	:is_speak_begin(false), is_use(false), vad_cache_time(300)
{	
	//bf_queue = new BufferQueue();
}

AsrNoiseFilter::~AsrNoiseFilter(void)
{	
	//delete bf_queue;
}

bool AsrNoiseFilter::setSpeakBegin(bool flag)
{
	if (!is_use)
		return true;
	is_speak_begin = flag;
	return is_speak_begin;
}

bool AsrNoiseFilter::isSpeakBegin()
{
	if (!is_use)
		return true;
	return is_speak_begin;
}

int AsrNoiseFilter::setVadCacheTime(int time)
{
	if (time < 0)
		return -1;
	vad_cache_time = time;
	return vad_cache_time;
}

int AsrNoiseFilter::getVadCacheTime()
{
	return vad_cache_time;
}

bool AsrNoiseFilter::setIsUse(bool flag)
{
	is_use = flag;
	return is_use;
}

bool AsrNoiseFilter::getIsUse()
{
	return is_use;
}

//~~~~~~~~~~~~~AsrNoiseFilter~~~~~~~end~~~~~~~~~~~~~~~~


#ifdef __cplusplus
}
#endif
