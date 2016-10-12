#ifndef _LIBUSC_VPR_VPR_H_
#define _LIBUSC_VPR_VPR_H_

// ���������ʽ
#define AUDIO_FORMAT_PCM_16K         "pcm16k"
#define AUDIO_FORMAT_PCM_8K          "pcm8k"
#define AUDIO_FORMAT_AMRNB           "amrnb"

// ״̬����
#define USC_ENABLE        "true"
#define USC_DISABLED      "false"


enum {
    // ����ʶ������
    USC_VPR_OK                           = 0,

    // �н������
    USC_VPR_RECOGNIZER_PARTIAL_RESULT    = 2,

    // ��⵽������ʼ
    USC_VPR_RECOGNIZER_SPEAK_BEGIN       = 100,

    // ��⵽��������
	USC_VPR_RECOGNIZER_SPEAK_END = 101,

	// ʶ��������
	USC_VPR_NO_HANDLE_INPUT = -91138,

	// ����ID����
	USC_VPR_INVALID_ID = -91151,

	// ��������
	USC_VPR_INVALID_PARAMETERS = -91152,

	// �������ݸ�ʽ����
	USC_VPR_INVALID_INPUT_DATA = -91157,

};


enum {
    // ����ΪAPP_KEY
	USC_VPR_OPT_APP_KEY = 9,

	USC_VPR_OPT_ENGINE_PARAMETER = 104,

	// �������Ʋ���
	USC_VPR_OPT_TRAFFIC_PARAMETER = 201,

	// �����Ƿ�����md5 У��
	USC_VPR_OPT_USE_MD5 = 203,

	// ����Ϊ�û�secret
	USC_VPR_OPT_USER_SECRET = 204,

	// ���ü��ܰ汾��
	USC_VPR_OPT_CRYPT_VERSION = 205,

    // �������������ʽ
    USC_VPR_OPT_INPUT_AUDIO_FORMAT       = 1001,

	// ����ʶ�����ı����Ƿ�ʹ�ñ�����
	USC_VPR_OPT_PUNCTUATION_ENABLED      = 1002,

	// ��������֡�ֽڳ���
	USC_VPR_OPT_DECODE_FRAME_SIZE        = 1003,

	// �������ݹ���
	USC_VPR_OPT_NOISE_FILTER			 = 1004,
	
	USC_VPR_OPT_RESULT_FORMAT			 = 1006,

	USC_VPR_SERVICE_STATUS_SELECT		 = 1015,

};

// ��������ʶ����
typedef long long VPRHANDLE;

#ifdef WIN32
	#ifdef LIBUSC_VPR_EXPORTS
		#ifdef JNI_EXPORTS
			#define USC_API
		#else
			#define USC_API extern "C" __declspec(dllexport)
		#endif
	#else
		#define USC_API extern "C" __declspec(dllimport)
	#endif
#else
	#define USC_API extern "C" __attribute__ ((visibility("default")))
#endif


// ��������������ʶ��
USC_API int usc_vpr_create_service(VPRHANDLE* handle);

// ����˽��������ʶ��
USC_API int usc_vpr_create_service_ext(VPRHANDLE* handle, const char* host, const unsigned short port);

// ��¼������ʶ��
USC_API int usc_vpr_login_service(VPRHANDLE handle);

// ����VAD��ʱʱ��
USC_API void usc_vpr_vad_set_timeout(VPRHANDLE handle, int frontSil,int backSil);

// ����
USC_API int usc_vpr_start_recognizer(VPRHANDLE handle);

// ֹͣ
USC_API int usc_vpr_stop_recognizer(VPRHANDLE handle);

// ��������ʶ��
USC_API int usc_vpr_feed_buffer(VPRHANDLE handle, const char* buffer, int len);

// �������ʶ����
USC_API const char* usc_vpr_get_result(VPRHANDLE handle);

// �ͷ�����ʶ��
USC_API void usc_vpr_release_service(VPRHANDLE handle);

// ��������ʶ�����
USC_API int usc_vpr_set_option(VPRHANDLE handle, int option_id, const char* value);

// ������������ string string
USC_API int usc_vpr_set_option_str(VPRHANDLE handle, const char* key, const char* value);

// ��ȡSDK �汾��
USC_API const char* usc_vpr_get_version();

// ��ȡ����ʶ����ز���
USC_API const char * usc_vpr_get_option(VPRHANDLE handle, int option_id);

USC_API void usc_vpr_clear_option(VPRHANDLE handle, int option_id);

#endif

