//
//  USCVADParam.m
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-13.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCVADParam.h"

#define MINBACKENG_ 5
#define MINBACKENGH_ (MINBACKENG_ + 1)  // 6
#define PITCHTH_ (MINBACKENGH_ + 1)
#define PITCHSTNUMTH_ (PITCHTH_ + 1)
#define PITCHENDNUMTH_  ( PITCHSTNUMTH_ + 1 )
#define LOWHIGHTH_ (PITCHENDNUMTH_ + 1)
#define MINSIGLEN_ (LOWHIGHTH_ +1 )
#define MAXSILLEN_ (MINSIGLEN_ +1)
#define SINGLEMAX_ (MAXSILLEN_ +1)
#define NOISE2YTH_ (SINGLEMAX_ +1 )
#define NOISE2YTHVOWEL_ (SINGLEMAX_ + 1 )
#define VOICEPROBTH_ (NOISE2YTHVOWEL_ +1 )
#define USEPEAK_ (VOICEPROBTH_ +1)
#define NOISE2YST_ (USEPEAK_ +1)
#define RESET_FEND_ (NOISE2YST_ + 1)
#define RESET_VEND_ (RESET_FEND_ +1 )
#define PITCHLASTTH_ (RESET_VEND_ +1 )

/** front是300帧(3秒) */
static   int DEFAULT_FRONT_SIL = 3000;
/** back是100帧(1秒)*/
static   int DEFAULT_BACK_SIL = 1000;

/**
 * 语音采样率
*/
static   int FREQUENCY_16K = 16000;
static   int FREQUENCY_8K = 8000;

/**
 * 缓冲语音数据长间(ms)
 */
static   int DEFAULT_CACHE_PCM_TIME = 450;

@interface USCVADParam()
{
    BOOL isFrontVadEnabled;
    BOOL isPlayStartBeep;
    int sampleRateInHz;
    int frontSil;
    int backSil;
    int pcmCacheByteSize;
    int checkBeepCacheByteSize;

    /**
     * 是否使用远讲
     */
     BOOL isFarFeildEnabled;
     int giveUpResultMinMillisecond;
     BOOL is8K216K;

    /**
     * 录音线程VAD是否可用
     */
     BOOL isVADEnabled;
}
@end

@implementation USCVADParam


+ shareVADParams
{
    static USCVADParam *instance =nil;
    static dispatch_once_t predicate;
    dispatch_once(&predicate, ^{
        instance = [[USCVADParam alloc]init];
    });
    return instance;

}

- (instancetype)init
{
    if (self = [super init]) {
      isFrontVadEnabled = false;
      isPlayStartBeep = true;
      sampleRateInHz = FREQUENCY_16K;
      frontSil = DEFAULT_FRONT_SIL;
      backSil = DEFAULT_BACK_SIL;
      pcmCacheByteSize = 0;
      checkBeepCacheByteSize = 38000;

    /**
     * 是否使用远讲
     */
    isFarFeildEnabled = false;
    giveUpResultMinMillisecond = 350;
    is8K216K =false;

    /**
     * 录音线程VAD是否可用
     */
    isVADEnabled = true;

    _MINBACKENGH = [[USCVADParamValueFloat alloc]initWithKey:MINBACKENGH_];
    _PITCHENDNUMTH = [[USCVADParamValueInt alloc]initWithKey:PITCHENDNUMTH_];
    _PITCHSTNUMTH = [[USCVADParamValueInt alloc]initWithKey:PITCHSTNUMTH_];

    _PITCHTH =[[USCVADParamValueFloat alloc]initWithKey:PITCHTH_];
    _LOWHIGHTH = [[USCVADParamValueFloat alloc]initWithKey:LOWHIGHTH_];

    _MINSIGLEN = [[USCVADParamValueInt alloc]initWithKey:MINSIGLEN_];

    _MAXSILLEN = [[USCVADParamValueInt alloc]initWithKey:MAXSILLEN_];

    _SINGLEMAX = [[USCVADParamValueFloat alloc]initWithKey:SINGLEMAX_];

    _NOISE2YTH = [[USCVADParamValueFloat alloc]initWithKey:NOISE2YTH_];

    _NOISE2YTHVOWEL = [[USCVADParamValueFloat alloc]initWithKey:NOISE2YTHVOWEL_];

    _VOICEPROBTH = [[USCVADParamValueFloat alloc]initWithKey:VOICEPROBTH_];

    _USEPEAK = [[USCVADParamValueInt alloc]initWithKey:USEPEAK_];

    _NOISE2YST = [[USCVADParamValueInt alloc]initWithKey:NOISE2YST_];

    _PITCHLASTTH =[[USCVADParamValueInt alloc]initWithKey:PITCHLASTTH_];

     /***********初始化设置为安静模式***********/
    [self setFrontCacheTime:DEFAULT_CACHE_PCM_TIME];

//    [self setVADModelToDefault];
    }
    return self;
}

/**
 * 放弃识别结果的最小语音时间
 * @param ms
 */
-  (void)setGiveUpResultMinMillisecond:(int)ms
{
    giveUpResultMinMillisecond = ms;
}

- (int)getGiveUpResultMinMillisecond
{
    return giveUpResultMinMillisecond;
}

/**
 * 是否使用远讲
 * @return 是 true, 否 false
 */
- (BOOL)isFarFeildEnabled
{
    return isFarFeildEnabled;
}

/**
 * 是否使用远讲
 * @return 是 true, 否 false
 */
- (void)setFarFeildEnabled:(BOOL)enabled
{
    isFarFeildEnabled = enabled;
}

- (void)setVADTimeout:(int)FrontSil backSil:(int)BackSil
{
    [self.MAXSILLEN setValue:(backSil / 10)];
    frontSil = FrontSil;
    backSil = BackSil;
}

-  (void)setFrontCacheTime:(int)nTime
{
    if (nTime < 100) {
        nTime = 100;
    }

    pcmCacheByteSize = sampleRateInHz / 1000 * nTime * 2;
}

- (void)setFrontVadEnabled:(BOOL)enabled
{
    isFrontVadEnabled = enabled;
}

- (BOOL)isFrontVadEnabled
{
    return isFrontVadEnabled;
}

- (BOOL)isPlayStartBeep
{
    return isPlayStartBeep;
}

- (BOOL)isCheckBeep
{
    return isPlayStartBeep;
}

- (void)setPlayStartBeep:(BOOL)enabled
{
    isPlayStartBeep = enabled;
}

/**
 *  设置录音线程VAD是否启用
 * @param enabled
 */
- (void)setVADEnabled:(BOOL)enabled
{
    isVADEnabled = enabled;
}

/**
 * 获取录音线程VAD是否启用
 * @return
 */
- (BOOL)isVADEnabled
{
    return isVADEnabled;
}

- (void)resetVAD
{
    [_MINBACKENG reset];   // min back energy, default 5e6 (float)
    [_MINBACKENGH reset]; // min back energy higher TH, just larger than this, may voice occur, 5e8(for very noise, if voice is good, set it low, like 5e6)
    [_PITCHTH reset];
    [_PITCHSTNUMTH reset];
    [_PITCHENDNUMTH reset];
    [_LOWHIGHTH reset];
    [_MINSIGLEN reset];
    [_MAXSILLEN reset];
    [_SINGLEMAX reset];
    [_NOISE2YTH reset];
    [_NOISE2YTHVOWEL reset];
    [_VOICEPROBTH reset];
    [_USEPEAK reset];
    [_NOISE2YST reset];
    [_PITCHLASTTH reset];
}

/**
 *  强噪声、强语音音量参数设置
 */
- (void)setVADModelToStrongNoiseOrVoice
{
    [self resetVAD];
    [_NOISE2YTHVOWEL setValue:.7f];
    [_NOISE2YTH setValue:.22f];
    [_PITCHTH setValue:5.f];
    [_VOICEPROBTH setValue:.6f];
    [_USEPEAK setValue:1];
    [_NOISE2YST setValue:3];
    [_MINBACKENG setValue:2E8f];
    [_MINBACKENG setValue:80.f];
    [_LOWHIGHTH setValue:1e6f];
    [_PITCHLASTTH setValue:1];
}

/**
 * 安静环境设置 A
 */
- (void)setVADModelToQuietEnvironmentA
{
    [self resetVAD];

    [_MINBACKENGH setValue:5e6f];
    [_PITCHSTNUMTH setValue:6];
}

/*!
 *  @author usc_zy, 15-01-23 17:01:33
 *
 *  @brief  初始化设置默认参数
 *
 */
- (void)setVADModelToDefault
{
    [self resetVAD];

//    [_MINBACKENGH setValue:5e6f];
//    [_PITCHSTNUMTH setValue:6];
//    [_PITCHENDNUMTH setValue:0];
}

/**
 * 安静环境设置 B
 */
- (void)setVADModelToQuietEnvironmentB
{
    [self resetVAD];
    [_PITCHTH setValue:.22f];
    [_PITCHSTNUMTH setValue:5];
    [_VOICEPROBTH setValue:.7f];
    [_USEPEAK setValue:1];
    [_MINBACKENGH setValue:5e6f];
}

/**
 * 乐视VAD（过滤小音量语音），前端需缓存350ms语音
 */
- (void)setVADModelToLetv
{
    [self resetVAD];

    // 前端需缓存450ms语音
    [_MINBACKENGH setValue:5e6f];
    [_MAXSILLEN setValue:50];
    [_PITCHSTNUMTH setValue:6];
    [_PITCHLASTTH setValue:1];
}

- (void)showParams
{
//    NSLog(@"");
}
@end
