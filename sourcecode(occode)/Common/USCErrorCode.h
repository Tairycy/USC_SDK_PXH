//
//  USCErrorCode.h
//  usc
//
//  Created by hejinlai on 13-2-22.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "log.h"

static const int RECOGNIZER_OK = 0;

// 上传用户数据
static const int UPLOAD_USER_DATA_SERVER_ERROR = -63001;
static const int UPLOAD_USER_DATA_NETWORK_ERROR = -63002;
static const int UPLOAD_USER_DATA_EMPTY = -63003;
static const int UPLOAD_USER_DATA_NAME_EMPTY = -63004;
static const int UPLOAD_USER_DATA_APP_EMPTY = -63005;
static const int UPLOAD_USER_DATA_SONG_EMPTY = -63006;
static const int UPLOAD_USER_DATA_TOO_LARGE = -63007;
static const int UPLOAD_USER_DATA_INVALID_FORMAT = -63008;
static const int UPLOAD_USER_DATA_TOO_FAST = -63009;
static const int UPLOAD_USER_DATA_ENCODE_ERROR = -63010;

// 上传全语音词表
static const int UPLOAD_SCENE_DATA_SERVER_REFUSED = -63011;
static const int UPLOAD_SCENE_DATA_NETWORK_ERROR = -63012;
static const int UPLOAD_SCENE_DATA_EMPTY = -63013;
static const int UPLOAD_SCENE_OUT_MAX_COUNT = -63014;
static const int UPLOAD_SCENE_TOO_LARGE = -63017;
static const int UPLOAD_SCENE_DATA_TOO_FAST = -63019;
static const int UPLOAD_SCENE_ENCODE_ERROR = -63020;
static const int UPLOAD_SCENE_GENERAL_ERROR = -63021;
static const int UPLOAD_SCENE_INVALID_KEY = -63022;
static const int UPLOAD_SCENE_STREAM_IO_ERR = -63023;
static const int UPLOAD_SCENE_UNKNOWN_ERR = -63024;
static const int UPLOAD_SCENE_DATA_SIZE_IS_FORBIDDEN = -63025;
static const int UPLOAD_SCENE_INVALID_VER = -63026;

// 语义理解
static const int NLU_SERVER_ERROR = -71001;
static const int NLU_REQUEST_EMPTY = -71002;

//识别
static const int RECOGNITION_TIMEOUT = -62002;  //识别超时

// 操作过于频繁
static const int OPERATION_TOO_FAST = -64001;


// 录音
static const int RECORDING_AUDIO_INPUT_ERROR = -65001;
static const int RECORDING_AUDIO_SESSION_ERROR = -65002;
static const int RECORDING_CATEGORY_ERROR = -65003;
static const int RECORDING_ENQUEUE_BUFFER_ERROR = -65004;
static const int RECORDING_ALLOC_BUFFER_ERROR = -65005;
static const int RECORDING_AUDIO_START_ERROR = -65006;

// 全局变量
static int g_recognizer_code = 0;
static int g_conn_code = 0;
static int g_os_errorno = 0;

// asr错误码
static const int ASRCLIENT_VAD_TIMEOUT = -30001;
static const int ASRCLIENT_MAX_SPEECH_TIMEOUT = -30002;
static const int ASRCLIENT_COMPRESS_PCM_ERROR = -30003;
static const int ASRCLIENT_INVALID_PARAMETERS = -30004;



static  int ERROR_GET_MODEL = -90001;
static  int ERROR_LOAD_MODEL = -90002;

/* 合成错误 */
static const int SYSTHESIS_MODEL_ERROR = -100;
static  const int SYSTHESIS_ENGINE_ERROR = -101;
static  const int PLAY_ERROR = -200;



// asr 识别过程中遇到的普通错误
static const int ASR_RECOGNIZE_AUDIOFILE_ERROR = -10000000;

@interface USCErrorCode : NSObject


+(NSString *)getErrorMsg:(int)code;

@end
