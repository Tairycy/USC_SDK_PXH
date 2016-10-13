//
//  USCErrorCode.m
//  usc
//
//  Created by hejinlai on 13-2-22.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//
#import "USCTTSErrorCode.h"


@implementation USCTTSErrorCode

+(NSString *)getErrorMsg:(int)code
{
    switch (code) {
        case ERROR_GET_MODEL:
            return @"获取模型路径错误";
            break;
        case ERROR_LOAD_MODEL:
            return @"加载模型错误";
            break;
        case SYSTHESIS_ENGINE_ERROR:
            return @"离线合成错误";
            break;
        case PLAY_ERROR:
            return @"播放错误";
            break;
        case AUTHORIZE_ERROR:
            return @"授权失败";
            break;

        case ERROR_SYNRHESISTEXT:
            return @"合成文字错误";
            break;
        case SYNTHESIZE_CANCLEED:
            return @"合成被取消";
            break;
        default:
            return @"未知错误";
    }
}
@end
