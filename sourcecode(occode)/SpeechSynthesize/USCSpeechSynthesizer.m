//
//  USCOnlineTTS.m
//  tts_online_test2
//
//  Created by yunzhisheng-zy on 14-12-4.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCSpeechSynthesizer.h"
#import "USCTTSEngine.h"
#import "USCLog.h"
#import "USCPlayThread.h"
#import "USCTTSErrorCode.h"
#import "USCSpeechConstant.h"
#import "USCIAudioSource.h"
//#import "USCOfflineTTS.h"
//#import "USCOfflineEngine.h"
// offline synthesize thread
//#import "USCSynthesisThread.h"
//#import "USCTtsEngineParam.h"

#define MAXBUFFERSIZE 6000

// 状态
typedef enum{
    USCSynthesizeStatusReady = 1,
    USCSynthesizeStatusSynthesizing = 2,
    USCSynthesizeStatusPlaying ,
    USCSynthesizeStatusPause,
    USCSynthesizeStatusStopped
}USCSynthesizeStatus;

@interface USCSpeechSynthesizer()<USCTTSEngineDelegate,USCPlayThreadDelegate>
{
    @private
    // 合成引擎
    // appkey
    NSString *_appkey;
    USCTTSEngine *_engine;
    NSString *_secret;

    // brief  播放器
    USCPlayThread *_playThread;
    // brief  当前合成能否取消
    BOOL canCancel;

    // 是不是公有云
    BOOL isPublic;
    // 设置的私有地址
    NSString *privateAddress;

    NSString *_vol;
    NSString *_spd;
    NSString *_vcn;
}

// 是否使用离线合成(yes 表示离线,no 表示在线)
@property (nonatomic,assign) BOOL isOfflineTTSEnable;
// 用来测试保持到文件syn.pcm
@property (nonatomic,strong) NSMutableData *testData;
// 保存下次播放的音频数据
@property (nonatomic,strong) NSMutableArray  *needPlayDataArray;
@property (nonatomic,assign) BOOL synOnly;
// 表示当前tts 状态READY = 1, SYNTHESIZING = 2, PAUSE = 3, PLAYING = 4, STOPPED = 5
@property (nonatomic,assign) USCSynthesizeStatus currentStatus;
//  audiosource
@property (nonatomic,strong)  USCIAudioSource *playAudioSource;
//  离线tts
//@property (nonatomic,strong) USCOfflineTTS *offlinetts;
// 离线模型路径
@property (nonatomic,strong) NSString *modelPath;
// 供离线合成使用的队列
@property (nonatomic,strong) NSOperationQueue *operationQueue;
// 离线合成线程对象
//@property (nonatomic,strong) USCSynthesisThread *offlineSynthesisThread;

@end

@implementation USCSpeechSynthesizer
- (NSOperationQueue *)operationQueue
{
    if (!_operationQueue) {
        _operationQueue = [[NSOperationQueue alloc]init];
    }
    return _operationQueue;
}

#pragma mark  - public method
- (id)initWithAppkey:(NSString *)appkey secret:(NSString *)secret
{
    if (self = [super init]) {
        _appkey = appkey;
        _secret = secret;
//        _playThread = [[USCPlayThread alloc]init];
//        _playThread.delegate = self;
        canCancel = NO;
        isPublic = YES;
        _synOnly = NO;

        // set synthesize default value
        _vol = @"50";
        _spd = @"50";
        _vcn = @"xiaoli";
        _currentStatus = USCSynthesizeStatusReady;
        _isOfflineTTSEnable = NO;

    }
    return self;
}

#pragma mark - new interface

- (void)synthesizeText:(NSString *)utfTxt
{
    self.synOnly = YES;
    self.needPlayDataArray = [NSMutableArray array];
    [self playText:utfTxt];
}

- (int)playText:(NSString *)utfTxt
{
    self.synOnly = NO;

    self.isOfflineTTSEnable = NO;

    [USCLog log_usc:@"%s",__func__];
    if (self.isOfflineTTSEnable) {
//        [self offlinettsPlay:utfTxt];
    }
    else{
        [self speaking:utfTxt];
    }
    return 0;
}

- (int)loadModel:(NSString *)dataFile
{
    if (dataFile && dataFile.length > 0) {
        self.modelPath = dataFile;
        [self doEvent:USC_TTS_EVENT_MODEL_LOAD];
        return 0;
    }
    return -1;
}

- (void)playSynWav
{
    if(self.needPlayDataArray){
        for (NSData *data in self.needPlayDataArray) {
            [_playThread addPlayData:data];
        }
    }
}

- (void)stop
{
    [self cancelSpeaking];
}

- (void)resume
{
    [self resumeSpeaking];
}

- (void)pause
{
    [self pauseSpeaking];
}

- (void)setOption:(int)key value:(id)value
{
        switch (key) {
            case USC_TTS_KEY_VOICE_SPEED:
                _spd = value;
                break;
            case USC_TTS_KEY_VOICE_VOLUME:
                _vol = value;
                break;
            case USC_TTS_KEY_VOICE_NAME:
                _vcn = value;
                break;
            case USC_TTS_KEY_TYPE:
            {
                if ([@"net" isEqualToString:(NSString *)value]) {
                    _isOfflineTTSEnable = NO;
                }

                if ([@"fix" isEqualToString:(NSString *)value]) {
                    _isOfflineTTSEnable = YES;
                }
            }
                break;
            default:
                break;
        }
}

/**
 *  获取合成相关参数设置
 */
- (id)getOption:(int)key
{
    if (_engine) {
        switch (key) {
        case USC_TTS_KEY_VOICE_SPEED:
                break;
                
            default:
                break;
        }
    }
    return nil;
}

/**
 *  判断当前状态 READY, SYNTHESIZING, PAUSE, PLAYING, STOPPED
 */
- (int)getStatus
{
    return self.currentStatus;
}

- (int)setAudioSource:(USCIAudioSource *)audioSource
{
    if (audioSource) {
        self.playAudioSource = audioSource;
        return 0;
    }
    return -1;
}

#pragma mark - private method
// 离线播放
//- (void)offlinettsPlay:(NSString *)txt
//{
//    [self.operationQueue cancelAllOperations];
//    self.offlineSynthesisThread = nil;
//
//    [USCLog log_usc:@"%s",__func__];
////    NSLog(@"调用play方法--%@",[NSThread currentThread]);
//
//    USCSynthesisThread *synthesisThread = [[USCSynthesisThread alloc]init];
//    synthesisThread.delegate = self;
//    self.offlineSynthesisThread = synthesisThread;
//
//    /* 1.合成线程 */
//    if (txt.length > 0) {
//        self.offlineSynthesisThread.synthesisText = txt;
//    }
//
//    if (self.modelPath.length > 0) {
//        self.offlineSynthesisThread.modelPath = self.modelPath;
//    }
//
//    /* 2.播放线程 */
////    NSLog(@"synthread = %@",self.offlineSynthesisThread);
//    /* 3.开始执行 */
//    [self.operationQueue addOperation:self.offlineSynthesisThread];
//}

- (void)speaking:(NSString *)text
{
    // 1.先停止上一次任务
    [self stop];
    NSString *msg = @"";
    if (!text || text.length == 0) {
        [self doType:USC_TTS_ERROR Error:[NSError errorWithDomain:@"没有要合成的内容" code:ERROR_SYNRHESISTEXT userInfo:nil]];
        return;
    }

    // 2.每次都重新创建新的合成引擎线程
    if(_engine){

        USCTTSEngine *temp = _engine;
        _engine = nil;
        [temp cancel];
    }
    canCancel = YES;
    _engine = [[USCTTSEngine alloc]initWithAppkey:_appkey secret:_secret];
    _engine.vol = _vol;
    _engine.spd = _spd;
    _engine.vcn = _vcn;

    // 判断是不是私有云
    if (!isPublic) {
        [_engine setTTSAddress:privateAddress public:NO];
    }

    _engine.delegate = self;
    [_engine start:text];
}

- (void)pauseSpeaking
{
    [_playThread pause];
}

- (void)resumeSpeaking
{
    [_playThread resume];
}

- (void)cancelSpeaking
{
    [self cancelSyn];
    [self cancelPlay];
}

// 取消合成,本次合成取消
- (void)cancelSyn
{
    if (canCancel) {
        [_engine cancel];
    }
}

// 取消播放,本次播放取消
- (void)cancelPlay
{
    if (canCancel) {
        [_playThread requestStop];
    }
}

- (BOOL)setProperty:(USCSynthesizeParam)property value:(NSString *)value
{
    if (!value) {
        return NO;
    }

    switch (property) {
        case USCSynthesizeParam_Volume:
            _vol = value;
            break;
        case USCSynthesizeParam_Speed:
            _spd = value;
            break;
        case USCSynthesizeParam_VoiceName:
            _vcn = value;
            break;
        default:
            break;
    }
    return true;
}

// 根据synOnly状态 决定数据是保持还是播放
- (void)appendSynData:(NSData *)data
{
    if (self.synOnly && self.needPlayDataArray) {
        [self.needPlayDataArray addObject:data];
    }
    else{
        [self writeDataToPlayer:data];
    }
}

// 把合成的播放数据给播放器播放
- (void)writeDataToPlayer:(NSData *)data
{
    [USCLog log_usc:@"%s,playthead=%@",__func__,self.playAudioSource];
    if (self.playAudioSource) {
        [self.playAudioSource writeData:data size:[data length]];
    }
}

#pragma mark -  personal public method
- (void)setTTSAddress:(NSString *)address public:(BOOL)publicCloud
{
    if (!publicCloud) {
        privateAddress = address;
        isPublic = NO;
    }
}

/*!
 *
 *  @brief  停止播放，停止合成。
 *
 */
//- (void)stop
//{
//    [_playThread requestStop];
//    [_engine stop];
//}

#pragma mark - on Delegate
- (void)doType:(int)type Error:(NSError *)error
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(onError:error:)]) {
        [self.delegate onError:0 error:error];
    }
}

- (void)doEvent:(int)type
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(onEvent:)]) {
        [self.delegate onEvent:type];
    }
}

#pragma mark - Offline TTS Delegate
#pragma mark synthesize thread
/**
 *  合成开始
 */
- (void)onSynthesisBegin
{
    [self doEvent:USC_TTS_EVENT_SYNTHESIZE_START];
}

/**
 *  合成完成
 */
- (void)onSynthesisComplete
{
    [self doEvent:USC_TTS_EVENT_SYNTHESIZE_END];
}

/**
 *  返回合成的语音数据
 *
 *  @param data 一段语音数据
 */
- (void)onSynthesizeProcessData:(NSData *)data
{
        @autoreleasepool{
        NSMutableData *mData = [[NSMutableData alloc]initWithData:data];
        int i = 0;
        int lastLen = data.length;

        while (lastLen > MAXBUFFERSIZE) {
            @autoreleasepool {
                NSData *data = [mData subdataWithRange:NSMakeRange(MAXBUFFERSIZE * i, MAXBUFFERSIZE)];

                // 判断直接播放还是
                [self appendSynData:data];

                // 剩余的数据大小
                lastLen = lastLen - MAXBUFFERSIZE;
                i++;
            }
        }// while

        if (lastLen > 0 && lastLen <= MAXBUFFERSIZE) {
            NSData *data = [mData subdataWithRange:NSMakeRange(MAXBUFFERSIZE * i, lastLen)];
            //
            [self appendSynData:data];
        }
    }
}

/**
 *  合成中途取消
 */
- (void)onSynthesisCancel
{
    [self doEvent:USC_TTS_EVENT_CANCEL];
}

/**
 *  合成中途出错
 */
- (void)onSynthesisError:(int)error
{
    NSError *err = [NSError errorWithDomain:[USCTTSErrorCode getErrorMsg:error] code:error userInfo:nil];
    [self doType:error Error:err];
}

#pragma mark - play delegate
//- (void)playBegin
//{
//    self.currentStatus = USCSynthesizeStatusPlaying;
//    [self doEvent:USC_TTS_EVENT_PLAYING_START];
//}
//
//- (void)playPause
//{
//    self.currentStatus = USCSynthesizeStatusPause;
//    [self doEvent:USC_TTS_EVENT_PAUSE];
//}
//
//- (void)playResume
//{
//    self.currentStatus = USCSynthesizeStatusPlaying;
//    [self doEvent:USC_TTS_EVENT_RESUME];
//}
//
//- (void)playEnd:(NSError *)error
//{
//    self.currentStatus = USCSynthesizeStatusStopped;
//
//    if (error) {
//        // 1.先判断是否由取消导致结束
//        if (error.code == SYNTHESIZE_CANCLEED) {
//            [self doEvent:USC_TTS_EVENT_CANCEL];
//            return;
//        }
//        [self doType:USC_TTS_ERROR Error:error];
//        return;
//    }
//    else{
//        [self doEvent:USC_TTS_EVENT_PLAYING_END];
//    }
//}
//
#pragma mark - OnlineEngine delegate
- (void)synthesizeBengin
{
    self.currentStatus = USCSynthesizeStatusSynthesizing;
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_TTS_EVENT_SYNTHESIZE_START];
    });
}


- (void)resultData:(NSData *)data
{
    self.currentStatus = USCSynthesizeStatusSynthesizing;
    // 有数据就像播放对象中添加数据，播放对象会根据数据数量自动开启播放。
    // if data length > MAXBUFFERSIZE,then divided into small pieces
    @autoreleasepool{
        NSMutableData *mData = [[NSMutableData alloc]initWithData:data];
        int i = 0;
        int lastLen = data.length;

        while (lastLen > MAXBUFFERSIZE) {
            @autoreleasepool {
                NSData *data = [mData subdataWithRange:NSMakeRange(MAXBUFFERSIZE * i, MAXBUFFERSIZE)];

                // 判断直接播放还是
                [self appendSynData:data];

                // 剩余的数据大小
                lastLen = lastLen - MAXBUFFERSIZE;
                i++;
            }
        }// while

        if (lastLen > 0 && lastLen <= MAXBUFFERSIZE) {
            NSData *data = [mData subdataWithRange:NSMakeRange(MAXBUFFERSIZE * i, lastLen)];

            //
            [self appendSynData:data];
        }
    }

    // 把合成的数据返回给代理
    dispatch_async(dispatch_get_main_queue(), ^{
        // 返回合成的音频数据
//        if (self.delegate && [self.delegate respondsToSelector:@selector(synthesizedData)]) {
//            [self.delegate synthesizedData:data];
//        }
    });

    [self.testData appendData:data];
    [self.testData writeToFile:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"syn.pcm"] atomically:YES];
}

- (void)notificatePlayerSynthesizeEnd
{
    if (self.playAudioSource) {
        [self.playAudioSource writeData:[NSData data] size:-1];
    }
    else{
        [_playThread synthesizeFinish];
    }
}

- (void)synthesiszeEnd:(NSError *)error
{
//    [USCLog log_usc:@"%s",__func__];
    _engine = nil;
    canCancel = YES;

    [self notificatePlayerSynthesizeEnd];

    if (error){
        self.currentStatus = USCSynthesizeStatusStopped;

        dispatch_async(dispatch_get_main_queue(), ^{
            [self doType:USC_TTS_ERROR Error:error];
        });
        return;
    }
    [self doEvent:USC_TTS_EVENT_SYNTHESIZE_END];
}


#pragma mark - lazy load

//- (USCOfflineTTS *)offlinetts
//{
//    if (!_offlinetts) {
//        _offlinetts = [USCOfflineTTS sharedOfflineTTS];
//        _offlinetts.delegate = self;
//    }
//    return _offlinetts;
//}

- (NSMutableData *)testData
{
    if (!_testData) {
        _testData = [NSMutableData data];
    }
    return _testData;
}
@end
