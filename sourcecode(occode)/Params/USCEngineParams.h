//
//  USCEngineParams.h
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-12.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCConstant.h"
/*!
 *
 *  @brief  识别引擎参数类，用来设置引擎参数 modeltype + language + voiceField + sample
 *
 *  @since
 */
@interface USCEngineParams : NSObject

/*!
 *  @author usc_zy, 15-04-01 14:04:06
 *
 *  @brief  设置语言，不再使用以前的更换服务地址办法
 *
 *  @param language 语言
 */
- (BOOL)setLanguage:(NSString *)language;

/*!
 *
 *  @brief  设置远近讲
 */
- (BOOL)setVoiceField:(NSString *)voiceField;

// 采样率
- (void)setSampleRate:(NSString *)sampleRate;

- (BOOL)setSampleRateInt:(int)rate;

- (int)getRate;

- (NSString *)toString;

- (void)reset;

- (void)setSampleRate16k;

- (void)setSampleRateReal8k;

- (void)setSampleRate8k;

- (BOOL)setModelType:(NSString *)value;

- (void)setEnabled:(BOOL)enabled;

- (BOOL)isEnabled;

@property (nonatomic,assign) BOOL farVoiceField;

@end
