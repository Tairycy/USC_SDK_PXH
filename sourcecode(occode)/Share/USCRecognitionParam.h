//
//  USCRecognitionParam.h
//  asr&nlu&tts
//
//  Created by iOSDeveloper-zy on 15-5-8.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCUtil.h"
#import "USCEngineParams.h"
#import "USCScene.h"

@interface USCRecognitionParam : NSObject

// 主动设置
@property (nonatomic,assign) int recognitionType; // asr = 1，nlu = 2,vpr = 3

@property (nonatomic,copy) NSString *appkey;
@property (nonatomic,strong) NSString *secret;
@property (nonatomic,assign) int sampleRate;
@property (nonatomic,strong) NSString *engine;
@property (nonatomic,strong) NSString *serviceAddress;
@property (nonatomic,assign) NSString *port;
@property (nonatomic,strong) USCUtil *resultFormat;
@property (nonatomic,assign) BOOL isPunctuation;

///------------
///  asr
///------------
@property (nonatomic,strong) USCEngineParams *engineParam;
///------------
///  nlu
///------------
@property (nonatomic,strong) NSString *nluScenario;
@property (nonatomic,assign) BOOL isReturnNluResult;
///------------
///  vpr
///------------
@property (nonatomic,assign) USCVPRType vprType;
@property (nonatomic,assign) BOOL isVPREnable;
@property (nonatomic,strong) NSString *userName;
@property (nonatomic,strong) NSString *vprTypeStr;
@property (nonatomic,strong) USCScene *scene;
@end
