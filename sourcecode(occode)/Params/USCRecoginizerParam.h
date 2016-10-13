//
//  USCRecoginizerParam.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-11.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCParams.h"
#import "USCRecognitionParam.h"

@class USCScene,USCEngineParams,USCVADParam;
// @class - 识别参数类
// @brief - 保存识别过程中的各个参数

@interface USCRecoginizerParam : USCParams
// appkey
@property (nonatomic,strong) NSString *appkey;

@property (nonatomic,strong) USCEngineParams *engineParam;

/**
 *  获取appkey
 *
 *  @return appkey
 */
- (NSString *)getAppkey;

/**
 *  获取场景对象
 *
 *  @return 场景
 */
- (USCScene *)getStartScene;

/**
 *  设置场景
 */
- (void)setStartScene:(USCScene *)scene;

/*!
 *
 *  @brief  获取engineParams
 *
 *  @return
 *
 */
- (USCEngineParams *)getEngineParams;

- (BOOL)setSampleRate:(int)rate;

- (BOOL)setModelType:(NSString*)engine;

/*!
 *
 *  @brief  设置识别语言
 *
 *  @param language 语言
 *
 *  @return 是否设置成功
 */
- (BOOL)setLanguage:(NSString *)language;

- (BOOL)setVoiceField:(NSString *)voiceField;

@property (nonatomic,strong) USCVADParam *vadParam;

@end
