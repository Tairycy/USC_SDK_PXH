//
//  USCRecorder.h
//  usc
//
//  Created by hejinlai on 12-11-16.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "USCIAudioSource.h"

#define kNumberAudioQueueBuffers 3
#define kBufferDurationSeconds 0.1f

@class USCIAudioSource;

@protocol USCRecorderDelegate <NSObject>

/**
 *  录音开始
 *
 *  @param errorCode 录音开启失败返回错误码
 */
- (void) onRecordingStart:(int)errorCode;
/**
 *  录音停止
 *
 *  @param recordingDatas 返回的录音数据
 */
- (void) onRecordingStop:(NSMutableData *)recordingDatas;

@end

@interface USCRecorder : USCIAudioSource
{
    AudioQueueRef				_audioQueue;
    AudioQueueBufferRef			_audioBuffers[kNumberAudioQueueBuffers];
    AudioStreamBasicDescription	_recordFormat;
    NSTimer *volumeTimer;
}

/**
 *  打开录音设备
 *
 *  @return
 */
- (int)openAudioIn;
/**
 *  读取数据
 *
 *  @param buffer 读取数据
 *  @param size   数据大小
 *
 *  @return 返回值
 */
- (int)readData:(NSData *)buffer size:(int)size;

/**
 *  关闭录音设备
 */
- (void)closeAudioIn;
@end
