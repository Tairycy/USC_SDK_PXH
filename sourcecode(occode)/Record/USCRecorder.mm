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
{
    int _frontTimeout;
    int _backTimeout;
}

/**
 *  保存录音数据,取走一个数据,就删除一个元素
 */
@property (nonatomic,strong) NSMutableArray *originalRecordDataArray;

/**
 *  保存录音数据,从这里取数据
 */
@property (nonatomic,strong) NSMutableData *originalRecordData;

// 保存录音数据到文件
@property (nonatomic,strong) NSMutableData *recordOriginalData;

@property (nonatomic,strong) NSMutableArray *tenMSArray;

@property (nonatomic,strong) NSMutableData *testMData;

@property (nonatomic,assign) int readedDataLen;

@end

@implementation USCRecorder

- (NSMutableData *)testMData
{
    if (!_testMData) {
        _testMData = [[NSMutableData alloc]init];
    }
    return _testMData;
}

- (NSMutableData *)originalRecordData
{
    if (!_originalRecordData) {
        _originalRecordData = [[NSMutableData alloc]init];
    }
    return _originalRecordData;
}

- (NSMutableArray *)tenMSArray
{
    if (!_tenMSArray) {
        _tenMSArray = [NSMutableArray array];
    }
    return _tenMSArray;
}


- (NSMutableData *)recordOriginalData
{
    if (!_recordOriginalData) {
        _recordOriginalData = [[NSMutableData alloc]init];
    }
    return _recordOriginalData;
}

- (NSMutableArray *)originalRecordDataArray
{
    if (!_originalRecordDataArray) {
        _originalRecordDataArray = [NSMutableArray array];
    }
    return _originalRecordDataArray;
}

- (id)init
{
    if (self = [super init])
    {
        AudioSessionInitialize(NULL, NULL, NULL,  (__bridge void *)self);
//        _asrVad = new ASRVadBeepWapper();
//        _asrVad->delegate = self;
//        _asrVad->init();

        _recordingDatas = [[NSMutableData alloc] init];
        self.isRecording = NO;
        self.isSpeechStart = NO;
        _sampleRate = sampeleRate;

        _readedDataLen = 0;

//        _frontTimeout = 300;
//        _backTimeout = 400;

//        NSData *data = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"01.pcm" ofType:nil]];
//        self.originalRecordData = [NSMutableData dataWithData:data];
//        int dataLen = data.length;
//        int i = 0;
//        while (dataLen >= 3200) {
//            [self.originalRecordDataArray addObject:[data subdataWithRange:NSMakeRange(i * 3200, 3200)]];
//            dataLen -= 3200;
//            ++i;
//        }
    }
    return self;
}

// 设置vad超时时间
//- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime
//{
//    _frontTimeout = frontTime;
//    _backTimeout = backTime;
//}

// 设置录音格式
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

//- (void) writeRecordingData:(NSData *)data
//{
//    if (!_asrVad) {
//         [USCLog log_d:@"error:asrVad = nil\n--------------"];
//        return;
//    }
//    _asrVad->write(data);
//}

- (void)recordingEnd
{
    
}

// 回调函数
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
      // old  // 回调录音数据
//        [recorder writeRecordingData:data];
        [recorder addRecordDataToArray:data];

        // 把数据保存文件
        [recorder appendRecordData:data];

        // get volume
        AudioQueueLevelMeterState meters[1];
        UInt32 dlen = sizeof(meters);
        OSStatus Status = AudioQueueGetProperty(inAQ,kAudioQueueProperty_CurrentLevelMeterDB,meters,&dlen);
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
    }
}

- (void)appendRecordData:(NSData *)data
{
    [self.recordOriginalData appendData:data];
    
    
#ifdef DEBUG
    [self.recordOriginalData writeToFile:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"recordroriginaldata.pcm"] atomically:YES];
#endif
}

- (int)numOfArray
{
    return self.originalRecordDataArray.count;
}

// 开始录音
- (void) startRecording:(id<USCRecorderDelegate>) delegate
{
//    [USCLog log_w:@"record start"];
    //动态设置代理
    _delegate = delegate;
    _isSpeechStart = NO;
    
    [_recordingDatas setLength:0];
    
    OSStatus error;
    // set session
    error = AudioSessionSetActive(true);
    if (error) {
//        [_delegate onRecordingStart:RECORDING_AUDIO_SESSION_ERROR];
        return;
    }
  
    // category，设置属性，实现边录音边播放
    ///------------
    /// //???:  这里根据不同场景应该能够进行不同设置，要开放接口
    ///------------
    UInt32 category = kAudioSessionCategory_PlayAndRecord;
    error = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
    
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetooth error:nil];
    
    if (error)
    {
//        [_delegate onRecordingStart:RECORDING_CATEGORY_ERROR];
        return;
    }
    
//    UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_Speaker;
//    error = AudioSessionSetProperty (kAudioSessionProperty_OverrideCategoryMixWithOthers, sizeof (audioRouteOverride),&audioRouteOverride);
//    if (error)
//    {
//        [_delegate onRecordingStart:RECORDING_CATEGORY_ERROR];
//        return;
//    }
//    
//    // bluetooth
//    UInt32 allowBluetoothInput = 1;
//    AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryEnableBluetoothInput,
//                            sizeof(allowBluetoothInput),
//                            &allowBluetoothInput
//       );

    if (self.sampleRate > 0) {

    }
    // 录音格式
    [self setupAudioFormat:kAudioFormatLinearPCM SampleRate:self.sampleRate];

    // 设置回调函数
    error = AudioQueueNewInput(&_recordFormat, inputBufferHandler, (__bridge void *)self , NULL , NULL, 0 , &_audioQueue);
    if (error) {
//        [_delegate onRecordingStart:RECORDING_AUDIO_INPUT_ERROR];
        return;
    }

    // enalbe metering to get volume
    UInt32 on = 1;
    AudioQueueSetProperty(_audioQueue, kAudioQueueProperty_EnableLevelMetering, &on, sizeof(on));
    
    // 创建缓冲器
    //int bufferByteSize = [self computeRecordBufferSize:&_recordFormat duration:kBufferDurationSeconds];
    for (int i = 0; i < kNumberAudioQueueBuffers; ++i){
        error = AudioQueueAllocateBuffer(_audioQueue, bufferByteSize, &_audioBuffers[i]);
        if (error) {
//            [_delegate onRecordingStart:RECORDING_ALLOC_BUFFER_ERROR];
            return;
        }
        error = AudioQueueEnqueueBuffer(_audioQueue, _audioBuffers[i], 0, NULL);
        if (error) {
//            [_delegate onRecordingStart:RECORDING_ENQUEUE_BUFFER_ERROR];
            return;
        }
    }
    
     // 开始录音
    _isRecording = YES;

    error = AudioQueueStart(_audioQueue, NULL);
    if (error) {
//        [_delegate onRecordingStart:RECORDING_AUDIO_START_ERROR];
        return;
    }
     
    // 通知录音启动成功
//    [_delegate onRecordingStart:RECOGNIZER_OK];
}

/**
 *  把数据直接添加到数组中
 *
 *  @param data 数组
 */
- (void)addRecordDataToArray:(NSData *)data
{
    @synchronized(self.originalRecordData)
    {
        [self.originalRecordData appendData:data];
    }

    @synchronized(self.originalRecordDataArray)
    {
        [self.originalRecordDataArray addObject:data];
    }
}

// 取消录音
- (void)cancelRecording
{
    _delegate = nil;
    [self stopRecording];
}

// 停止录音
- (void)stopRecording
{
    if (_isRecording) {
        _isRecording = NO;
        AudioQueueStop(_audioQueue, true);
        AudioQueueDispose(_audioQueue, true);
        AudioSessionSetActive(NO);
        [_delegate onRecordingStop:_recordingDatas];
    }
}

-(BOOL)isAllowedPlayBeep
{
    return [self.delegate isAllowedPlayBeep];
}
#pragma mark -
#pragma mark new interface

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

    // 这里每次录音buffer 最大返回3200,每次vadthread 读取3200 
    @synchronized(_originalRecordDataArray){
        if ( 0 == self.originalRecordDataArray.count && NO == _isRecording) {
            return -1;// 表示录音结束
        }

        if (self.originalRecordDataArray.count > 0) {
            [buffer appendData:[self.originalRecordDataArray objectAtIndex:0]];
//            buffer = [self.originalRecordDataArray objectAtIndex:0];
            [self.originalRecordDataArray removeObjectsInRange:NSMakeRange(0, 1)];
            return (int)buffer.length;// 正常返回数据
        }
        else{
            return 0; // 表示暂时没有录音数据
        }
    }
}

- (void)closeAudioIn
{
    [self stopRecording];
}

#pragma mark -
#pragma mark release
- (void) dealloc
{
    _isSpeechStart = NO;
    _isRecording = NO;
    
    AudioQueueStop(_audioQueue, true);
    AudioQueueDispose(_audioQueue, true);
    
//    if (_asrVad != NULL) {
//        delete _asrVad;
//        _asrVad = NULL;
//    }
    self.delegate = nil;
}
@end
