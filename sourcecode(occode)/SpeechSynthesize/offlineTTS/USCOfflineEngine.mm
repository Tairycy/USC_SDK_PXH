//
//  Engine.m
//  offline_tts_sdk
//
//  Created by yunzhisheng on 14-10-30.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#import "USCOfflineEngine.h"
#import "yzsttshandle.h"
#import "USCErrorCode.h"

using namespace std;
@interface USCOfflineEngine()
{
    void *handle;
}
@property (nonatomic,strong) NSMutableData *mData;
@end

@implementation USCOfflineEngine
- (NSMutableData *)mData
{
    if (!_mData) {
        _mData = [[NSMutableData alloc]init];
    }
    return _mData;
}

singleton_m(USCOfflineEngine)

- (id)init
{
    if(self = [super init]) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _speakingRate = @"1.0";
            _pitch = @"1.00";
        });
    }
    return self;
}
- (void)cancelProcess
{
    yzstts_cancel(handle);

//    yzstts_release(handle);

    if ([self.delegate respondsToSelector:@selector(synthesisCancel)]) {
        [self.delegate synthesisCancel];
    }
}

- (void)setSpeakingRate:(NSString *)speakingRate
{
    if (speakingRate != _speakingRate) {
        _speakingRate = speakingRate;
        yzstts_set_option(handle, 1,  [speakingRate cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

- (void)setPitch:(NSString *)pitch
{
    if (pitch != _pitch) {
        _pitch = pitch;
        yzstts_set_option(handle, 3, [pitch cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

- (BOOL)setOption:(int)ID option:(NSString *)option
{
    return yzstts_set_option(handle, ID, [option cStringUsingEncoding:NSUTF8StringEncoding]) == 0 ? YES : NO;
}

/**
 *  收到数据回调
 *
 *  @param wave     数据
 *  @param nsamples 数据长度
 */
- (void)write:(short *)wave nsample:(int)nsamples
{
//    NSLog(@"%s,合成数据= %d",__func__,nsamples);

    NSMutableData *data = [NSMutableData dataWithBytes:wave length:nsamples * 2];
    if ([self.delegate respondsToSelector:@selector(engine:synthesisData:)]) {
        [self.delegate engine:self synthesisData:data];
    }
}
//
void callback_save_wave( USCOfflineEngine *param, short* wave, int nsamples)
{
//    NSLog(@"%s--数据大小= %d",__func__,nsamples);
    USCOfflineEngine* engine = param;
    [engine write:wave nsample:nsamples];
}

- (int)synthesisData:(NSString *)text withModelPath:(NSString *)modelPath
{

//    NSLog(@"engine = %s,%s",__func__,__FILE__);
    const char* model_path = [modelPath UTF8String];
    const char * path = [modelPath cStringUsingEncoding:NSUTF8StringEncoding];
    const char *inputText = [text UTF8String];
  /***********1. create***********/
     handle = yzstts_create_singleton(path,0);
    if (!handle) {
        if ([self.delegate respondsToSelector:@selector(synthesisError:)]) {
            [self.delegate synthesisError:SYSTHESIS_ENGINE_ERROR];
        }
        return -1;
    }

 /***********2. set option***********/
//    yzstts_set_option(handle, 1, [self.speakingRate cStringUsingEncoding:NSUTF8StringEncoding]); // speaking rate
//    yzstts_set_option(handle, 3, [self.pitch cStringUsingEncoding:NSUTF8StringEncoding]); // pitch scale

     /***********3.set call back***********/
//    if (yzstts_generate_params_callback(handle, [text UTF8String], callback_save_wave, (__bridge void*)self)) {
//        if ([self.delegate respondsToSelector:@selector(synthesisComplete)]) {
//            [self.delegate synthesisComplete];
//        }
//    }
//    else
//    {
//        if ([self.delegate respondsToSelector:@selector(synthesisError:)]) {
//            [self.delegate synthesisError:SYSTHESIS_ENGINE_ERROR];
//        }
//    }

    int code =  yzstts_set_text(handle, inputText);
    if (0 > code) {
        return -1;
    }

      unsigned int samplenum = 0;
      unsigned int maxSampleNum = 16000;
      short *wavbuf = new short[maxSampleNum];
    do
    {
        samplenum = yzstts_generate_wave(handle, wavbuf, maxSampleNum);
        callback_save_wave(self, wavbuf, samplenum);
//        NSData *data = [NSData dataWithBytes:wavbuf length:sizeof(short)*samplenum];
//        if (data) {
//        }
//        [self.mData appendBytes:wavbuf length:sizeof(short)*samplenum];

    } while (samplenum > 0);


     /*********** 4. release***********/
    yzstts_release(handle);
    return 0;
}
@end
