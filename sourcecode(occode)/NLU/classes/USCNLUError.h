//
//  NLUError.h
//  prase
//
//  Created by jiqiangsheng on 14-3-6.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import <Foundation/Foundation.h>


static const int USCNLU_SUCCESS = 0;                     //操作成功
static const int USCNLU_TASK_OPERATION_FAILUER = 1;      //业务操作失败
static const int USCNLU_INVALID_REQUEST = 2;             //无效请求
static const int USCNLU_SEVER_INNER_ERROR = 3;           //服务器内出错误
static const int USCNLU_NOT_SUPPORT_OPERATION = 4;       //应用不支持此功能模块
static const int USCNLU_SEVER_UNENABLE_UNDERSTAND = 5;   //服务器不理解或不能处理此文本
static const int USCNLU_CONNECT_TIME_OUT = 10000;        //网络连接超时
static const int USCNLU_RETURN_FROM_SEVER_NIL = 10001;   //服务器返回内容为空
static const int USCNLU_NET_UNREACHABLE = 10002;         //无法连接到网络
static const int USCNLU_CONTENT_PRASED_NIL = 10003;      //解析内容为空
static const int USCNLU_JSON_FORMAT_WRONG = 10004;       //服务器返回json字符串格式错误

static const int USCNLU_LOCATION_SERVICE = 101;            //定位服务属性id
static const int USCNLU_LOCATION_NOT_FOUND = 102;          //定位地理位置失败
static const int USCNLU_JOB_CANCELED = 103;                //取消该次任务

@interface USCNLUError : NSObject

+(NSString *)errorMessageWith:(int)code;

@end
