
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string>

//#include "define/define.h"
#include "http_def.h"
#include "set_option_key.h"
#include "Version.h"
#include "asr_client.h"
#include "libusc.h"
#include "AsrVadInterface.h"
#include "AsrServiceControlBlock.h"
#include "AsrVad.h"
#include "errorNo.h"
#include "function_pc.h"

#define ControlBlock AsrServiceControlBlock 

#define INPUT_BUFFER_LEN    640    // 50ms
#define PCM16K_BUFFER_LEN_300 9600
#define PCM16K_BUFFER_LEN	4800   // 150 ms
#define PCM8K_BUFFER_LEN	2400   // 150 ms
#define OPUS_BUFFER_LEN		1000
#define MP3_BUFFER_LEN		1000


static
int usc_internal_init(const char* uri, const unsigned short port, USC_HANDLE* handle){

	if(!handle)
		return NO_HANDLE_ERROR;
	int r = USC_OK;
	InitNetwork();
	ControlBlock *self = new ControlBlock();
	self->asrService = (asr_client*)asrCreateAsrService(uri, port);
  	if(self->asrService == NULL) {
		delete self;
		return INIT_ERROR;
	}
	*handle = (USC_HANDLE)self;
	self->setServer(uri, port);

	// 因为vad 总是出问题，所以此次上线先关闭vad 功能
	self->setVadEnabled(false);

  	return r;	
}

static 
int Stop(USC_HANDLE handle) {

	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return NO_HANDLE_ERROR;
	}

	asr_client* service =  self->asrService;

	if(self->isStop()) {
		return 0;
	}

	self->setStop();

	int r = NO_HANDLE_ERROR;
	if( service != NULL)
	{
		r = service->Stop();
	}
	return r;
}

static int Start(USC_HANDLE handle){

	ControlBlock *self = (ControlBlock*)handle;
	if (self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	//if (self->isAsrStarted()) {
	//	usc_stop_recognizer(handle);
	//}
	// set asr params
	char value[256];
	memset(value, 0, sizeof(value));
	get_mac(value, sizeof(value));

	const char* tmp;
	tmp = usc_get_option(handle, OPT_IMEI_ID);
	if (tmp != NULL && strlen(tmp) <= 0)
		usc_set_option(handle, OPT_IMEI_ID, value);
	tmp = usc_get_option(handle, OPT_USER_ID);
	if (tmp != NULL && strlen(tmp) <= 0)
		usc_set_option(handle, OPT_USER_ID, value);
	tmp = usc_get_option(handle, OPT_UDID_ID);
	if (tmp != NULL && strlen(tmp) <= 0)
		usc_set_option(handle, OPT_UDID_ID, value);

	if (self->collected_info.length() == 0)
		self->collected_info = "asr";
	int err = atoi(usc_get_option(handle, OPT_LAST_SOCKET_ERROR));
	get_client_info(value, sizeof(value), err, (char*)self->collected_info.c_str());
	usc_set_option(handle, OPT_COLLECTED_INFO, value);
	usc_vad_set_timeout(handle, self->frontSil, self->backSil);

	self->start();

	asr_client* service = self->asrService;
	int r = NO_HANDLE_ERROR;
	if (service != NULL)
	{
		service->SetValueInt(ASR_VAD_ENABLED, (int)self->isVadEnabled());
		r = service->Start();
	}
	return r;
}

USC_API
const char* usc_get_version(){
	return USC_SDK_VERSION;
}

USC_API
int usc_create_service(USC_HANDLE* handle){
    
	int ret = usc_create_service_ext(handle, U_HTTP_HOST, atoi(U_HTTP_PORT));
	if(ret == 0)
	{
		ControlBlock *self = (ControlBlock*) *handle;		
		if( self != NULL) {
			self->setCustomedAddress(true);
		}
	}

	return ret;
}

USC_API
int usc_create_service_ext(USC_HANDLE* handle ,const char* host, const unsigned short port )
{
	*handle = NULL;    
	const char* uri = host;
	short nPort = port;

	return usc_internal_init(uri, nPort, handle);
}

USC_API
int usc_login_service(USC_HANDLE handle)
{
	ControlBlock *self = (ControlBlock*)handle;
	if (self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	asr_client* service = self->asrService;
	int r = NO_HANDLE_ERROR;
	if (service != NULL)
	{
		r = service->Login();
	}
	return 0;
}

USC_API 
int usc_start_recognizer(USC_HANDLE handle)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) 
	{
		return NO_HANDLE_ERROR;
	}
	self->setRenew(false);
	return Start(handle);
}

USC_API
void usc_vad_set_timeout(USC_HANDLE handle, int frontSil,int backSil) {

	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return;
	}

	asr_client* service =  self->asrService;

	if( service != NULL)
	{
		self->frontSil = frontSil;
		self->backSil = backSil;

		frontSil = frontSil/10;
		backSil = backSil/10;
		service->setVadSilTime(frontSil, backSil);
	}
}

USC_API
int usc_stop_recognizer(USC_HANDLE handle){

	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return NO_HANDLE_ERROR;
	}
	asr_client* service =  self->asrService;
	if(self->isStop()) {
		return 0;
	}
	self->setStop();
	int r = NO_HANDLE_ERROR;
	if( service != NULL)
	{
		r = service->Stop();
	}
	return r;
}

USC_API
int usc_set_sample_rate(USC_HANDLE handle, int rate){
	
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return NO_HANDLE_ERROR;
	}

	asr_client* service =  self->asrService;


	int r = NO_HANDLE_ERROR;
	if( service != NULL)
	{
		if(rate == USC_SAMPLE_RATE_16K) {
			r = service->SetValueString(ASR_AUDIO_ENCODE_MTD,AUDIO_ENC_METH_OPUS);
			self->sampleRate = rate;
		}
		else if(USC_SAMPLE_RATE_8K == 8000){
			r = service->SetValueString(ASR_AUDIO_ENCODE_MTD8K, "");
			self->sampleRate = rate;
		}
	}
	return r;
}

USC_API 
int usc_get_result_begin_time(USC_HANDLE handle)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
		return NO_HANDLE_ERROR;
	return self->getBeginTime();
}

USC_API 
int usc_get_result_end_time(USC_HANDLE handle)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
		return NO_HANDLE_ERROR;
	return self->getEndTime();
}

static int usc_renew(USC_HANDLE handle)
{
	LOGMYE("USC_RENEW", "vad get cut", 1);
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return NO_HANDLE_ERROR;
	}
	self->setRenew(true);
	// get last result
	int ret = usc_stop_recognizer(handle);
	if(ret != 0) {
		return ret;
	}
	self->lastResult = self->asrService->GetResult();
	return Start(handle);
}

// put the data of buffer in the service added by lqy
static bool flush(USC_HANDLE handle, asr_client* service)
{
	ControlBlock *self = (ControlBlock*) handle;
	if (NULL == self)
		return false;
	int ret = 0;
	while (!self->asr_noise_filter.bf_queue.isNull())
	{
		cacheBuffer* bf_queue_front = self->asr_noise_filter.bf_queue.get_front();
		ret = service->Recognizer(bf_queue_front->buffer, bf_queue_front->length);
		self->asr_noise_filter.bf_queue.pop();
		if (ret < 0)
		{
			return false;
		}
	}
	return true;
}

USC_API 
int usc_feed_buffer(USC_HANDLE handle, const char* buffer, int len)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL) {
		return NO_HANDLE_ERROR;
	}
	// The beginning of the first reset voice when writing data
	self->updateBeginTime();
	asr_client* service =  self->asrService;
	int r = NO_HANDLE_ERROR;
	
  	if( service != NULL){
  	
		int isActive = 0;
		int inLen = len;
		r = USC_OK;

		int offset = 0;
		do {
			int readLen;
			unsigned char *temp_buffer = (unsigned char*)buffer + offset;
			if (inLen > INPUT_BUFFER_LEN)
				readLen = INPUT_BUFFER_LEN;
			else
				readLen = inLen;
			inLen -= readLen;
			offset += readLen;
			r = service->Recognizer((char*)temp_buffer, readLen);
			if (r<0) {
				break;
			}

#ifdef USC_ASR_SDK_LIMIT_SPEED
			usc_sleep(3 + readLen/64);
#endif
		} while(inLen > 0);
		
		if( r == 0/* && self->isgetpartialresult()*/){
			//isactive = asr_vad_back_end;
			self->result_buffer_size = 0;
			r = service->QueryResult();
		}

		if(USC_OK == r)
		{
			if( isActive == USC_VAD_BACK_END  || isActive == USC_VAD_MAX_SIL) {
			//	printf("~~~is Renew SpeakBegin~~~\n");
				self->asr_noise_filter.setSpeakBegin(false);
				self->asr_noise_filter.bf_queue.clear();
				r = usc_renew(handle);
				if(0 == r) {
					r = USC_RECOGNIZER_SPEAK_END;
				}
			}
			else if( isActive == USC_VAD_FRONT_END) {
				r = USC_RECOGNIZER_SPEAK_BEGIN;
			}
		}
		else if( SUCC_CODE::ASRCLIENT_RECOGNIZER_NO_RESULT == r) {
			r = SUCC_CODE::ASRCLIENT_RECOGNIZER_OK;
		}
		else if (MAX_SPEECH_TIMEOUT == r) {
			self->asr_noise_filter.setSpeakBegin(false);
			self->asr_noise_filter.bf_queue.clear();
			r = usc_renew(handle);
			if(0 == r) {
				r = USC_RECOGNIZER_SPEAK_END;
			}
		}
	}
  	return r;	
}

USC_API 
void usc_set_vad_time(USC_HANDLE handle, int frontSil,int backSil)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return;
	}
	asr_client* service =  self->asrService;
  	if( service != NULL)
	{  	
		self->frontSil = frontSil;
		self->backSil = backSil;
		frontSil /= 10;
		backSil /= 10;
  		service->setVadSilTime(frontSil, backSil);
  	}
}

USC_API 
int usc_get_volume(USC_HANDLE handle)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	asr_client* service =  self->asrService;
	int r=0;
  	if( service != NULL)
	{  	
  		r = service->usc_volume;
  	}
	return r;
}

USC_API 
const char* usc_get_result(USC_HANDLE handle)
{
	const char *result = "";
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return result;
	}
	asr_client* service =  self->asrService;
  	if( service != NULL)
  	{		
		if(self->lastResult.length() > 0)
		{			
			self->lastResult = "";
		}
		else {	
  			result = service->GetResult();	
			if(result == NULL) {
				result = "";
			}		
			if (self->isStop()) {
			}
			else {		
			}
		}
  	}	
	return result;	
}

USC_API 
int usc_cancel_recognizer(USC_HANDLE handle)
{
	int r = NO_HANDLE_ERROR;
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return r;
	}
	asr_client* service =  self->asrService;
  	if( service != NULL)
	{
  		r = service->Cancel();
  	}
	return r;	
}

USC_API 
void usc_release_service(USC_HANDLE handle)
{

	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return ;
	}
	asr_client* service =  self->asrService;	
	if(service)
	{
		service->Logout();
		asrDestroyAsrService(service);
		self->asrService = NULL;
		delete self;
	}	
}

static
int setOptionInt(asr_client* service, int id , int value)
{	
	int r = NO_HANDLE_ERROR;	
	if( service != NULL)
	{
		r = service->SetValueInt(id, value);
	}
	return r;
}

static
int setOptionString(asr_client* service, int id , const char* value)
{
	int r = NO_HANDLE_ERROR;	
	if( service != NULL)
	{	
		const char* s = value;
		r = service->SetValueString(id, s);
	}
	return r;
}

USC_API
const char * usc_get_option(USC_HANDLE handle, int option_id)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return "";
	}
	asr_client* service =  self->asrService;
	return service->GetOptionValue(option_id);
}

USC_API
int usc_set_option(USC_HANDLE handle, int option_id, const char* value)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	char msg[512] = { 0 };
	sprintf(msg, "option_id : %d, value : %s", option_id, value);
	LOGMYI("usc_set_option", msg, 2);
	asr_client* service =  self->asrService;
	int r = 0;
	int int_value = 0;
	switch(option_id)
	{
		case ASR_VAD_ENABLED:
			if(!strcmp(USC_DISABLED, value))
			{
				self->setVadEnabled(false);
			}
			else
            {
				self->setVadEnabled(true);
			}
			r = USC_OK;
			break;
		case OPT_APP_KEY:
		case USC_OPT_PUNCTUATION_ENABLED:
		case USC_OPT_NLU_PARAMETER:
		case USC_OPT_RESULT_FORMAT:
		case OPT_PACKAGE_NAME:
		case ASR_PUNCTUATION_ENABLE:
		case OPT_COLLECTED_INFO:
		case ASR_AUDIO_ENCODE_MTD:
		case ASR_AUDIO_ENCODE_MTD8K:
		case OPT_USE_MD5:
		case OPT_SECRET:
		case OPT_CRYPT_VERSION:
		case USC_OPT_ASR_ENGINE_PARAMETER:
		case OPT_USER_ID:
		case OPT_UDID_ID:
		case OPT_IMEI_ID:
		case USC_OPT_RECOGNITION_FIELD:
		case ASR_RESULT_PARAMETER:
		case OPT_LOG_FILE:
		case OPT_REQUEST_SLEEP_TIME:
		case OPT_SET_TOKEN:
		case USC_OPT_SET_CANCEL:
			r = setOptionString(service, option_id, value);
			break;
		case ASR_MAX_SPEECH_TIMEOUT_ID:
		case OPT_DEVICE_OS:
		case OPT_CARRIER:
		case OPT_NETWORK_TYPE:
		case USC_SERVICE_STATUS_SELECT:
		case USC_OPT_ASR_ENCODE_BITRATE:
		case ASR_WAITING_RESULT_TIMEOUT_ID:
		case ASR_NET_PACKAGE_SIZE:
		case OPT_LOG_SWITCH:
		case OPT_LOG_WRITE:
		case ASR_OPUS_BITRATE:
			int_value = atoi(value);
			r = setOptionInt(service, option_id, int_value);
			break;

		// enabled  the field of speech recognition  add by zhangkaijun 2014-03-03
		case ASR_PARTIAL_RESULT_ENABLED:
			//if(!strcmp(USC_DISABLED, value)) {
			//	self->setPartialResultEnabled(false);
			//}
			//else
   //         {
			//	self->setPartialResultEnabled(true);
			//}
			break;
            
        case USC_OPT_LANGUAGE_SELECT:
			if (value == NULL)
				r = OPTION_PARAM_ERROR;
			if (strcmp(value, LANGUAGE_CHINESE) == 0)
				r = setOptionString(service, ASR_LANGUAGE_SELECT, "cn");
			else if (strcmp(value, LANGUAGE_ENGLISH) == 0)
				r = setOptionString(service, ASR_LANGUAGE_SELECT, "en");
			else if (strcmp(value, LANGUAGE_CANTONESE) == 0)
				r = setOptionString(service, ASR_LANGUAGE_SELECT, "co");
			else
				r = setOptionString(service, ASR_LANGUAGE_SELECT, value);
            break;
		case USC_OPT_INPUT_AUDIO_FORMAT:
			if(value != NULL) {

				if(!strcmp(AUDIO_FORMAT_PCM_16K, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD,AUDIO_ENC_METH_OPUS);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_16K);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, PCM16K_BUFFER_LEN_300);
					service->SetValueInt(ASR_OPT_USE_DECODE, true);
				}
				else if(!strcmp(AUDIO_FORMAT_PCM_8K, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD8K, "");
                    // 8k pcm encode 16kpbs
					setOptionInt(service, USC_OPT_ASR_ENCODE_BITRATE, 32000);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, PCM16K_BUFFER_LEN);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_8K);
					service->SetValueInt(ASR_OPT_USE_DECODE, true);
				}
                else if(!strcmp(AUDIO_FORMAT_PCM_16_TO_8K, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD, AUDIO_ENC_METH_OPUS_NB);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, PCM16K_BUFFER_LEN_300);
                    // 8k pcm encode 16kpbs
                    setOptionInt(service, USC_OPT_ASR_ENCODE_BITRATE, 32000);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_16K);
					service->SetValueInt(ASR_OPT_USE_DECODE, true);
				}
				else if (!strcmp(AUDIO_FORMAT_PCM_NO_ENCODE_16k, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD, AUDIO_FORMAT_PCM);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_16K);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, PCM16K_BUFFER_LEN);
					service->SetValueInt(ASR_OPT_USE_DECODE, false);
				}
				else if (!strcmp(AUDIO_FORMAT_PCM_NO_ENCODE_8k, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD8K, "");
					// 8k pcm encode 16kpbs
					setOptionInt(service, USC_OPT_ASR_ENCODE_BITRATE, 32000);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_8K);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, PCM8K_BUFFER_LEN);
					service->SetValueInt(ASR_OPT_USE_DECODE, false);
				}
				else if (!strcmp(AUDIO_FORMAT_USC_OPUS_16K, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD, AUDIO_ENC_METH_OPUS);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, OPUS_BUFFER_LEN);
					service->SetValueInt(ASR_OPT_USE_DECODE, false);
					self->setDecode(INPUT_AUDIO_FORMAT_USC_OPUS_16K);
				}
				else if (!strcmp(AUDIO_FORMAT_USC_OPUS_8K, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD8K, "");
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, OPUS_BUFFER_LEN);
					service->SetValueInt(ASR_OPT_USE_DECODE, false);
					self->setDecode(INPUT_AUDIO_FORMAT_PCM_8K);
				}
				else if (!strcmp(AUDIO_FORMAT_MP3, value)) {
					r = service->SetValueString(ASR_AUDIO_ENCODE_MTD, AUDIO_FORMAT_MP3);
					r = service->SetValueInt(ASR_NET_PACKAGE_SIZE, MP3_BUFFER_LEN);
					service->SetValueInt(ASR_OPT_USE_DECODE, false);
					self->setDecode(INPUT_AUDIO_FORMAT_USC_OPUS_16K);
				}
			}
			break;
		case USC_OPT_DECODE_FRAME_SIZE:
			if( value != NULL) {
				int_value = atoi(value);
				r = 0;
			}
			break;
		case USC_OPT_NOISE_FILTER:
			if (value != NULL) {
				r = 0;
				// 状态开关
				if (!strcmp(USC_ENABLE, value))
				{
					self->asr_noise_filter.setIsUse(true);
				}
			}
			break;
		default:
			r = OPTION_ID_ERROR;
			break;
	}
	return r;	
}

// 设置语音参数 string string
USC_API
int usc_set_option_str(USC_HANDLE handle, const char* key, const char* value)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	asr_client* service =  self->asrService;
	return service->SetSerParam((char*)key, (char*)value);
}

// 设置语义参数 string string
USC_API
int usc_set_nlu_option_str(USC_HANDLE handle, const char* key, const char* value)
{
	ControlBlock *self = (ControlBlock*) handle;
	if(self == NULL)
	{
		return NO_HANDLE_ERROR;
	}
	asr_client* service =  self->asrService;
	return service->SetTraParam((char*)key, (char*)value);
}

USC_API
void usc_clear_option(USC_HANDLE handle, int option_id)
{
	ControlBlock *self = (ControlBlock*) handle;
    if(self == NULL)
	{
            return ;
    }
    asr_client* service =  self->asrService;
	return;// service->ClearOptionValue(option_id);
}