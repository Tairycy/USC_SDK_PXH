//
//  USCVadThread.h
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-6-24.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ASRVadBeepWapper.h"
#import "USCVADParam.h"
#import "USCErrorCode.h"
#import "USCIAudioSource.h"

@protocol USCVadThreadDelegate <NSObject>
- (void)onRecordingStart;
- (void)onUpdateVolume:(int)volume;
- (void)onVADTimeOut;
@end

@interface USCVadThread : NSObject<ASRVadBeepWapperDelegate>

@property (nonatomic,assign) ASRVadBeepWapper *asrVAD;
@property (nonatomic,strong) USCVADParam *vadParam;
@property (nonatomic,strong) USCIAudioSource *audioSource;
// 共享数组
@property (nonatomic, strong) NSMutableArray *recordingQueue;
@property (nonatomic,weak) id<USCVadThreadDelegate> delegate;

/**
 *  开启
 *
 *  @param enable 是否经过vad处理
 */
//- (void)start:(BOOL)enable;

//是开启vad
-(void)start;

/**
 *  停止
 */
- (void)stop;

@end
