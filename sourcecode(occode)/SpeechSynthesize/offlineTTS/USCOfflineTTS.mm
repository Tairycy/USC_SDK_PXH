//
//  OfflineTTS.m
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#import "USCOfflineTTS.h"
#import "USCSynthesisThread.h"
//#import "USCPlayThread.h"
#import "USCOfflineTTSCommon.h"
#import "USCErrorCode.h"
#import "USCLog.h"
#import <AVFoundation/AVFoundation.h>
#import "USCErrorCode.h"

#define kBgQueue dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)

@interface USCOfflineTTS()<USCSynthesisThreadDelegate>
@property (nonatomic,strong) USCSynthesisThread *synthesisThread;
//@property (nonatomic,strong) USCPlayThread *player;
@property (nonatomic,strong) NSData *data;
@property (nonatomic,strong) AVAudioPlayer *avPlayer;
@property (nonatomic,strong) NSOperationQueue *operationQueue;
@end

@implementation USCOfflineTTS

- (NSOperationQueue *)operationQueue
{
    if (!_operationQueue) {
        _operationQueue = [[NSOperationQueue alloc]init];
    }
    
    return  _operationQueue;
}

/**
 单例
 */
singleton_m(OfflineTTS)

- (id)init
{
    if(self = [super init]) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
          // ...
        });
    }
    return self;
}

#pragma mark - 对外接口
- (void)initTTSEngineWithContext:(id)context modelPath:(NSString *)modelPath
{
    if (modelPath && modelPath.length >0) {
        self.modelPath = modelPath;
    }
    if (self.modelPath && [self.delegate respondsToSelector:@selector(onInitFinish)]) {
        [self.delegate onInitFinish];
    }
}

- (void)clear
{
    self.synthesisThread = nil;
//    self.player = nil;
}

/*
 检查包名是否合法
 */
- (BOOL)checkPackageName
{
    NSBundle *bundle =[NSBundle mainBundle];
    NSDictionary *info =[bundle infoDictionary];
    NSString *packageName = [info objectForKey:@"CFBundleIdentifier"];

    if ([@"LC.GEEKERY" isEqualToString:packageName]) {
        return YES;
    }
//    if ([@"com.sogou.yyinhouse" isEqualToString:packageName]) {
//        return YES;
//    }
//    if ([@"cn.yunzhisheng.uscsdkdemo" isEqualToString:packageName]){
//        return YES;
//    }

    return NO;
}

// 返回合成
- (void)play:(NSString *)ttsString
{
        [USCLog log_usc:@"%s",__func__];
//     NSLog(@"调用play方法--%@",[NSThread currentThread]);
    /* 清空上次的线程 */
    [self releaseTTSEngine];

//    if (![self checkPackageName]) {
//        if (self.delegate && [self.delegate respondsToSelector:@selector(onError:)]) {
//            [self.delegate onError:[NSError errorWithDomain:@"包名限制错误" code:-101 userInfo:nil]];
//        }
//        return;
//    }

    USCSynthesisThread *synthesisThread = [[USCSynthesisThread alloc]init];
    synthesisThread.delegate = self;
    self.synthesisThread = synthesisThread;

//    USCPlayThread *playThread = [[USCPlayThread alloc]init];
//    playThread.delegate = self;
//    self.player = playThread;
//    self.player.playStatus = PlayUnBegin;


    /* 1.合成线程 */
    if (ttsString.length > 0) {
        self.synthesisThread.synthesisText = ttsString;
    }

    if (self.modelPath.length > 0) {
        self.synthesisThread.modelPath = self.modelPath;
    }

    /* 2.播放线程 */
//    self.player.receiveDataArray = self.synthesisThread.audioDataArray;

//    NSLog(@"synthread = %@",self.synthesisThread);
    /* 3.开始执行 */
    [self.operationQueue addOperation:self.synthesisThread];
}

/**
 *  销毁播放器和合成器
 */
- (void)releaseTTSEngine
{
    [self.operationQueue cancelAllOperations];

//    [self.player destoryPlayer];

//    self.player = nil;

    self.synthesisThread = nil;
}
/**
 *  取消
 */
- (void)cancel
{
    if (self.synthesisThread != NULL) {
        [self.synthesisThread reqStop];
    }
//    if (self.player != NULL) {
////        [self.player reqStop];
//    }

     [self releaseTTSEngine];
}
/**
 *  停止
 */
- (void)stop
{
    //    [self.mTts cancel];
    if (self.synthesisThread != NULL) {
        [self.synthesisThread reqStop];
    }

//    if (self.player != NULL)
    {
//        [self.player reqStop];
    }

    [self releaseTTSEngine];
}
- (void)setVoicePitch:(float)pitch
{
    [self.synthesisThread setPicthScale:[NSString stringWithFormat:@"%f",pitch]];
   }

- (void)setVoiceSpeed:(float)speed
{
    [self.synthesisThread setSpeakRate:[NSString stringWithFormat:@"%f",speed]];
}

- (void)setOption:(int)key value:(id)value
{
    if (!self.synthesisThread)
        return;

    [self.synthesisThread setOption:key option:value];
}

#pragma mark - play delegate
/**
 *  开始播放
 */
- (void)playBegin
{
//    NSLog(@"收到开始播放--%@",[NSThread currentThread]);
    if ([self.delegate respondsToSelector:@selector(onPlayBegin)])
    {
        [self.delegate onPlayBegin];
    }
}
/**
 *  播放取消
 */
- (void)playCancel
{
//    NSLog(@"收到播放取消--%@",[NSThread currentThread]);
    if ([self.delegate respondsToSelector:@selector(onPlayCancel)])
    {
        [self.delegate onPlayCancel];
    }
}

/**
 *  播放结束
 */
- (void)playComplete
{
//    NSLog(@"收到播放完成--%@",[NSThread currentThread]);
    if ([self.delegate respondsToSelector:@selector(onPlayComplete)]) {
        [self.delegate onPlayComplete];
    }
}

/**
 *  返回语音数据
 *
 *  @param data 语音数据
 */
- (void)playTTSData:(NSData *)data
{
//    NSLog(@"收到播放语音数据--%@",[NSThread currentThread]);

    if ([self.delegate respondsToSelector:@selector(onPlayTTSData:)]) {
        [self.delegate onPlayTTSData:data];
    }
}

- (void)playError:(int)error
{
    if ([self.delegate respondsToSelector:@selector(onError:)]) {
        NSString *msg = [USCErrorCode getErrorMsg:error];
        [self.delegate onError:[NSError errorWithDomain:msg code:error userInfo:nil]];
    }
}

#pragma mark - synthesis delegate
/**
 *  告诉播放器合成完成
 */
- (void)onSynthesisComplete
{
//    NSLog(@"收到合成完成--%@",[NSThread currentThread]);
//    [self.player setSynthesisStop];
}

- (void)onSynthesisBegin
{
//    NSLog(@"收到合成开始--%@",[NSThread currentThread]);
}
- (void)onSynthesisError:(int)error
{
    if ([self.delegate respondsToSelector:@selector(onError:)])
    {
        NSString *msg = [USCErrorCode getErrorMsg:error];
        [self.delegate onError:[NSError errorWithDomain:msg code:error userInfo:nil]];
    }
}
- (void)onSynthesisCancel
{
//    NSLog(@"收到合成取消--%@",[NSThread currentThread]);
}

- (void)onSynthesizeProcessData:(NSData *)data
{
//   if([self.delegate respondsToSelector:@selector(onBuffer)])
//   {
//       [self.delegate onBuffer];
//   }

    if ([self.delegate respondsToSelector:@selector(onSynData:)]) {
        [self.delegate onSynData:data];
    }


//    if (self.player.playStatus == PlayUnBegin) {
//
//        if (self.synthesisThread.audioDataArray.count > 3) {
//
//            dispatch_async(kBgQueue, ^{
////                 NSLog(@"startPlay方法--%@",[NSThread currentThread]);
//                [self.player startPlay];
//            });
//        }
//    }
}
@end
