//
//  PlayThread.h
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "USCIAudioSource.h"

typedef enum
{
  PlayStatusReady,
  PlayStatusPaused,
  PlayStatusPlaying,
  PlayStatusEnd
 }USCPlayStatus;

@interface USCPlayThread : USCIAudioSource
/**
 * 打开播放器
 *
 *  @return 成功返回0
 */
- (int)openAudioOut;

/**
 *  向播放器传递播放数据
 *
 *  @param buffer 数据
 *  @param size   数据大小
 *
 *  @return 返回0 边上成功
 */
- (int)writeData:(NSData *)buffer size:(int)size;

/**
 *  关闭播放器
 */
- (void)closeAudioOut;

@end
