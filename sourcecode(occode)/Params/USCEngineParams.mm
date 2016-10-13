//
//  USCEngineParams.m
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-12.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCEngineParams.h"

static   NSString *MODEL_TYPE = @"modelType";
static   NSString *MODEL_TYPE_GENERAL = @"general";

static   NSString *VOICE_FIELD = @"voiceField";
static   NSString *SAMPLE_RATE = @"sampleRate";

static   NSString *KEY_VOICE_FIELD = @"voiceField";
static   NSString *KEY_LANGUAGE = @"lang";
static   NSString *KEY_SAMPLE_RATE = @"sampleRate";

/**
 * 16kto 8k
 */
static int RATE_8K = 8000;
/**
 * 16k
 */
static  int RATE_16K = 16000;
/**
 * 8k语音输入
 */
static  int RATE_REAL_8K = 80000;

/**
 * 语言类型英语
 */
static   NSString  *LANGUAGE_ENGLISH = @"en";
/**
 * 语言类型粤语
 */
static   NSString  *LANGUAGE_CANTONESE = @"co";
/**
 * 语言类型汉语普通话
 */
static    NSString  *LANGUAGE_CHINESE = @"cn";

typedef enum
{
    USCVOICEFIELD_FAR,
    USCVOICEFIELD_NEAR
}USCVOICEFIELD;

@interface USCEngineParams()
{
    BOOL _mIsChange;
    int _mSampleRate;
    NSString *_mLanguage;
    NSMutableDictionary *_mParams;
    NSMutableString *_mStringParam;
    BOOL _mIsEnabled;
}
@end

@implementation USCEngineParams
- (instancetype)init
{
    if (self = [super init]) {
        _mIsChange = YES;
        _mSampleRate = RATE_16K;
        _mIsEnabled = YES;
        _mLanguage = [NSMutableString stringWithString:LANGUAGE_CHINESE];
        _mParams = [[NSMutableDictionary alloc]init];
        _mStringParam = [[NSMutableString alloc]init];

        // 初始化设置默认的
        [self setVoiceField:@"near"];
        [self setSampleRateInt:RATE_16K];
    }
    return self;
}

#pragma mark - public method

/**
 * 设置语言 目前没有使用，待服务器支持 2015-01-27
 * @param language
 */
- (BOOL)setLanguage:(NSString *)language
{
    if (!language) {
        return NO;
    }
    [_mParams setValue:language forKey:KEY_LANGUAGE];
    [self setChanged];
    return YES;
}
- (BOOL)setVoiceField:(NSString *)voiceField
{
    if (!voiceField) {
        return NO;
    }
    
    [_mParams setValue:voiceField forKey:KEY_VOICE_FIELD];
    [self setChanged];
    return YES;
}

- (void)setSampleRate:(NSString *)sampleRate
{
    if ([sampleRate isEqualToString:SAMPLE_RATE_8K]) {
         [_mParams setValue:SAMPLE_RATE_16kto8K  forKey:SAMPLE_RATE];
    }
    else
        [_mParams setValue:sampleRate  forKey:SAMPLE_RATE];

    [self setChanged];
}

- (BOOL)isEnabled
{
    return _mIsEnabled;
}

- (void)setEnabled:(BOOL)enabled
{
    _mIsEnabled  = enabled;
}

- (BOOL)setModelType:(NSString *)value
{
    [_mParams setValue:value forKey:MODEL_TYPE];
    [self setChanged];
    return TRUE;
}
- (void)setSampleRate8k
{
    [self setSampleRate:SAMPLE_RATE_8K];
    _mSampleRate = RATE_8K;
}

- (void)setSampleRateReal8k
{
    [self setSampleRate:SAMPLE_RATE_8K];
    _mSampleRate = RATE_REAL_8K;
}

- (void)setSampleRate16k
{
    [self setSampleRate:SAMPLE_RATE_16K];
    _mSampleRate = RATE_16K;
}

- (void)reset
{
    [_mStringParam deleteCharactersInRange:NSMakeRange(0, _mStringParam.length)];
    [_mParams removeAllObjects];
}

- (NSString *)toString
{
    [self update];
    return _mStringParam;
}

- (int)getRate
{
    return _mSampleRate;
}

- (BOOL)setSampleRateInt:(int)rate
{
    switch(rate) {
        case 8000:
            [self setSampleRate8k];
            return true;
        case 16000:
            [self setSampleRate16k];
            return true;
        case 80000:
            [self setSampleRateReal8k];
            return true;
    }
    return  FALSE;
}

#pragma mark - private method
- (void)setChanged
{
    _mIsChange = true;
}

- (void)update
{
    if(_mIsChange) {
        _mIsChange = FALSE;
        [_mStringParam deleteCharactersInRange:NSMakeRange(0, _mStringParam.length)];

        for (NSString *key in _mParams.keyEnumerator) {
            [_mStringParam appendFormat:@"%@:",key];
            [_mStringParam appendFormat:@"%@\n",[_mParams valueForKey:key]];
        }
    }
}
@end
