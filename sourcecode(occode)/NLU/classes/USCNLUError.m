//
//  NLUError.m
//  prase
//
//  Created by jiqiangsheng on 14-3-6.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import "USCNLUError.h"

@implementation USCNLUError

+(NSString *)errorMessageWith:(int)code
{
    NSString *errorMessage = nil;
    switch (code)
    {
        case USCNLU_SUCCESS:
            errorMessage = @"操作成功";
            break;
        case USCNLU_TASK_OPERATION_FAILUER:
            errorMessage = @"业务操作失败";
        case USCNLU_INVALID_REQUEST:
            errorMessage = @"无效请求";
            break;
        case USCNLU_SEVER_INNER_ERROR:
            errorMessage = @"服务器内部错误";
            break;
        case USCNLU_NOT_SUPPORT_OPERATION:
            errorMessage = @"应用不支持此功能模块";
            break;
        case USCNLU_SEVER_UNENABLE_UNDERSTAND:
            errorMessage = @"服务器不理解或不能处理此文本";
            break;
        case USCNLU_RETURN_FROM_SEVER_NIL:
            errorMessage = @"服务器返回内容为空";
            break;
        case USCNLU_NET_UNREACHABLE:
            errorMessage = @"无法连接到网络";
            break;
        case USCNLU_CONTENT_PRASED_NIL:
            errorMessage = @"语音理解内容为空";
            break;
        case USCNLU_CONNECT_TIME_OUT:
            errorMessage = @"网络连接超时";
            break;
        case USCNLU_LOCATION_NOT_FOUND:
            errorMessage = @"定位失败";
            break;
        case USCNLU_JOB_CANCELED:
            errorMessage = @"语音理解被取消";
            break;
        default:errorMessage = @"%>_<%";
    }
    return errorMessage;
}

@end
