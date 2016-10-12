#ifndef _LIBUSC_VPR_VPR_H_
#define _LIBUSC_VPR_VPR_H_

// 语音编码格式
#define AUDIO_FORMAT_PCM_16K         "pcm16k"
#define AUDIO_FORMAT_PCM_8K          "pcm8k"
#define AUDIO_FORMAT_AMRNB           "amrnb"

// 状态开关
#define USC_ENABLE        "true"
#define USC_DISABLED      "false"


enum {
    // 声纹识别正常
    USC_VPR_OK                           = 0,

    // 有结果返回
    USC_VPR_RECOGNIZER_PARTIAL_RESULT    = 2,

    // 检测到语音开始
    USC_VPR_RECOGNIZER_SPEAK_BEGIN       = 100,

    // 检测到语音结束
	USC_VPR_RECOGNIZER_SPEAK_END = 101,

	// 识别句柄错误
	USC_VPR_NO_HANDLE_INPUT = -91138,

	// 参数ID错误
	USC_VPR_INVALID_ID = -91151,

	// 参数错误
	USC_VPR_INVALID_PARAMETERS = -91152,

	// 语音数据格式错误
	USC_VPR_INVALID_INPUT_DATA = -91157,

};


enum {
    // 参数为APP_KEY
	USC_VPR_OPT_APP_KEY = 9,

	USC_VPR_OPT_ENGINE_PARAMETER = 104,

	// 设置声纹参数
	USC_VPR_OPT_TRAFFIC_PARAMETER = 201,

	// 设置是否启用md5 校验
	USC_VPR_OPT_USE_MD5 = 203,

	// 参数为用户secret
	USC_VPR_OPT_USER_SECRET = 204,

	// 设置加密版本号
	USC_VPR_OPT_CRYPT_VERSION = 205,

    // 输入语音编码格式
    USC_VPR_OPT_INPUT_AUDIO_FORMAT       = 1001,

	// 声纹识别结果文本中是否使用标点符号
	USC_VPR_OPT_PUNCTUATION_ENABLED      = 1002,

	// 语音解码帧字节长度
	USC_VPR_OPT_DECODE_FRAME_SIZE        = 1003,

	// 噪音数据过滤
	USC_VPR_OPT_NOISE_FILTER			 = 1004,
	
	USC_VPR_OPT_RESULT_FORMAT			 = 1006,

	USC_VPR_SERVICE_STATUS_SELECT		 = 1015,

};

// 定义声纹识别句柄
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


// 创建公有云声纹识别
USC_API int usc_vpr_create_service(VPRHANDLE* handle);

// 创建私有云声纹识别
USC_API int usc_vpr_create_service_ext(VPRHANDLE* handle, const char* host, const unsigned short port);

// 登录公有云识别
USC_API int usc_vpr_login_service(VPRHANDLE handle);

// 设置VAD超时时间
USC_API void usc_vpr_vad_set_timeout(VPRHANDLE handle, int frontSil,int backSil);

// 启动
USC_API int usc_vpr_start_recognizer(VPRHANDLE handle);

// 停止
USC_API int usc_vpr_stop_recognizer(VPRHANDLE handle);

// 进行声纹识别
USC_API int usc_vpr_feed_buffer(VPRHANDLE handle, const char* buffer, int len);

// 获得声纹识别结果
USC_API const char* usc_vpr_get_result(VPRHANDLE handle);

// 释放声纹识别
USC_API void usc_vpr_release_service(VPRHANDLE handle);

// 设置声纹识别参数
USC_API int usc_vpr_set_option(VPRHANDLE handle, int option_id, const char* value);

// 设置语音参数 string string
USC_API int usc_vpr_set_option_str(VPRHANDLE handle, const char* key, const char* value);

// 获取SDK 版本号
USC_API const char* usc_vpr_get_version();

// 获取声纹识别相关参数
USC_API const char * usc_vpr_get_option(VPRHANDLE handle, int option_id);

USC_API void usc_vpr_clear_option(VPRHANDLE handle, int option_id);

#endif

