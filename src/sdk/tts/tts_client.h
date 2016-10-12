#ifndef __TTSCLIENT_NEW_H_INCLUDED__
#define __TTSCLIENT_NEW_H_INCLUDED__


typedef long long TTSHANDLE;

#ifdef __cplusplus
extern "C"{
#endif

int tts_create_service(TTSHANDLE* handle);
int tts_create_service_ext(TTSHANDLE* handle, const char *host, const char *port);
int tts_release_service(TTSHANDLE handle); //�ͷźϳɽӿ�ʵ��
int tts_start_synthesis(TTSHANDLE handle);
int tts_stop_synthesis(TTSHANDLE handle); //ֹͣ�ϳ�
int tts_text_put(TTSHANDLE handle, const char * textdata, unsigned int textlen); //�ϴ��ı����ݽ��кϳ�
const void* tts_get_result(TTSHANDLE handle, unsigned int* audioLen, int* synthStatus, int* errorCode);//��ȡ�ϳɽ��
const char* tts_get_option(TTSHANDLE handle, int option_id, int* errorCode);//�õ���չ����
const char* tts_get_version(TTSHANDLE handle, int* errorCode);//�õ��汾��
int tts_set_option(TTSHANDLE handle, const char* key, const char* value);
int tts_set_option_id(TTSHANDLE handle, int key, const char* value);
int tts_cancle(TTSHANDLE handle);

#ifdef __cplusplus
}
#endif

#endif