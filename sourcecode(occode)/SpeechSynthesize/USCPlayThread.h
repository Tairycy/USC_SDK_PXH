//
//  PlayThread.h
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "USCTTSCommon.h"
#import "USCIAudioSource.h"

typedef enum
{
  PlayStatusReady,
  PlayStatusPaused,
  PlayStatusPlaying,
  PlayStatusEnd
 }USCPlayStatus;

@protocol USCPlayThreadDelegate <NSObject>
/**
 *  开始播放
 */
- (void)playBegin;

/**
 *  播放暂停回调
 */
- (void)playPause;

/**
 *  播放恢复回调
 */
- (void)playResume;

/*!
 *
 *  @brief  播放结束，不管是正常结束，中途取消，出错
 *
 *  @param error nil表示正常
 *
 */
- (void)playEnd:(NSError *)error;
@end

@interface USCPlayThread : USCIAudioSource

// 存放音频数据，提前装好，每次播放都保持不变
@property (nonatomic,strong) NSMutableArray *audioDataArray;

singleton_h(PLayer)

@property (nonatomic,weak) id<USCPlayThreadDelegate> delegate;

/**
 *  add audio data to receivedDataArray
 *
 *  @param data 数据
 */
- (void)addPlayData:(NSData*) data;

/**
 *  pause the play thread
 */
- (void)pause;

/*!
 *
 *  @brief  if play thread playing ,pause it
 *
 */
- (void)resume;

/**
 *  设置停止播放状态，让播放器自己自动停止
 */
- (void)requestStop;

/*!
 *
 *  @brief  表示要播放的数据全部传递完成
 *
 */
- (void)synthesizeFinish;


- (int)openAudioOut;

- (int)writeData:(NSData *)buffer size:(int)size;

- (void)closeAudioOut;

@end
