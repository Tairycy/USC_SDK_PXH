//
//  USCErrorCode.m
//  usc
//
//  Created by hejinlai on 13-2-22.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//
#import "USCErrorCode.h"


@implementation USCErrorCode

+(NSString *)getErrorMsg:(int)code
{
    switch (code) {

        case RECORDING_ALLOC_BUFFER_ERROR:
        case RECORDING_AUDIO_INPUT_ERROR:
        case RECORDING_CATEGORY_ERROR:
        case RECORDING_ENQUEUE_BUFFER_ERROR:
        case RECORDING_AUDIO_SESSION_ERROR:
            return @"录音启动失败";

        case OPERATION_TOO_FAST:
            return @"操作太频繁";

        case UPLOAD_USER_DATA_EMPTY:
            return @"个性化数据：数据为空";
        case UPLOAD_USER_DATA_TOO_LARGE:
            return @"个性化数据：大小超出限制";
        case UPLOAD_USER_DATA_NETWORK_ERROR:
            return @"个性化数据：网络连接错误";
        case UPLOAD_USER_DATA_SERVER_ERROR:
            return @"个性化数据：服务器连接错误";
        case UPLOAD_USER_DATA_TOO_FAST:
            return @"个性化数据：上传过于频繁";
        case UPLOAD_USER_DATA_ENCODE_ERROR:
            return @"个性化数据：加密失败";

        case  ASRCLIENT_MAX_SPEECH_TIMEOUT:
            return @"说话时间超出限制";

        case  ASRCLIENT_COMPRESS_PCM_ERROR:
            return @"语音数据压缩错误";

        case SESSION_LOGIC_ERR::ASRCLIENT_AUTHORIZE_ERROR:
            return @"无效的appkey";

        case RECOGNITION_TIMEOUT:
            return @"识别超时";

        case RECOGNIZER_OK:
            return @"成功";
        case UPLOAD_SCENE_DATA_SERVER_REFUSED:
            return @"上传场景数据:服务器拒绝";
        case UPLOAD_SCENE_DATA_NETWORK_ERROR:
            return @"上传场景数据:网络连接失败";
        case UPLOAD_SCENE_DATA_EMPTY:
            return @"上传场景数据:不能为空";
        case UPLOAD_SCENE_TOO_LARGE:
            return @"上传场景数据:内容太多";
        case UPLOAD_SCENE_ENCODE_ERROR:
            return @"上传场景数据:编码失败";
        case UPLOAD_SCENE_DATA_TOO_FAST:
            return @"上传场景数据:上传过于频繁";
        case UPLOAD_SCENE_GENERAL_ERROR:
            return @"上传场景数据:常见错误";
        case UPLOAD_SCENE_INVALID_KEY:
            return @"上传场景数据无效的AppKey";
        case UPLOAD_SCENE_STREAM_IO_ERR:
            return @"上传场景数据上传的数据流异常";
        case UPLOAD_SCENE_UNKNOWN_ERR:
            return @"上传场景数据未知异常";
        case UPLOAD_SCENE_DATA_SIZE_IS_FORBIDDEN:
            return @"上传场景数据上传的数据体积异常";
        case UPLOAD_SCENE_INVALID_VER:
            return @"上传场景数据无效的SDK版本号";
        case NLU_SERVER_ERROR:
            return @"语义理解: 语义服务器访问失败";
        case NLU_REQUEST_EMPTY:
            return @"语义理解:语义请求为空";
        case ASR_RECOGNIZE_AUDIOFILE_ERROR:
            return @"语音文件路径错误";
            break;
        case SYSTHESIS_ENGINE_ERROR:
            return @"合成错误";
            break;
        case SYSTHESIS_MODEL_ERROR:
            return @"加载模型错误";
            break;
        case PLAY_ERROR:
            return @"播放错误";
            break;
        default:
            return @"网络连接错误";
    }
}


struct SESSION_LOGIC_ERR{
    static const int ASRCLIENT_AUTHORIZE_ERROR = -20001;
    static const int ASRCLIENT_SERVICE_NOT_RUNNING = -20002;
    static const int ASRCLIENT_SERVER_RETURN_TOO_LONG_RESP = -20003;
    static const int ASRCLIENT_SERVER_COMM_ERROR = -20004;
    static const int ASRCLIENT_CONTENT_DECODE_ERROR = -20005;
};
@end
