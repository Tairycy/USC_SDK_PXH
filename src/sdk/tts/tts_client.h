#ifndef __TTSCLIENT_NEW_H_INCLUDED__
#define __TTSCLIENT_NEW_H_INCLUDED__


typedef long long TTSHANDLE;

#ifdef __cplusplus
extern "C"{
#endif

int tts_create_service(TTSHANDLE* handle);
int tts_create_service_ext(TTSHANDLE* handle, const char *host, const char *port);
int tts_release_service(TTSHANDLE handle); //释放合成接口实例
int tts_start_synthesis(TTSHANDLE handle);
int tts_stop_synthesis(TTSHANDLE handle); //停止合成
int tts_text_put(TTSHANDLE handle, const char * textdata, unsigned int textlen); //上传文本数据进行合成
const void* tts_get_result(TTSHANDLE handle, unsigned int* audioLen, int* synthStatus, int* errorCode);//获取合成结果
const char* tts_get_option(TTSHANDLE handle, int option_id, int* errorCode);//得到扩展属性
const char* tts_get_version(TTSHANDLE handle, int* errorCode);//得到版本号
int tts_set_option(TTSHANDLE handle, const char* key, const char* value);
int tts_set_option_id(TTSHANDLE handle, int key, const char* value);
int tts_cancle(TTSHANDLE handle);

#ifdef __cplusplus
}
#endif

#endif