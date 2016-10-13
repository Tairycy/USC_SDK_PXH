//
//  SynthesisThread.m
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#define kBufferSize 10240
#define kWaitTime 0.05

#import "USCSynthesisThread.h"
#import "USCOfflineEngine.h"

@interface USCSynthesisThread()<USCOfflineEngineDelegate>
{
}

@property (nonatomic,assign) BOOL isStoped;
@property (nonatomic,strong) USCOfflineEngine *engine;
@end

@implementation USCSynthesisThread

- (instancetype)init
{
    if (self = [super init]) {

        _engine = [[USCOfflineEngine alloc]init];
        _engine.delegate = self;
    }
    return self;
}

- (NSMutableArray *)audioDataArray
{
    if (!_audioDataArray) {
        _audioDataArray = [[NSMutableArray alloc]init];
    }
    return _audioDataArray;
}

- (void)setSpeakRate:(NSString *)speakRate
{
    if (speakRate != _speakRate) {
        _speakRate = speakRate;
       
        [self.engine setSpeakingRate:speakRate];
    }
}

- (void)setPicthScale:(NSString *)picthScale
{
    if (picthScale != _picthScale) {
        _picthScale = picthScale;
        [self.engine setPitch:picthScale];
    }
}

- (BOOL)setOption:(int)ID option:(NSString *)option
{
   return [self.engine setOption:ID option:option];
}

- (void)reqStop
{
    if (self.sysnthesisStatus != SynthesisCancel) {
        self.sysnthesisStatus = SynthesisCancel;
        [self.engine cancelProcess];
        [self cancel];
    }
}

- (void)destorySynthesis
{
    self.engine = nil;
    self.audioDataArray = nil;
}
/**
 *  通知代理合成取消
 */
- (void)notificateCancel
{
    if ([self.delegate respondsToSelector:@selector(onSynthesisCancel)]) {
        [self.delegate onSynthesisCancel];
    }
}

/**
 *  通知代理合成的数据
 */
- (void)notifcateSysnthesisData:(NSData *)data
{
    if ([self.delegate respondsToSelector:@selector(onSynthesizeProcessData:)])
    {
        [self.delegate onSynthesizeProcessData:data];
    }
}

- (void)main
{
//    NSLog(@"%s--%s",__FILE__,__func__);
    @autoreleasepool {
        self.sysnthesisStatus = SynthesisBegin;

        if ([self.delegate respondsToSelector:@selector(onSynthesisBegin)])
        {
            [self.delegate onSynthesisBegin];
        }

        if (self.isCancelled)
        {
            [self notificateCancel];
            return;
        }

         /***********开始TTS************/
        if (self.synthesisText && self.modelPath) {
//            NSLog(@"开启识别线程--%@",[NSThread currentThread]);
            [self.engine synthesisData:self.synthesisText withModelPath:self.modelPath];
        }
        else
        {
//            NSLog(@"条件有问题，无法tts--%@",[NSThread currentThread]);
        }
    }
}

#pragma mark - 代理
- (void)engine:(USCOfflineEngine *)engine synthesisData:(NSData *)data
{
    [self notifcateSysnthesisData:data];

//    @synchronized(self.audioDataArray)
//    {
//        [self.audioDataArray addObject:data];
////         NSLog(@"合成线程添加数据-%ld-%@",(unsigned long)data.length,[NSThread currentThread]);
//
//    }
}

- (void)synthesisError:(int)error
{
    [self setIsStoped:YES];

    if ([self.delegate respondsToSelector:@selector(onSynthesisError:)]) {

        [self.delegate onSynthesisError:error];
    }
}
- (void)synthesisCancel
{
//      NSLog(@"synthesisCancel--收到取消--%@",[NSThread currentThread]);
    [self setIsStoped:YES];
    if ([self.delegate respondsToSelector:@selector(onSynthesisCancel)]) {
        [self.delegate onSynthesisCancel];
    }
}
- (void)synthesisComplete
{
    [self setIsStoped:YES];
    self.isStoped = YES;
//      NSLog(@"synthesisComplete合成结束--%@",[NSThread currentThread]);
    if ([self.delegate respondsToSelector:@selector(onSynthesisComplete)]) {
        [self.delegate onSynthesisComplete];
    }
}
@end