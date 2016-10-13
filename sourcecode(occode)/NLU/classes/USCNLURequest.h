//
//  USCNLURequest.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-15.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

// @class - 语义理解参数类
// @brief - 包含语义理解过程中各种参数
@interface USCNLURequest : NSObject
// 语义云服务访问域名（请勿更改）
@property (nonatomic,strong) NSString *url;
// 方法的默认参数名（请勿更改）
@property (nonatomic,strong) NSString *method;
// 语义云服务协议版本号(一般情况下不需要更改)
@property (nonatomic,strong) NSString *ver;
// 语义云平台创建的应用的appKey;
@property (nonatomic,strong) NSString *appkey;
// 语义云平台创建的应用的appSecret, 仅用于签名
@property (nonatomic,strong) NSString *secret;
// 您自己的应用的版本号
@property (nonatomic,strong) NSString *appver;
// 您的终端用户的唯一标识，可以是手机的IMEI，设备序列号，微信ID等（用于唯一标识用户，在对话场景下是必需的）
@property (nonatomic,strong) NSString *udid;

@property (nonatomic,strong) NSString *gps;

@property (nonatomic,strong) NSString *city;
// 时间，格式：yyyy-MM-dd HH:mm:ss
@property (nonatomic,strong) NSString *time;
// 待解析文字
@property (nonatomic,strong) NSString *text;
// 上一次访问语义云平台返回的history信息
@property (nonatomic,strong) NSString *history;
// 语音日志ID
@property (nonatomic,strong) NSString *voiceid;
@property (nonatomic,strong) NSString *scenario;
@property (nonatomic,strong) NSString *screen;
@property (nonatomic,strong) NSString *dpi;
@property (nonatomic,strong) NSString *platform;
@property (nonatomic,strong) NSString *viewid;

@end
