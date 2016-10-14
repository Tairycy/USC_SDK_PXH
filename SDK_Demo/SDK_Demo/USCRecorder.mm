//
//  USCRecorder.m
//  usc
//
//  Created by hejinlai on 12-11-16.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//
#import "USCRecorder.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

static const int bufferByteSize = 3200;
static  int sampeleRate = 16000;
static const int bitsPerChannel = 16;

@interface USCRecorder()
@property (nonatomic, assign) int netType;
@property (nonatomic, assign) int sampleRate;
@property (nonatomic, strong) NSMutableArray *recordingQueue;
@property (nonatomic, strong) NSMutableData *recordingDatas;
@property (nonatomic, assign) BOOL isRecording;
@property (nonatomic,assign) BOOL isSpeechStart;
@property (nonatomic,strong) NSMutableArray *originalRecordDataArray;
@property (nonatomic,assign) NSInteger  readedLen; // 已经读取的长度
@end

@implementation USCRecorder


#pragma mark -
#pragma mark public

- (int)openAudioIn
{
    [self startRecording:nil];
    return 0;
}

- (int)readData:(NSMutableData *)buffer size:(int)size
{
    if (NO == _isRecording) {
        return -1;
    }
    
    @synchronized(self.recordingDatas) {
        NSInteger lastedLen = self.recordingDatas.length - self.readedLen;
        if (lastedLen == 0 ) {
            return 0;
        }
        
        if (lastedLen < size) {
          [buffer appendData:[self.recordingDatas subdataWithRange:NSMakeRange(self.readedLen, lastedLen)]];
            self.readedLen += lastedLen;
            return (int)lastedLen;
        }else{
            [buffer appendData:[self.recordingDatas subdataWithRange:NSMakeRange(self.readedLen, size)]];
            self.readedLen += size;
            return size;
        }
    }
}

- (void)closeAudioIn
{
    [self stopRecording];
}


- (int)numOfArray
{
    return (int)self.originalRecordDataArray.count;
}

#pragma mark -
#pragma mark private 
#warning 请尽量不要修改下面的方法
- (id)init
{
    if (self = [super init])
    {
        AudioSessionInitialize(NULL, NULL, NULL,  (__bridge void *)self);

        _recordingDatas = [[NSMutableData alloc] init];
        self.isRecording = NO;
        self.isSpeechStart = NO;
        _sampleRate = sampeleRate;
    }
    return self;
}

- (void)setupAudioFormat:(UInt32)inFormatID SampleRate:(int)sampeleRate
{
    memset(&_recordFormat, 0, sizeof(_recordFormat));
    _recordFormat.mSampleRate = sampeleRate;
    
	UInt32 size = sizeof(_recordFormat.mChannelsPerFrame);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareInputNumberChannels, &size, &_recordFormat.mChannelsPerFrame);
	_recordFormat.mFormatID = inFormatID;
	if (inFormatID == kAudioFormatLinearPCM){
        
		// if we want pcm, default to signed 16-bit little-endian
		_recordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
		_recordFormat.mBitsPerChannel = bitsPerChannel;
		_recordFormat.mBytesPerPacket = _recordFormat.mBytesPerFrame = (_recordFormat.mBitsPerChannel / 8) * _recordFormat.mChannelsPerFrame;
		_recordFormat.mFramesPerPacket = 1;
	}
}
- (void)setSampleRate:(int)sampleRate
{
    _sampleRate = sampleRate;
}
void inputBufferHandler(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp *inStartTime,
                        UInt32 inNumPackets, const AudioStreamPacketDescription *inPacketDesc)
{
    USCRecorder *recorder = (__bridge USCRecorder *)inUserData;
    if (recorder == nil){
        return;
    }

    if ((inNumPackets > 0) && (recorder.isRecording))
    {
        int _pcmSize = inBuffer->mAudioDataByteSize;
        char *_pcmData = (char *)inBuffer->mAudioData;
        
        NSData *data = [[NSData alloc] initWithBytes:_pcmData length:_pcmSize];

        [recorder addRecordDataToArray:data];

        // get volume
        AudioQueueLevelMeterState meters[1];
        UInt32 dlen = sizeof(meters);
        OSStatus Status = AudioQueueGetProperty(inAQ,kAudioQueueProperty_CurrentLevelMeterDB,meters,&dlen);
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}
- (void) startRecording:(id<USCRecorderDelegate>) delegate
{
    _isSpeechStart = NO;
    
    [_recordingDatas setLength:0];
    self.readedLen = 0;

    OSStatus error;

    // set session
    error = AudioSessionSetActive(true);
    if (error) {
        return;
    }

    UInt32 category = kAudioSessionCategory_PlayAndRecord;
    error = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetooth error:nil];
    
    if (error)
    {
        return;
    }

    if (self.sampleRate > 0) {

    }
    // 录音格式
    [self setupAudioFormat:kAudioFormatLinearPCM SampleRate:self.sampleRate];

    // 设置回调函数
    error = AudioQueueNewInput(&_recordFormat, inputBufferHandler, (__bridge void *)self , NULL , NULL, 0 , &_audioQueue);
    if (error) {
        return;
    }

    // enalbe metering to get volume
    UInt32 on = 1;
    AudioQueueSetProperty(_audioQueue, kAudioQueueProperty_EnableLevelMetering, &on, sizeof(on));
    
    // 创建缓冲器
    for (int i = 0; i < kNumberAudioQueueBuffers; ++i){
        error = AudioQueueAllocateBuffer(_audioQueue, bufferByteSize, &_audioBuffers[i]);
        if (error) {
            return;
        }
        error = AudioQueueEnqueueBuffer(_audioQueue, _audioBuffers[i], 0, NULL);
        if (error) {
            return;
        }
    }
    
     // 开始录音
    _isRecording = YES;

    error = AudioQueueStart(_audioQueue, NULL);
    if (error) {
        return;
    }

}

- (void)addRecordDataToArray:(NSData *)data
{
    @synchronized(self.originalRecordDataArray)
    {
        [self.originalRecordDataArray addObject:data];
    }
    
    @synchronized(self.recordingDatas) {
        [self.recordingDatas appendData:data];
    }
}

- (void)cancelRecording
{
    [self stopRecording];
}

- (void)stopRecording
{
    if (_isRecording) {
        _isRecording = NO;
        AudioQueueStop(_audioQueue, true);
        AudioQueueDispose(_audioQueue, true);
        AudioSessionSetActive(NO);
    }
}

#pragma mark -
#pragma mark lazy init

- (NSMutableArray *)originalRecordDataArray
{
    if (!_originalRecordDataArray) {
        _originalRecordDataArray = [NSMutableArray array];
    }
    return _originalRecordDataArray;
}
#pragma mark -
#pragma mark release
- (void) dealloc
{
    _isSpeechStart = NO;
    _isRecording = NO;
    
    AudioQueueStop(_audioQueue, true);
    AudioQueueDispose(_audioQueue, true);
}
@end
