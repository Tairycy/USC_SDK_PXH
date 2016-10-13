//
//  USCVadThread.m
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-6-24.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCVadThread.h"
#import "ASRVadBeepWapper.h"
//#import "asrclient.h"
#import "USCLog.h"

@interface USCVadThread()

@property (nonatomic,strong) NSMutableData *recordMData;
@property (nonatomic,strong) NSMutableData *vadmData;

// 数据是否经过vad处理,思路:不管如何设置都经过vad处理,只不过调用vad代理方法时,进行判断
@property (nonatomic,assign) BOOL isVADEnable;
@end

@implementation USCVadThread
- (NSMutableData *)recordMData
{
    if (!_recordMData) {
        _recordMData = [[NSMutableData alloc]init];
    }
    return _recordMData;
}

- (NSMutableData *)vadmData
{
    if (!_vadmData) {
        _vadmData = [[NSMutableData alloc]init];
    }
    return _vadmData;
}

- (instancetype)init
{
    if (self = [super init]) {
        //        _asrVAD = new ASRVadBeepWapper();
        _isVADEnable = YES;

        _asrVAD = new ASRVadBeepWapper();
        _asrVAD->init();
        _asrVAD->delegate = self;
    }
    return self;
}

#pragma mark -
#pragma mark public method
- (void)stop
{
    if (self.audioSource) {
        [self.audioSource closeAudioIn];
    }
}

- (void)start:(BOOL)enable
{
    self.isVADEnable = enable;

    if (_asrVAD) {
        _asrVAD->reset();
    }

    /***********create new asrvad every time***********/
    if (_asrVAD != NULL) {
        delete _asrVAD;
        _asrVAD = NULL;
    }

    _asrVAD = new ASRVadBeepWapper();
    _asrVAD->init();
    _asrVAD->delegate = self;
    _asrVAD->vadParams = _vadParam;
    _asrVAD->setVadTimeout(self.vadParam.frontTimeout, self.vadParam.backTimeout);


    if (!self.audioSource) {
        return;
    }

    if (self.delegate && [self.delegate respondsToSelector:@selector(onRecordingStart)]) {
        [self.delegate onRecordingStart];
    }

    [self.audioSource openAudioIn];

    int i = 1;

    while (1) {
        NSMutableData *partData = [NSMutableData data];
        int size = [self.audioSource readData:partData size:1200];
        i++;
        if(size < 0){
            // 取到负数就关闭录音
            [self.audioSource closeAudioIn];
            break;
        }
        else if(size > 0){
            [self.recordMData appendData:partData];
            _asrVAD->write(partData);
            [partData setLength:0];
        }
        else{
            [NSThread sleepForTimeInterval:0.05];
        }
    }

    [USCLog log_usc:@"reocordoriginalarray count = %d",[self.audioSource numOfArray]];
    [self.recordMData writeToFile:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"recordMData.pcm"] atomically:YES];
}

#pragma mark - delegate
- (void)onVADTimeOut
{
    if (self.isVADEnable) {
        [self.audioSource closeAudioIn];
    }
    if (self.delegate && [self.delegate respondsToSelector:@selector(onVADTimeOut)]) {
        [self.delegate onVADTimeOut];
    }
    [self.vadmData writeToFile:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"vadMdata.pcm"] atomically:YES];
}

- (void)onUpdateVolume:(int)volume
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(onUpdateVolume:)]) {
        [self.delegate onUpdateVolume:volume];
    }
}

- (void)onRecordingData:(NSData *)data isValid:(BOOL)isValid
{
    if (!self.isVADEnable) {
        NSData *vadData = [NSData dataWithData:data];
        @synchronized(self.recordingQueue){
            [self.recordingQueue addObject:vadData];
        }
    }

    if (isValid) {
        [self.vadmData appendData:data];
        NSData *vadData = [NSData dataWithData:data];
        @synchronized(self.recordingQueue){
            [self.recordingQueue addObject:vadData];
        }
    }
}

- (BOOL)isAllowedPlayBeep
{
    return YES;
}
@end
