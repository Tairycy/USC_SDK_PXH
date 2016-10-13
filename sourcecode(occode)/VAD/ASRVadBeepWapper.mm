//
//  ASRVadBeepWapper.cpp
//  usc
//
//  Created by Andy on 14-1-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#include "ASRVadBeepWapper.h"
#include "string.h"
//#import "asrclient.h"
#import "ASRVadWapper.h"
#import "USCVADParamValue.h"
#import "set_option_key.h"
#import "errorNo.h"

#define INPUT_BUFFER_LEN	(8000*2)	// 500ms
#define VAD_BUFFER_LEN		(4000*2)	// 250ms

ASRVadBeepWapper::ASRVadBeepWapper():ASRVadWapper()
{
    isBegin = NO;
    planCacheSize = BITNUM_PERSECOND * DEFAULT_CACHE_TIME;
    cacheData = [[NSMutableData alloc] init];
    tempTestData = [NSMutableData data];
}

void ASRVadBeepWapper::reset()
{
    ASRVadWapper::reset();
    isBegin = NO;
    [cacheData setLength:0];
}

//设置缓冲时间，然后通过缓冲时间计算缓冲的字节数
void ASRVadBeepWapper::setCacheTime(float cacheTime)
{
    planCacheSize = BITNUM_PERSECOND * cacheTime;
}

void ASRVadBeepWapper::write(NSData *data)
{
    if (!data)
    {
        return;
    }
    
    if (data.length <= 0)
    {
        return;
    }
    
    int volume = 0;
    int vad = isVadTimeout((char *)[data bytes], (int)data.length, &volume);
    if (this->delegate && [this->delegate respondsToSelector:@selector(onUpdateVolume:)])
    {
        [this->delegate onUpdateVolume:volume];
    }
    if (vad == VAD_TIMEOUT)
    {
//        NSLog(@"vad timeout ------");
        signed short tmp = pow(2, 15) - 1;
        [tempTestData appendBytes:&tmp length:2];
        if (this->delegate && [this->delegate respondsToSelector:@selector(onVADTimeOut)])
        {
            [this->delegate onVADTimeOut];
        }
    }
    
    frontVadCallBack(data);

//    [tempTestData appendData:data];
//    [tempTestData writeToFile:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"tempTest.pcm"] atomically:YES];
//    [delegate onRecordingData:data isValid:true];
}

void ASRVadBeepWapper::frontVadCallBack(NSData *data)
{
    //如果不播放提示音的话，那么就直接发送数据
    if (![this->delegate isAllowedPlayBeep])
    {
        [this->delegate onRecordingData:data isValid:YES];
        return;
    }
    
    //如果播放提示音
    if (isBegin) // 如果已经开始进行识别，那么就把数据直接发送
    {
        [this->delegate onRecordingData:data isValid:isBegin];
    }
    else
    {
        [cacheData appendData:data];
        if (cacheData.length >= planCacheSize)
        {
            char *cache = (char *)[cacheData bytes];
            int noisePosition = checkPitchOffset(cache, (int)cacheData.length);
            if (noisePosition > 0)
            {
                NSData *nosieData = [NSData dataWithBytes:cache length:noisePosition];
                [delegate onRecordingData:nosieData isValid:false];
            }

            // 把缓冲的数据中噪音后面的所有数据，重新写到缓冲cache中
            int leftLen = (int)cacheData.length - noisePosition;
            if (leftLen > 0)
            {
                NSData *leftdata = [cacheData subdataWithRange:NSMakeRange(noisePosition, leftLen)];
                [delegate onRecordingData:leftdata isValid:YES];
                int volume = 0;
                int vad = isVadTimeout(cache + noisePosition, leftLen, &volume);
                if (this->delegate && [this->delegate respondsToSelector:@selector(onUpdateVolume:)] && [this->delegate respondsToSelector:@selector(onVADTimeOut)])
                {
                    [this->delegate onUpdateVolume:volume];
                    if (vad ==  VAD_TIMEOUT)
                    {
                        [this->delegate onVADTimeOut];
                    }
                }
            }

            ASRVadWapper::reset(); // 当缓冲1.2s以后，为了避免提示音对录音数据的影响重置vad(这里仅仅重置vad
            isBegin = YES;
        }
    }
}

void ASRVadBeepWapper::flush()
{
    if (isBegin == false)
    {
        char *cache = (char *)[cacheData bytes];
        int noisePosition = checkPitchOffset(cache, (int)cacheData.length);
        if (noisePosition > 0)
        {
            if (noisePosition > [cacheData length])
            {
                [delegate onRecordingData:cacheData isValid:false];
            }
            else
            {
                NSData *noiseData = [NSData dataWithBytes:cache length:noisePosition];
                [delegate onRecordingData:noiseData isValid:false];
            }
        }
        
        //把缓冲的数据中噪音后面的所有数据，重新写到缓冲cache中
        int leftLen = (int)cacheData.length - noisePosition;
        if (leftLen > 0)
        {
            NSData *leftdata = [cacheData subdataWithRange:NSMakeRange(noisePosition, leftLen)];
            [delegate onRecordingData:leftdata isValid:YES];
            int volume = getVolume(cache + noisePosition, leftLen);
            if (this->delegate && [this->delegate respondsToSelector:@selector(onUpdateVolume:)])
            {
                [this->delegate onUpdateVolume:volume];
            }
        }
        isBegin = true;
    }
    [this->delegate onUpdateVolume:0];
}

ASRVadBeepWapper::~ASRVadBeepWapper()
{
    cacheData = nil;
    this->delegate = nil;
}

void  ASRVadBeepWapper::setVADParams()
{
//     setoption(vadParams.MINBACKENGH);
//     setoption(vadParams.PITCHENDNUMTH);
//     setoption(vadParams.PITCHSTNUMTH);
//     setoption(vadParams.MINBACKENG);
//     setoption(vadParams.PITCHTH);
//     setoption(vadParams.LOWHIGHTH);
//     setoption(vadParams.MINSIGLEN);
//     setoption(vadParams.MAXSILLEN);
//     setoption(vadParams.SINGLEMAX);
//     setoption(vadParams.NOISE2YTH);
//     setoption(vadParams.NOISE2YTHVOWEL);
//     setoption(vadParams.VOICEPROBTH);
//     setoption(vadParams.USEPEAK);
//     setoption(vadParams.NOISE2YST);
//     setoption(vadParams.PITCHLASTTH);
}

//int ASRVadBeepWapper::setoption(USCVADParamValueFloat *param)
//{
//    if (![param isEnabled]) {
//        return 0;
//    }
//
//    NSLog(@"float param == %f",param.value);
//    float value = param.value;
//    return setVadParams([param key],&value);
//}

//int ASRVadBeepWapper::setoption(USCVADParamValueInt *param)
//{
//    if (![param isEnabled]) {
//        return 0;
//    }
////    NSLog(@"float param == %d",param.value);
//    int intValue = param.value;
//    return setVadParams([param key ],&intValue);
//}