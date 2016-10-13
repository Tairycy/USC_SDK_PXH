//
//  NetWatcher.m
//  usc
//
//  Created by jiqiangsheng on 14-3-24.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "NetWatcher.h"

@implementation NetWatcher

- (id)init
{
    self = [super init];
    if (self)
    {
        self.isAutoSwitch = NO;
        self.lastSampleRate = USCSAMPLE_16K;   //该属性用来记录每次识别完以后，根据识别时间来获取的采样率
    }
    
    return self;
}

- (void)start          //开始计时，当录音结束以后，开始计时
{
//    [USCLog setAllowLogShow:NO];
    if (self.isAutoSwitch)
    {
        self.startTime = [[NSDate date] timeIntervalSince1970];
        [USCLog log_i:@"开始时间是：%f", self.startTime];
    }
    
}

- (void)stop           //当返回内容以后，停止计时
{
    if (self.isAutoSwitch)      //如果是自动切换
    {
        self.endTime = [[NSDate date] timeIntervalSince1970];
        [USCLog log_i:@"停止时间是：%f", self.endTime];
        float mRecognizeTime = self.endTime - self.startTime;
        //停止计时以后，根据识别时间发送网络切换消息
        [self switchNet:mRecognizeTime];
    }
}

- (void)switchNet:(float)recognizeTime
{
    [USCLog log_i:@"识别时间是：%f", recognizeTime];
    if (recognizeTime > BEYOND_TIME)
    {
        if (self.delegate && [self.delegate respondsToSelector:@selector(onSetNetType:)])
        {
            [self.delegate onSetNetType:USCSAMPLE_8K];
            [USCLog log_i:@"自动切换到8000"];
            self.lastSampleRate = USCSAMPLE_8K;
        }
    }
    else if ((recognizeTime > 0.0f) && (recognizeTime <= LESS_TIME))
    {
        if (self.delegate && [self.delegate respondsToSelector:@selector(onSetNetType:)])
        {
            [self.delegate onSetNetType:USCSAMPLE_16K];
            [USCLog log_i:@"自动切换到16000"];
            self.lastSampleRate = USCSAMPLE_16K;
        }
    }
    else
    {
        [self.delegate onSetNetType:self.lastSampleRate];
        [USCLog log_i:@"采样率仍然是：%f", self.lastSampleRate];
    }
}

- (void)dealloc
{
    self.delegate = nil;
}

@end
