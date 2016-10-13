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
 *  开始说话回调
 */
- (void)onSpeechStart;
- (void) onUpdateVolume:(int)volume;
- (void) onVADTimeout;
- (void) onRecordingStart:(int)errorCode;
- (void) onRecordingStop:(NSMutableData *)recordingDatas;
- (BOOL) isAllowedPlayBeep;
@end

@interface USCRecorder : USCIAudioSource
{
    AudioQueueRef				_audioQueue;
    AudioQueueBufferRef			_audioBuffers[kNumberAudioQueueBuffers];
    AudioStreamBasicDescription	_recordFormat;
    NSTimer *volumeTimer;
}

@property (nonatomic, weak) id<USCRecorderDelegate> delegate;
@property (nonatomic, assign) int netType;
@property (nonatomic, assign) int sampleRate;
@property (nonatomic, strong) NSMutableArray *recordingQueue;
@property (nonatomic, strong) NSMutableData *recordingDatas;
@property (nonatomic, assign) BOOL isRecording;

// 是否开始说话
@property (nonatomic,assign) BOOL isSpeechStart;

- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime;
- (void) startRecording:(id<USCRecorderDelegate>) delegate;
- (void) stopRecording;
- (void) cancelRecording;

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
