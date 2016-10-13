//
//  USCRecognizer.m
//  usc
//
//  Created by hejinlai on 12-11-3.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "USCRecognizer.h"
#import "USCUtil.h"
#import "USCRecognition.h"
//#import "asrclient.h"
#import "Encrypter.h"
#import "USCMarcos.h"
#import "USCErrorCode.h"
#import <dispatch/dispatch.h>
#import "WaveHeader.h"
#import "USCVersion.h"
#import "BeepPlayer.h"
#import "RecognizationWatcher.h"
#import "NetWatcher.h"
#import "USCAddressGetter.h"
#import "USCAddress.h"
#import "USCAsrResultFormatCantonese.h"
#import "USCAsrResultFormatChinese.h"
#import "USCAsrResultFormatEnglish.h"
#import "USCRecoginizerParam.h"
#import "USCLocationTool.h"
#import <CoreLocation/CoreLocation.h>
#import "USCVADParam.h"
#import "USCLog.h"
#import "USCRecognitionParam.h"
#import "USCSpeechConstant.h"
#import "USCSpeechTimer.h"
#import "USCVadThread.h"


#define kBgQueue dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0)
#define kMainQueue dispatch_get_main_queue()

@interface USCRecognizer() <USCVadThreadDelegate, USCRecognitionDelegate, RecognizationWatcherDelegate, BeepPlayerDelegate, NetWatcherDelegate>
{
    // 用来保存录音数据数组
    NSMutableArray *_recordingQueue;

    NSOperationQueue *_operationQueue;
    USCRecognition *_recognition;
    //    USCRecorder *_recorder;

    // 上传用户数据的返回结果
    NSMutableData *receiveData;

    NetWatcher *_netWatcher;     //网络监视器，用于监视识别速度

    NSTimeInterval cancelTime;
    NSString *_appKey;
    NSString *_secret;
    int _netType;
    NSString *_sessionId;
    // 识别是否取消
    BOOL isRecognizerCancelled;
    int sampleRate;
    NSString *_engine; // 识别领域
    BeepPlayer *beepPlayer;
    RecognizationWatcher *recognizationWatcher;

    //是否播放提示音
    BOOL _isAllowedPlayBeep;

    NSString *serviceAddressAndPort;
    NSArray *arrayAddressAndPort;
    NSString *address, *port;

    USCAddressGetter *_addressGetter;
    USCAddress *_address;

    USCUtil *_resultFormat;

    BOOL _isPunctuation;

    NSString *_language;   //需要识别的语言

    // 识别参数（包括vad参数和识别参数）
    USCRecoginizerParam *_param;
    // vad 参数
    USCVADParam *_vadParam;

    // 当前所在的城市
    NSString *currentCity;
    // 当前的坐标
    CLLocationCoordinate2D currentCoordinate;

    // 获取地理位置
    USCLocationTool *locationTool;

    // 每次请求是否返回语义结果
    BOOL _isReturnNluResult;

    // 语义场景
    NSString *_nluScenario;
    // 设置
    BOOL _farField;

    BOOL _isVPREnable;
}
@property (nonatomic,assign) BOOL locationEnable;
@property (nonatomic,strong) USCRecognitionParam *recognitionParam;
@property (nonatomic,assign) int frontTime,backTime;
/***********new interface***********/
@property (nonatomic,assign) int volume;
@property (nonatomic,assign) USCSpeechStatus speechStatus;
@property (nonatomic,strong) USCIAudioSource *recordAudioSource;
@property (nonatomic,strong) USCVadThread *vadThread;

@property (nonatomic,strong) NSString  *asrEngine;
@end

@implementation USCRecognizer
#pragma mark - Public Method
#pragma mark new interface
- (id)initWithAppKey:(NSString *)appkey secret:(NSString *)secret
{
    if (self = [super init]) {
        _appKey = [appkey copy];
        _secret = [secret copy];
        _recordingQueue = [[NSMutableArray alloc] init];

        _operationQueue = [[NSOperationQueue alloc] init];

        _netWatcher = [[NetWatcher alloc] init];
        _netWatcher.delegate = self;
        _netWatcher.isAutoSwitch = YES;

        recognizationWatcher = [[RecognizationWatcher alloc] init];
        recognizationWatcher.delegate = self;

        isRecognizerCancelled = NO;
        sampleRate = SAMPLE_RATE_AUTO;
        _engine = USC_ENGINE_GENERAL;
        _asrEngine = USC_ENGINE_GENERAL;
        
        _locationEnable = YES;
        beepPlayer = [[BeepPlayer alloc] init];
        beepPlayer.delegate = self;
        _isAllowedPlayBeep = YES;
        locationTool = [[USCLocationTool alloc]init];
        serviceAddressAndPort = @"vpr.hivoice.cn:80";
        address = @"vpr.hivoice.cn";
        port = @"80";
        _isPunctuation = YES;
        _addressGetter = [[USCAddressGetter alloc] init];
        _language = USC_LANGUAGE_MANDARIN;           //识别语言默认是中文
        _resultFormat = [[USCAsrResultFormatChinese alloc] init];    //初始化_resultFormat，以中文初始化
        _resultFormat.isPunctuation = _isPunctuation;                //中文初始化，对应的标点符号处理

        _speechStatus = USC_Status_Ready;

        /***********创建默认参数对象***********/
        _param = [[USCRecoginizerParam alloc]init];
        _vadParam = [[USCVADParam alloc] init];
        _isReturnNluResult = YES;
        _nluScenario = @"";
        _farField = NO;
        _isVPREnable = NO;

        /***********创建vad线程***********/
        _volume = 10;
        _vadThread = [[USCVadThread alloc]init];
        _vadThread.delegate = self;
        _vadThread.recordingQueue = _recordingQueue;
    }
    return self;
}

- (int)setAudioSource:(USCIAudioSource *)audioSource
{
    if (audioSource) {
        _recordAudioSource  = audioSource;
        if (_vadThread) {
            _vadThread.audioSource = audioSource;
        }
        return 0;
    }
    return -1;
}

- (void)start
{
    //    [USCLog log_w:@"recognizer start"];
    [recognizationWatcher reset];  //每次开始录音的时候都要对识别监视器进行重置

    // 开始计时,以便计算各个事件完成的时间
    [USCSpeechTimer begine];

    // 开始定位
    if (self.locationEnable) {
        [locationTool start];
    }

    dispatch_async(kBgQueue, ^{
        isRecognizerCancelled = NO;

        // 操作过于频繁
        NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
        double diff = startTime - cancelTime;
        if (diff <= 0.3) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self doError:[NSError errorWithDomain:@"操作过快" code:-1 userInfo:nil] type:USC_ASR_NLU_ERROR_OPERATETOOFAST];
            });
            return;
        }

        // 确保清空录音数据和取消识别线程
        [self clear];

        // 开启vad处理
        if (_vadThread) {
            _vadThread.vadParam = _vadParam;
        }
        [_vadThread start:YES];
    });
}


- (void)stop
{
    dispatch_async(kBgQueue, ^{
        // 停止录音
        if (_vadThread) {
        }

        // 停止识别线程
        if (_recognition) {
            _recognition.setToStopped = YES;
        }
    });
}

- (void)cancel
{
    dispatch_async(kBgQueue, ^{
        if (!isRecognizerCancelled)
        {
            isRecognizerCancelled = YES;
            [self clear];
            cancelTime = [[NSDate date] timeIntervalSince1970];
        }
    });
}

- (id)getOption:(int)key
{
    switch (key) {
        case USC_ASR_EVENT_VOLUMECHANGE:
        {
            return [NSNumber numberWithInt:self.volume];}
            break;
        default:
            break;
    }
    return nil;
}

- (void)setOption:(int)key value:(id)value
{
    // sample,modeltype,voiceField,language,frontTime,nluenable,nluscenario,punctuation,recognitiontimeout,
    NSLog(@"setOption:%d value:%@",key,value);
    switch (key) {
        case USC_ASR_SERVICE_MODE:
            break;
        case USC_ASR_BANDWIDTH:
            [self setSampleRate:[value intValue]];
            break;
        case USC_ASR_DOMAIN:
            [self setASREngine:(NSString *)value];  //         [self setModelType:(NSString *)value];
            break;
        case USC_ASR_VOICE_FIELD:
            [self setVoiceField:(NSString *)value];
            break;
        case USC_ASR_LANGUAGE:
            [self setLanguage:(NSString *)value];
            break;
        case USC_ASR_VAD_TIMEOUT_FRONTSIL:// 前后端时间设置，修改vadparam，每次start在把vadParam传递给vadThread
            [_vadParam setFrontTimeout:[value intValue]];
            break;
        case USC_ASR_VAD_TIMEOUT_BACKSIL:
            [_vadParam setBackTimeout:[value intValue]];
            break;
        case USC_ASR_SERVER_ADDR:
            [self setProperty:value forKey:100];
            break;
        case USC_NLU_ENABLE:
            [self setNluEnable:value];
            break;
        case USC_NLU_SCENARIO:
            [self setNluScenario:(NSString *)value];//            [self.recognitionParam setNluScenario:(NSString *)value];
            break;
        case USC_ASR_NET_TIMEOUT:
            [self setRecognizationTimeout:[value floatValue]];
            break;
        case USC_ASR_RECOGNIZER_PARAM:
            [self setParam:(USCRecoginizerParam *)value];
            break;
        case USC_ASR_LOCATION_ENABLE:
            [self setLocation:(NSString *)value];
            break;
        case USC_ASR_NET_PNUCTUATION:
            [self setPunctuation:(NSString *)value];
            break;
        default:
            break;
    }
}

- (void)setLocation:(NSString *)location
{
    if ([(NSString *)location isEqualToString:@"true"]) {
        _locationEnable = YES;
    }
    else{
        _locationEnable = NO;
    }
}

- (void)setPunctuation:(NSString *)str
{
    if ([str isEqualToString:@"true"]) {
        [self.recognitionParam setIsPunctuation:YES];
    }
    else
    {
        [self.recognitionParam setIsPunctuation:NO];
    }
}

- (int)init:(NSString *)jsonString
{
    if (jsonString ) {
        return -1;
    }

    NSError *error;
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:[jsonString dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingAllowFragments error:&error];
    for (NSString *key in jsonDict) {
        [self setJsonKey:key value:[jsonDict valueForKey:key]];
    }
    return 0;
}
//???: 需要完善
- (int)postInfo:(NSString *)jsonString
{
    return 0;
}

- (void)uploadUserData:(NSDictionary *)userData
{
    [self setUserData:userData];
}

- (int)getStatus
{
    return self.speechStatus;
}

#pragma mark - Private Method
#pragma mark  set property method
/**
 *  set asr parameter base on json string
 *
 *  @param key   json key
 *  @param value json value
 */
- (void)setJsonKey:(NSString *)key value:(NSString *)value
{
    if ([key isEqualToString:@"engine"]) {
//        [self setEngine:value];
    }

    if ([key isEqualToString:@"sample"]) {
        [self setSampleRate:[value intValue]];
    }

    if ([key isEqualToString:@"language"]) {
        [self setLanguage:value];
    }

    if ([key isEqualToString:@"voicefield"]) {
        [self setVoiceField:value];
    }
}

/**
 *  设置asr 参数
 *
 *  @param param asr 参数,vad参数是单例
 */
- (void)setParam:(USCRecoginizerParam *)param
{
    if (param) {
        _param = param;
        _param.vadParam = _vadParam;
    }
}

/**
 *  设置vad前后端点
 *
 *  @param frontTime 前端点
 *  @param backTime  后端点
 */
- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime
{
    self.frontTime = frontTime / 10;
    self.backTime = backTime / 10;
}

- (void)setVADFrontTimeout:(int)frontTime
{
    _vadParam.frontTimeout = frontTime;
}

- (void)setVADBackTimeout:(int)frontTime
{
    _vadParam.frontTimeout = frontTime;
}

/**
 *  设置采样率
 *
 *  @param rate 采样率
 */
- (void)setSampleRate:(int)rate
{
    if (rate == 8000 || rate == 16000) {
        sampleRate = rate;
        [_param setSampleRate:rate];
        _netWatcher.isAutoSwitch = NO;
    }
    else
    {
        sampleRate = 16000;
        [_param setSampleRate:16000];
        _netWatcher.isAutoSwitch = YES;
    }
}

/*!
 *  @brief  最新的设置语言的方法
 *
 *  @param language 语言
 */
- (void)setLanguage:(NSString *)language
{
    if ([language isEqualToString:@"chinese"]) {
        [_param setLanguage:@"cn"];
    }else if ([language isEqualToString:@"english"])
    {
        [_param setLanguage:@"en"];
    }else if ([language isEqualToString:@"cantoness"])
    {
        [_param setLanguage:@"co"];
    }
}

//  设置语言暂不开放
- (void)setRecognizeLanguage:(USCRecognizeLanguage)language
{
    if (language == USCRecognizeLanguage_CN) {
        [_param setLanguage:@"cn"];
    }
    else if (language == USCRecognizeLanguage_EN)
    {
        [_param setLanguage:@"en"];
    }else if (language == USCRecognizeLanguage_CO)
    {
        [_param setLanguage:@"co"];
    }
}

/**
 *  设置远近讲
 *
 *  @param voiceField far ,near
 *
 *  @return 是否
 */
- (BOOL)setVoiceField:(NSString *)voiceField
{
    return [_param setVoiceField:voiceField];
}

/*
 * 设置属性值,目前用来设置服务器地址
 */
- (void)setProperty:(NSString *)property forKey:(int)key
{
    switch (key)
    {
        case USC_SERVICE_ADDRESS_PORT:
        {
            serviceAddressAndPort = property;
            if (arrayAddressAndPort)
                arrayAddressAndPort = nil;

            arrayAddressAndPort = [serviceAddressAndPort componentsSeparatedByString:@":"];
            if ([arrayAddressAndPort count] == 2)
            {
                if (address)
                {
                    address = nil;
                }
                address = [[arrayAddressAndPort objectAtIndex:0] copy];
                if ([address isEqual:@""] || !address)
                {
                    address = @"vpr.hivoice.cn";
                }
                if (port)
                {
                    port = nil;
                }
                port = [[arrayAddressAndPort objectAtIndex:1] copy];
                if ([port intValue] == 0)
                {
                    port = @"80";
                }
            }
        }
            break;

        default:
            break;
    }
}

/*
 设置识别领域
 */
//- (BOOL)setEngine:(NSString *)engine
//{
//    return  [self setModelType:engine];
//}

- (void)setASREngine:(NSString *)asrEngine
{
    if (!asrEngine) {
        return;
    }
    self.asrEngine = asrEngine;
}

- (void)setAsrEngine:(NSString *)asrEngine
{
    if (!asrEngine) {
        _asrEngine = USC_ENGINE_GENERAL;
    }
    _asrEngine = asrEngine;
}

/**
 *  设置识别领域,同上
 *
 *  @param modelType 领域
 *
 *  @return yes no
 */
- (BOOL)setModelType:(NSString *)modelType
{
    if (!modelType) {
        return NO;
    }
    _engine = modelType;
    _asrEngine = modelType;
    return YES;
}
/**
 *  设置识别超时时间
 *
 *  @param recognizationTime 超时时间
 */
- (void)setRecognizationTimeout:(float)recognizationTime
{
    if (recognizationTime < 5.0f || recognizationTime > 60.0f) {
        return;
    }
    recognizationWatcher.timeOutLimit = recognizationTime;
}

-(void)setPlayingBeep:(BOOL)isAllowed
{
    _isAllowedPlayBeep = isAllowed;
}

//???:06-05
// 设置语言对应的标点符号处理，并保持标点符号法则不变
//- (void) setResultFormat:(NSString *)language
//{
//    if (_resultFormat)
//    {
//        _resultFormat = nil;
//    }
//
//    if ([language isEqualToString:@"english"])
//    {
//        _resultFormat = [[USCAsrResultFormatEnglish alloc] init];
//    }
//    else if ([language isEqualToString:@"cantonese"])
//    {
//        _resultFormat = [[USCAsrResultFormatCantonese alloc] init];
//    }
//    else
//    {
//        _resultFormat = [[USCAsrResultFormatChinese alloc] init];;
//    }
//
//    // 当切换语言的时候，保持之前的标点符号法则不变
//    _resultFormat.isPunctuation = _isPunctuation;
//}

/*
 *获取session id
 */
- (NSString *)getSessionId
{
    return _sessionId;
}

+ (NSString *)getVersion
{
    return sdk_version;
}

#pragma mark Other Method
- (void)clear
{
    // 停止录音
    //    [_recorder cancelRecording];

    // 取消识别线程
    _recognition.delegate = nil;
    [_operationQueue cancelAllOperations];
    @synchronized(_recordingQueue){
        [_recordingQueue removeAllObjects];
    }
}

- (void)recognizeAudioFile:(NSString *)audioFilePath
{
    NSMutableData *fileData = [NSMutableData dataWithContentsOfFile:audioFilePath];
    if (!fileData && fileData.length <= 0) {
        NSLog(@"音频文件路径错误，找不到文件");
        NSString *msg = [USCErrorCode getErrorMsg:ASR_RECOGNIZE_AUDIOFILE_ERROR];
        NSError *error = [NSError errorWithDomain:msg code:ASR_RECOGNIZE_AUDIOFILE_ERROR userInfo:nil];
        [self doError:error type:ASR_RECOGNIZE_AUDIOFILE_ERROR];
        return;
    }
   
//    ZYLog(@"取到音频文件");
    [self splitAudioFileDataToArray:fileData];
    // 先释放上一次分配的资源
    if (_recognition) {
        _recognition = nil;
    }
    
#ifdef DEBUG
    NSLog(@"&&&&-%@",self.recognitionParam.nluScenario);
#endif
    // 分配新的识别线程
    _recognition = [[USCRecognition alloc] init];
    _recognition.delegate = self;
    _recognition.recordingQueue = _recordingQueue;
    [_recognition setRecognitionParam:self.recognitionParam];
    // 创建识别对象
    _recognition.setToStopped = YES;

    [_operationQueue addOperation:_recognition];
    self.speechStatus = USC_Status_Recognizing;
}

//处理个性化标签
- (NSMutableString *)handleUserData:(NSDictionary *)userData result:(NSMutableString *)result frontLabel:(NSString *)frontLabel backLabel:(NSString *)backLabel value:(NSArray *)value
{
    NSMutableString *lastResult = result;
    //    [userData retain];
    [result appendString:frontLabel];
    for (id str in value)
    {
        if (![str isKindOfClass:[NSString class]])
        {
            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_INVALID_FORMAT];
            if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)])
            {
                return lastResult;
            }
        }

        if (str && [str length] > 0)
        {
            NSString *label = [_resultFormat escapeBrackets:str];
            if (label && [label length] > 0)
            {
                [lastResult appendString:label];
                [lastResult appendString:@"\n"];
            }
        }
    }
    [lastResult appendString:backLabel];
    return lastResult;
}

-(BOOL)isAllowedPlayBeep
{
    return _isAllowedPlayBeep;
}

// 加密用户数据然后发送到服务器
- (void)encodeUploadUserData:(NSString *)data
{
    char * imei = NULL;
    NSString *macAddress = [USCUtil getLocalMacAddress];
    //USCLog(@"upload userdata -> imei=%@", macAddress);
    if (macAddress) {
        imei = const_cast<char*>([macAddress UTF8String]);
    }else{
        imei = const_cast<char*>("unknow iOS device");
    }
    int imeiLen = (int)strlen(imei);
    char *sourceData = const_cast<char*>([data UTF8String]);
    int sourceDataLen = (int)strlen(sourceData);

    int encodeDataLen = imeiLen + sourceDataLen + 10;
    char *encodeData = (char *)malloc(encodeDataLen);

    int encode_result = Encrypter::EncodeContent(imei, imeiLen, sourceData, sourceDataLen, encodeData, encodeDataLen);
    if (encode_result < 0) {
        NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_ENCODE_ERROR];
        if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
            //            [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_ENCODE_ERROR userInfo:nil]];
        }
        return;
    }

    NSURL *url = [NSURL URLWithString:@"http://v2.hivoice.cn:8081/casr/upload"];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url
                                                           cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                       timeoutInterval:30.0];
    [request setHTTPMethod:@"POST"];
    [request setHTTPBody:[NSData dataWithBytes:encodeData length:encodeDataLen]];

    NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:request delegate:self];
    if (theConnection) {
        receiveData = [[NSMutableData alloc] init];
    }else{
        receiveData = nil;
    }
    free(encodeData);
}

- (void)splitAudioFileDataToArray:(NSData *)fileData
{
    int n = 0;
    NSUInteger lastLen = fileData.length;
    while (lastLen > AUDIOBUFFERSIZE ) {
        NSData *data = [fileData subdataWithRange:NSMakeRange(AUDIOBUFFERSIZE * n, AUDIOBUFFERSIZE)];
        [_recordingQueue addObject:data];
        lastLen -=AUDIOBUFFERSIZE;
        n++;
    }

    if (lastLen > 0) {
        NSData *data = [fileData subdataWithRange:NSMakeRange(AUDIOBUFFERSIZE * n, lastLen)];
        [_recordingQueue addObject:data];
    }
}

// 用户上传的数据
- (void)setUserData:(NSDictionary *)userData
{
    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if(_delegate == nil)
            return;

        if ([userData count] == 0) {
            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_EMPTY];
            if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
                //                [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_EMPTY userInfo:nil]];
            }
            //            [userData release];
            return;
        }

        NSMutableString *result = [NSMutableString stringWithString:_appKey];
        [result appendString:@"\n"];

        for (id key in [userData allKeys]) {

            if (![key isKindOfClass:[NSNumber class]]) {
                NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_INVALID_FORMAT];
                if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
                    //                    [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_INVALID_FORMAT userInfo:nil]];
                }
                //                [userData release];
                return;
            }

            id value = [userData objectForKey:key];
            if (![value isKindOfClass:[NSArray class]]) {
                NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_INVALID_FORMAT];
                if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
                    //                    [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_INVALID_FORMAT userInfo:nil]];
                }
                return;
            }

            int type = [key intValue];
            switch (type) {

                case kUSCPersonName:
                    result = [self handleUserData:userData result:result frontLabel:@"<NAME>\n" backLabel:@"</NAME>\n" value:value];
                    break;

                case kUSCAppName:
                    result = [self handleUserData:userData result:result frontLabel:@"<APP>\n" backLabel:@"</APP>\n" value:value];
                    break;

                case kUSCSongName:
                    result = [self handleUserData:userData result:result frontLabel:@"<SONG>\n" backLabel:@"</SONG>\n" value:value];
                    break;

                case kUSCSongerName:
                    result = [self handleUserData:userData result:result frontLabel:@"<SONGER>\n" backLabel:@"</SONGER>\n" value:value];
                    break;

                case kUSCAlbumName:
                    result = [self handleUserData:userData result:result frontLabel:@"<ALBUM>\n" backLabel:@"</ALBUM>\n" value:value];
                    break;

                case kUSCCommandName:
                    result = [self handleUserData:userData result:result frontLabel:@"<COMMAND>\n" backLabel:@"</COMMAND>\n" value:value];
                    break;

                case kUSCPoiName:
                    result = [self handleUserData:userData result:result frontLabel:@"<POI>\n" backLabel:@"</POI>\n" value:value];
                    break;
            }
        }

        [self encodeUploadUserData:result];
    });
}

#pragma mark doDelegate
- (void)doEvent:(int)eventId withTime:(int)time
{
    if (!_delegate || ![_delegate respondsToSelector:@selector(onEvent:timeMs:)]) {
        return;
    }
    [_delegate onEvent:eventId timeMs:[USCSpeechTimer currentDuration]];
}

- (void)doError:(NSError *)error type:(int)type
{
    if (!_delegate || ![_delegate respondsToSelector:@selector(onError:error:)]) {
        return;
    }
    [_delegate onError:type error:error];
}

- (void)doResult:(int)type jsonString:(NSString *)jsonStr
{
    if (!_delegate || ![_delegate respondsToSelector:@selector(onResult:jsonString:)]) {
        return;
    }
    [_delegate onResult:type jsonString:jsonStr];
}

#pragma mark - All Delegate

#pragma mark  Record Delegate
//// 录音启动是否成功
- (void)onRecordingStart:(int)errorCode
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        //由于启动录音需要时间，又由于是异步启动的
        // 识别取消，不再启动识别线程
        if (isRecognizerCancelled) {
            return;
        }

        if (errorCode == RECOGNIZER_OK) {
            [beepPlayer play];
            [self doEvent:USC_ASR_EVENT_RECORDING_START withTime:100];
            // 先释放上一次分配的资源
            if (_recognition) {
                _recognition = nil;
            }
            
#ifdef DEBUG
            NSLog(@"&&&&-%@",self.recognitionParam.nluScenario);
#endif
            // 分配新的识别线程
            _recognition = [[USCRecognition alloc] init];
            _recognition.delegate = self;
            _recognition.recordingQueue = _recordingQueue;
            [_recognition setRecognitionParam:self.recognitionParam];
            [_operationQueue addOperation:_recognition];
            self.speechStatus = USC_Status_Recognizing;
        }
        else{
            // 录音失败错误代码，上传到服务器
            g_recognizer_code = errorCode;
            NSString *msg = [USCErrorCode getErrorMsg:errorCode];
            [self doError:[NSError errorWithDomain:msg code:errorCode userInfo:nil] type:100];
        }
    });
}
//
- (void)onRecordingStop:(NSMutableData *)recordingDatas
{
    NSFileManager *fileMgr = [NSFileManager defaultManager];
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
    NSString *pcmFile = [filePath stringByAppendingPathComponent:@"uscRecordFile.pcm"];
    NSString *wavFile = [filePath stringByAppendingPathComponent:@"uscRecordFile.wav"];
    [recordingDatas writeToFile:pcmFile atomically:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(onEvent:timeMs:)]) {
        pcm_to_wav([pcmFile UTF8String], [wavFile UTF8String], 16,16000,16);
        [self doEvent:USC_ASR_EVENT_RECORDING_STOP withTime:100];
        [fileMgr removeItemAtPath:wavFile error:nil];
    }

    //录音结束以后，监视器开始工作
    [recognizationWatcher start];

    //网络切换监视器开始工作
    [_netWatcher start];
}
//
////设置8k和16k采样率切换
//-(void)onSetNetType:(int)sample
//{
//    sampleRate = sample;
//}
//
//- (void)disableUnderstanderResult
//{
//    _isReturnNluResult = NO;
//}
//
- (void)setNluEnable:(id)enable
{
    if ([(NSString *)enable isEqualToString:@"true"]) {
        _isReturnNluResult = YES;
    }
    else{
        _isReturnNluResult = NO;
    }
}
//
- (void)setNluScenario:(NSString *)scenario
{
    if (scenario) {
        _nluScenario = scenario;
#ifdef DEBUG
        NSLog(@"￥￥￥￥-%@",_nluScenario);
#endif
    }
}
//
- (void)cancelRecognizationWatcher
{
    if (![recognizationWatcher isFinished])
    {
        [recognizationWatcher cancel];
    }
}

- (void)onUpdateVolume:(int)volume
{
    self.volume = volume;
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_ASR_EVENT_VOLUMECHANGE withTime:[USCSpeechTimer currentDuration]];
    });
}

- (void) onVADTimeout
{
    [USCLog log_d:@"%s",__func__];
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_ASR_EVENT_VAD_TIMEOUT withTime:[USCSpeechTimer currentDuration]];
    });
}

#pragma mark -
#pragma mark recognition Delegate

// 识别的结果全部经过这个方法返回
- (void)onRecognitionResult:(NSString *)result isLast:(BOOL)isLast
{
    ZYLog(@"全部世界结果：%@",result);
//#ifdef DEMO
//#endif
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doResult:USC_ASR_RESULT_NET jsonString:result];
    });
}

- (void)asrResult:(NSString *)result last:(BOOL)last
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doResult:USC_ASR_RESULT_NET_ONLY jsonString:result];
    });
}

- (void)onUnderstanderResult:(NSDictionary *)jsonResult
{
    [USCLog log_w:@"返回语义理解结果吧"];
    NSData *data = [NSJSONSerialization dataWithJSONObject:jsonResult options:NSJSONReadingAllowFragments error:nil];
    NSString *str = [[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doResult:USC_ASR_RESULT_NLU jsonString:str];
    });
}

- (void)onRecognitionStop:(NSError *)error
{
    self.speechStatus = USC_Status_End;

    // 通知外面接口本次识别成功结束
    dispatch_sync(dispatch_get_main_queue(), ^{

        // USC_ASR_ERROR_STOP
        [self doError:error type:100];

        // 取消识别监视器
        [self cancelRecognizationWatcher];

        // 网络监视器停止计时
        [_netWatcher stop];
        //AudioSessionSetActive(false);
    });
}

- (void)onSessionId:(NSString *)sessionId
{
    _sessionId = sessionId;
}

//识别超时
- (void)onRecognizationTimeout
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        //识别线程超时，那么取消识别线程
        [_operationQueue cancelAllOperations];
        NSString * msg = [USCErrorCode getErrorMsg:RECOGNITION_TIMEOUT];
        NSError *uscError = [NSError errorWithDomain:msg code:RECOGNITION_TIMEOUT userInfo:nil];
        [self doError:uscError type:USC_ASR_NLU_ERROR_RECOGNITION_TIMEOUT];
        [self doEvent:USC_ASR_EVENT_RECOGNITION_TIMEOUT withTime:100];
    });
}

#pragma mark -
#pragma mark vadThread delegate
//- (void)onUpdateVolume:(int)volume
//{
//      NSLog(@"%s--%@",__func__,[NSThread currentThread]);
//    self.volume = volume;
//    dispatch_sync(dispatch_get_main_queue(), ^{
//        [self doEvent:USC_ASR_EVENT_VOLUMECHANGE withTime:[USCSpeechTimer currentDuration]];
//    });
//}

- (void)onVADTimeOut
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_ASR_EVENT_VAD_TIMEOUT withTime:[USCSpeechTimer currentDuration]];
    });
}

- (void)onRecordingStart
{
    [self onRecordingStart:0];
}

// 把vad处理过的数据添加到共享数组,然后去取下一个数据
- (void)onRecordingData:(NSData *)data isValid:(BOOL)isValid
{
    @synchronized(_recordingQueue)
    {
        if (isValid)
        {
            [_recordingQueue addObject:data];
        }
    }
}

#pragma mark -
#pragma mark connection delegate
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    [receiveData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    [receiveData appendData:data];
}

- (void)connection:(NSURLConnection *)connection
  didFailWithError:(NSError *)error
{
    NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_NETWORK_ERROR];
    if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
    }
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    NSString *result = [[NSString alloc] initWithData:receiveData  encoding:NSUTF8StringEncoding];
    int code = [result intValue];

    if (code == 0) {
        if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
            //            [_delegate onUploadUserData:nil];
        }
    }else if(code == -6){
        if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_TOO_FAST];
            //            [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_TOO_FAST userInfo:nil]];
        }
    }else{
        if (_delegate && [_delegate respondsToSelector:@selector(onUploadUserData:)]) {
            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_USER_DATA_SERVER_ERROR];
            //            [_delegate onUploadUserData:[NSError errorWithDomain:msg code:UPLOAD_USER_DATA_SERVER_ERROR userInfo:nil]];
        }
    }
}

#pragma mark -
#pragma mark  property lazy inital
- (USCRecognitionParam *)recognitionParam
{
    if (!_recognitionParam) {
        _recognitionParam = [[USCRecognitionParam alloc]init];
    }

    /***********init recognitionParam ***********/
    _recognitionParam.appkey = _appKey;
    _recognitionParam.secret = _secret;
    _recognitionParam.sampleRate = sampleRate;
    _recognitionParam.engine = _asrEngine;
    _recognitionParam.engineParam = _param.engineParam;
    _recognitionParam.scene = [_param getStartScene];
    _recognitionParam.serviceAddress = address;
    _recognitionParam.port = port;
    _recognitionParam.resultFormat = _resultFormat;
    _recognitionParam.isPunctuation = _isPunctuation;
    _recognitionParam.isReturnNluResult = _isReturnNluResult;
    _recognitionParam.nluScenario = _nluScenario;
    
#ifdef DEBUG
    NSLog(@"*****nliu=%@",_recognitionParam.nluScenario);
#endif
    return _recognitionParam;
}

- (void)dealloc
{
    USCLog(@"recognizer -> dealloc");
    
    [_operationQueue cancelAllOperations];
    [_operationQueue waitUntilAllOperationsAreFinished];
    
    // 识别线程
    _recognition = nil;
    // 线程队列
    _operationQueue = nil;
    // 录音对象
    //    _recorder = nil;
    // recording queue
    _recordingQueue = nil;
    // app key
    _appKey = nil;
    _secret = nil;
    self.delegate = nil;
    beepPlayer = nil;
    recognizationWatcher = nil;
    self.delegate = nil;
    _netWatcher = nil;
    address = nil;
    port = nil;
}
@end

