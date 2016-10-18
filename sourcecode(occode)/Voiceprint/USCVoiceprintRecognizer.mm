//
//  USCRecognizer.m
//  usc
//
//  Created by hejinlai on 12-11-3.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "USCVoiceprintRecognizer.h"
#import "USCUtil.h"
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
#import "USCConstant.h"
#import "USCSpeechConstant.h"
#import "USCSpeechTimer.h"
#import "USCVadThread.h"
#import "USCSpeechConstant.h"
#import "USCRecognition.h"

#define kBgQueue dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0)
#define kMainQueue dispatch_get_main_queue()

// 请求vpr音频数据地址
static  const NSString *kUSCRequestAudioAddres  = @"117.121.49.3:10000";

@interface USCVoiceprintRecognizer() <USCRecognitionDelegate,USCVadThreadDelegate, RecognizationWatcherDelegate, BeepPlayerDelegate, NetWatcherDelegate>
{
    // record data array
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
    NSString *_engine;
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

    // recognition parameter
    USCRecoginizerParam *_param;
    // vad parameter
    USCVADParam *_vadParam;

    // 当前所在的城市
    NSString *currentCity;

    // 当前的坐标
    CLLocationCoordinate2D currentCoordinate;

    // 获取地理位置
    USCLocationTool *locationTool;

    // return nlu result with asr result
    BOOL _isReturnNluResult;

    // nlu scenario
    NSString *_nluScenario;

    // 设置
    BOOL _farField;

    BOOL _isVPREnable;

}

@property (nonatomic,strong) USCVoiceprintParams *vprParams;
@property (nonatomic,strong) USCRecognitionParam  *recognitionParam;
@property (nonatomic,strong) NSURLConnection *conn;
@property (nonatomic,strong) NSOperationQueue *queue;
@property (nonatomic,strong) NSFileManager *mgr;
@property (nonatomic,strong) NSFileHandle *fileHandle;
@property (nonatomic,strong) NSString *audioFilePath;
@property (nonatomic,strong) NSMutableData *allData;
@property (nonatomic,strong) USCIAudioSource *recordAudioSource;
@property (nonatomic,strong) USCVadThread *vadThread;
@end

@implementation USCVoiceprintRecognizer

@synthesize delegate = _delegate;
- (NSMutableData *)allData
{
    if (!_allData) {
        _allData = [[NSMutableData alloc]init];
    }
    return _allData;
}

- (id)initWithAppKey:(NSString *)appkey secret:(NSString *)secret
{
    if (self = [super init]) {
        _appKey = [appkey copy];
        _secret = [secret copy];

        _recordingQueue = [[NSMutableArray alloc] init];
        _operationQueue = [[NSOperationQueue alloc] init];

        _netWatcher = [[NetWatcher alloc] init];
        _netWatcher.delegate = self;

//        _recorder = [[USCRecorder alloc] init];
//        _recorder.delegate = self;
//        _recorder.recordingQueue = _recordingQueue;

        isRecognizerCancelled = NO;
        sampleRate = SAMPLE_RATE_AUTO;
        _netWatcher.isAutoSwitch = YES;
        _engine = USC_ENGINE_GENERAL;

        beepPlayer = [[BeepPlayer alloc] init];
        beepPlayer.delegate = self;
        locationTool = [[USCLocationTool alloc]init];
        recognizationWatcher = [[RecognizationWatcher alloc] init];
        recognizationWatcher.delegate = self;
        _isAllowedPlayBeep = YES;

        serviceAddressAndPort = @"vpr.hivoice.cn:80";
        address = @"vpr.hivoice.cn";
        port = @"80";
        _isPunctuation = YES;
        _addressGetter = [[USCAddressGetter alloc] init];
        _language = USC_LANGUAGE_MANDARIN;           //识别语言默认是中文
        _resultFormat = [[USCAsrResultFormatChinese alloc] init];    //初始化_resultFormat，以中文初始化
        _resultFormat.isPunctuation = _isPunctuation;                //中文初始化，对应的标点符号处理

         /***********识别参数***********/
        _param = [[USCRecoginizerParam alloc]init];
        _vadParam = [[USCVADParam alloc] init];
        _isReturnNluResult = NO;
        _nluScenario = @"";
        _farField = NO;
        _isVPREnable = NO;

        _vadThread = [[USCVadThread alloc]init];
        _vadThread.delegate = self;
        _vadThread.recordingQueue = _recordingQueue;
    }
    return self;
}

#pragma mark -
#pragma mark  puclic method

- (void)setOption:(int)key value:(id)value
{
    if (!value) {
        return;
    }

//    NSLog(@"key = %d",key);
    switch (key) {
        case USC_VPR_PARAMETER:
            [self setVocieprintParam:(USCVoiceprintParams *)value];
            break;
        default:
            break;
    }
}

// set recognitionParam base voiceprintparam
- (void)setVocieprintParam:(USCVoiceprintParams *)voiceprintParam
{
    if (voiceprintParam) {

        self.vprParams = voiceprintParam;

        self.recognitionParam.recognitionType = 3;
        self.recognitionParam.isReturnNluResult = NO;
        self.recognitionParam.userName = [voiceprintParam getUserName];
        self.recognitionParam.vprType = [voiceprintParam getVPRType];
        self.recognitionParam.vprTypeStr = [voiceprintParam  vprTypeString];
    }
}

- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime
{
    int frontFrame = frontTime / 10;
    int backFrame = backTime / 10;

//    [_recorder setVadFrontTimeout:frontFrame BackTimeout:backFrame];
}

- (void)setProperty:(NSString *)property forKey:(int)key
{
    switch (key)
    {
        case USC_SERVICE_ADDRESS_PORT:
        {
            serviceAddressAndPort = property;
            if (arrayAddressAndPort)
            {
                arrayAddressAndPort = nil;
            }

            arrayAddressAndPort = [serviceAddressAndPort componentsSeparatedByString:@":"];
            if ([arrayAddressAndPort count] == 2)
            {
                if (address){
                    address = nil;
                }
                address = [[arrayAddressAndPort objectAtIndex:0] copy];
                if ([address isEqual:@""] || !address)
                    address = @"vpr.hivoice.cn";
                if (port)
                    port = nil;

                port = [[arrayAddressAndPort objectAtIndex:1] copy];
                if ([port intValue] == 0)
                    port = @"80";
            }
        }
        break;
        default:
            break;
    }
    [USCLog log_v:@"%s---%@-- %@",__func__,address,port];
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


- (void) setResultFormat:(NSString *)language
{
    if (_resultFormat)
    {
        _resultFormat = nil;
    }

    if ([language isEqualToString:@"english"])
    {
        _resultFormat = [[USCAsrResultFormatEnglish alloc] init];
    }
    else if ([language isEqualToString:@"cantonese"])
    {
        _resultFormat = [[USCAsrResultFormatCantonese alloc] init];
    }
    else
    {
        _resultFormat = [[USCAsrResultFormatChinese alloc] init];
    }

    // 当切换语言的时候，保持之前的标点符号法则不变
    _resultFormat.isPunctuation = _isPunctuation;
}

- (NSString *)getSessionId
{
    return _sessionId;
}

- (void)fetchRecordFileWithRequestId:(NSString *)requestId filePath:(NSString *)filePath
{
    [self valueForKey:USCEventDownloadAudioFile withParam:requestId,filePath];
}

- (id)valueForKey:(int)key withParam:(NSString *)firstParam, ...
{
    NSMutableArray *paramArray = [NSMutableArray array];
    NSString *arg;

    if (firstParam) {
    va_list arg_list;
    va_start(arg_list,firstParam);
    [paramArray addObject:firstParam];
    arg = va_arg(arg_list, NSString *);
    while ([arg isKindOfClass:[NSString class]]) {
        [paramArray addObject:arg];
        arg = va_arg(arg_list, NSString *);
    }
     va_end(arg_list);
  }

    int paramCount = paramArray.count;
    id value = nil;
    switch (key) {
            case 100:
            if (paramCount == 2) {
                [self downloadVPRAudioWithRequestId:[paramArray objectAtIndex:0] filePath:[paramArray objectAtIndex:1]];
            }
            break;
            case 101:
            if (paramCount == 3) {
            NSString *page = [paramArray objectAtIndex:1];
            NSString *count = [paramArray objectAtIndex:2];
            value = [self fetchRequestIdWithPage:page perCount:count];
            }
            break;
        default:
            break;
    }
    return value;
}

- (void)clear
{
    // cancel record
//    [_recorder cancelRecording];

    // cancel recognition and don't receive recognition result
    _recognition.delegate = nil;
    [_operationQueue cancelAllOperations];

    // clear record data
    @synchronized(_recordingQueue){
        [_recordingQueue removeAllObjects];
    }
}

- (void)start:(NSString *)userName type:(NSString *)type
{
    if (![self.vprParams setVPRName:userName]) {
//        NSLog(@"self.vprpara =%@",self.vprParams);
        return;
    }

    
    if ([type isEqualToString:@"register"]) {
        [self.vprParams setVPRType:USCVPRTypeRegister];
    }

    if ([type isEqualToString:@"login"]) {
        [self.vprParams setVPRType:USCVPRTypeLogin];
    }

    [self start];
}

- (void)start
{
    [recognizationWatcher reset];

    // 开始计时,以便计算各个事件完成的时间
    [USCSpeechTimer begine];

    dispatch_async(kBgQueue, ^{
        isRecognizerCancelled = NO;
        //frequent start will error
        NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
        double diff = startTime - cancelTime;
        if (diff <= 0.3) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                NSString *msg = [USCErrorCode getErrorMsg:OPERATION_TOO_FAST];
                [self doError:[NSError errorWithDomain:msg code:OPERATION_TOO_FAST userInfo:nil] type:USC_VPR_ERROR_OPERATION_TOO_FAST];
            });
            return;
        }

        if (_vadThread) {
            _vadThread.vadParam = _param.vadParam;
        }
//        [_vadThread start:YES];
        [_vadThread start];
    });
}

- (void)stop
{
    dispatch_async(kBgQueue, ^{
//        if (_recorder) {
//            [_recorder stopRecording];
//        }
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

+ (NSString *)getVersion
{
    return sdk_version;
}
#pragma mark -
//#pragma mark DoDelegate
//
//- (void)doError:(NSError *)error type:(int)type
//{
//    if (self.delegate && [self.delegate respondsToSelector:@selector(onError:error:)]) {
//        [self.delegate onError:type error:error];
//    }
//}
//
//- (void)doEvent:(int)eventId times:(int)time
//{
//    if (self.delegate && [self.delegate respondsToSelector:@selector(onEvent:times:)]) {
//        [self.delegate onEvent:eventId times:time];
//    }
//}
//
//- (void)doResult:(int)type result:(NSString *)json
//{
//    if (self.delegate && [self.delegate respondsToSelector:@selector(onResult:result:)]) {
//        [self.delegate onResult:type result:json];
//    }
//}

#pragma mark -
#pragma mark private method

/*!  //FIXME: this method donot have public interface to users,and usless temporarily
 *
 *  @brief  get requestId list
 *
 *  @param page  页数
 *  @param count 每页个数
 *
 *  @return
 */
- (id)fetchRequestIdWithPage:(NSString *)page perCount:(NSString *)count
{
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@/s/list.do?appKey=%@&&n=%@&&per=%@",kUSCRequestAudioAddres,_appKey,page,count]];
    [NSURLConnection sendAsynchronousRequest:[NSURLRequest requestWithURL:url] queue:[[NSOperationQueue alloc]init] completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
        id result = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:nil];
    }];
    return nil;
}

/*!
 *
 *  @brief  download vpr record audio from server with requestId（sessionId）
 *
 *  @param requestId requestId对应的是sessionId
 *  @param filePath  下载完成后的录音文件保存的路径
 *
 *  @return
 */
- (id)downloadVPRAudioWithRequestId:(NSString *)requestId filePath:(NSString *)filePath
{
    if (!requestId || !filePath) {
        return [NSError errorWithDomain:@"url or filePath error" code:-1 userInfo:nil];
    }

    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@/s/get.do?appKey=%@&&rid=%@",kUSCRequestAudioAddres,_appKey,requestId]];
    self.audioFilePath = filePath;

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc]initWithURL:url];
    request.HTTPMethod = @"GET";
    [NSURLConnection  sendAsynchronousRequest:request queue:[[NSOperationQueue alloc]init] completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
        NSHTTPURLResponse *httpRespos = (NSHTTPURLResponse *)response;
        if (data) {
            [USCLog log_e:@"audiofilePath:%@",filePath];
            [data writeToFile:filePath atomically:YES];
        }

        if (connectionError) {
            [self doError:connectionError type:USC_VPR_ERROR_DOWNLOADAUDIO];
        }
        else{
            [self doEvent:USC_VPR_EVENT_DOWNLOADAUDIO_END withTime:100];
        }
    }];
    return nil;
}

- (void)createRecognition
{
    if (_recognition) {
        _recognition = nil;
    }

    // create new recognition thread everytime
    _recognition = [[USCRecognition alloc] init];
    _recognition.appKey = _appKey;
    _recognition.delegate = self;
    _recognition.sampleRate = sampleRate;

    // set engine
    _recognition.engine = _engine;

    [_recognition setParam:_param];

    _recognition.recordingQueue = _recordingQueue;

    _recognition.serviceAddress = address;
    _recognition.port = port;
    _recognition.resultFormat = _resultFormat;
    _recognition.isPunctuation = _isPunctuation;
    _recognition.isReturnNluResult = _isReturnNluResult;
    _recognition.nluScenario = _nluScenario;

    [_operationQueue addOperation:_recognition];
}

- (void)setRecognizationTimeout:(float)recognizationTime
{
    if (recognizationTime < 5.0f || recognizationTime > 60.0f) {
        return;
    }
    recognizationWatcher.timeOutLimit = recognizationTime;
}

#pragma mark doDelegate
- (void)doEvent:(int)eventId withTime:(int)time
{
    if (!_delegate || ![_delegate respondsToSelector:@selector(onEvent:times:)]) {
        return;
    }
    [_delegate onEvent:eventId times:[USCSpeechTimer currentDuration]];
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
    [_delegate onResult:type result:jsonStr];
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
    [USCLog log_d:@"%s",__func__];
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_VPR_EVENT_VADTIMEOUT withTime:[USCSpeechTimer currentDuration]];
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

- (void) onRecordingStart:(int)errorCode
{
//    NSLog(@"开启识别线程");
    dispatch_sync(dispatch_get_main_queue(), ^{
        [self doEvent:USC_VPR_EVENT_RECORDSTART withTime:[USCSpeechTimer currentDuration]];
//        [self doEvent:USC_VPR_EVENT_RECORDSTART times:[USCSpeechTimer currentDuration]];
    });

    dispatch_sync(dispatch_get_main_queue(), ^{

        //由于启动录音需要时间，又由于是异步启动的
        // 识别取消，不再启动识别线程
        if (isRecognizerCancelled) {
            return;
        }
        if (errorCode == RECOGNIZER_OK) {
            [beepPlayer play];

             [self doEvent:USC_VPR_EVENT_RECOGNITION_RECORD_START withTime:[USCSpeechTimer currentDuration]];
//            [self doEvent:USC_VPR_EVENT_RECOGNITION_RECORD_START times:[USCSpeechTimer currentDuration]];
            // release recognition
            if (_recognition) {
                _recognition = nil;
            }

            // create new recognition thread
            _recognition = [[USCRecognition alloc] init];
            _recognition.delegate = self;
            _recognition.recordingQueue = _recordingQueue;

            [self setVocieprintParam:self.vprParams];

            [_recognition setRecognitionParam:self.recognitionParam];
            [USCLog log_w:@"vpr description:%@",self.vprParams.description];
            [_operationQueue addOperation:_recognition];
        }
        else
        {//TODO:
            NSString *msg = [USCErrorCode getErrorMsg:errorCode];
            [self doError:[NSError errorWithDomain:msg code:errorCode userInfo:nil] type:USC_VPR_ERROR_RECORD_ERROR];
            //            if (_delegate && [_delegate respondsToSelector:@selector(onEnd:)]) {
            //                g_recognizer_code = errorCode;
            ////                    [_delegate onEnd:[NSError errorWithDomain:msg code:errorCode userInfo:nil]];
            //            }
        }
    });
}

#pragma mark -
#pragma mark  recognition delegate

- (void)onRecognitionResult:(NSString *)result isLast:(BOOL)isLast
{
    if (_delegate && [_delegate respondsToSelector:@selector(onResult:result:)]) {
        [_delegate onResult:[self.vprParams getVPRType] result:result];
    }
}

- (void)onVPRResult:(NSDictionary *)dict
{
    if (dict) {
        int statusCode = [[dict objectForKey:@"status"] intValue];
        if (statusCode != 200) {
            if (![dict objectForKey:@"errorMsg"]) {
                [dict setValue:@"" forKey:@"errorMsg"];
            }

            NSError *error = [NSError errorWithDomain:[dict objectForKey:@"errorMsg"] code:statusCode userInfo:nil];

            dispatch_sync(dispatch_get_main_queue(), ^{
                [self doError:error type:USC_VPR_ERROR_RESULT];
            });
        }
        else
        {
            USCVoiceprintResult *vprResult = [[USCVoiceprintResult alloc]init];
            vprResult.userName = [dict objectForKey:@"username"];
            vprResult.statusCode = [[dict objectForKey:@"status"] intValue];
            vprResult.score = [[dict objectForKey:@"score"] intValue];

            NSMutableDictionary *mDict = [NSMutableDictionary dictionary];
            [mDict setValue:vprResult.userName forKey:@"username"];
            [mDict setValue:[NSString stringWithFormat:@"%d",vprResult.statusCode] forKey:@"status"];
            [mDict setValue:[NSString stringWithFormat:@"%f",vprResult.score] forKey:@"score"];

            NSError *err;
            NSData *data = [NSJSONSerialization dataWithJSONObject:mDict options:0 error:&err];
            if (err) {
                [self doError:err type:USC_VPR_ERROR_RESULT_JSON];
            }

            NSString * vprJson = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];

            dispatch_sync(dispatch_get_main_queue(), ^{
                [self doResult:USC_VPR_RESULT jsonString:vprJson];
         });
        }
    }
}

// recognition processed have finished ，not mean recognition thread stop
- (void)onRecognitionStop:(NSError *)error
{
    dispatch_sync(dispatch_get_main_queue(), ^{

        if (error) {
            [self doError:error type:USC_VPR_ERROR_RECOGNITION];
        }
        else{
             [self doEvent:USC_VPR_EVENT_RECOGNITION_END withTime:[USCSpeechTimer currentDuration]];
        }

        [self cancelRecognizationWatcher];
        [_netWatcher stop];
    });
}

- (void)onSessionId:(NSString *)sessionId
{
    _sessionId = sessionId;
}

- (void)cancelRecognizationWatcher
{
    if (![recognizationWatcher isFinished])
    {
        [recognizationWatcher cancel];
    }
}

#pragma mark -
#pragma mark watcher beepplayer delegate
//识别超时
- (void)onRecognizationTimeout
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        // 识别线程超时，那么取消识别线程
        [_operationQueue cancelAllOperations];
        NSString * msg = [USCErrorCode getErrorMsg:RECOGNITION_TIMEOUT];
        NSError *uscError = [NSError errorWithDomain:msg code:RECOGNITION_TIMEOUT userInfo:nil];
        [self doError:uscError type:USC_ASR_NLU_ERROR_RECOGNITION_TIMEOUT];
//        [self doEvent:USC_ASR_EVENT_RECOGNITION_TIMEOUT withTime:100];
    });
}

//设置8k和16k采样率切换
-(void)onSetNetType:(int)sample
{
    sampleRate = sample;
}

-(BOOL)isAllowedPlayBeep
{
    return _isAllowedPlayBeep;
}

#pragma mark lazy
- (NSOperationQueue *)queue
{
    if (!_queue) {
        _queue = [[NSOperationQueue alloc]init];
    }
    return _queue;
}

#pragma mark gettter 
- (NSFileHandle *)fileHandle
{
    if (!_fileHandle) {
        _fileHandle = [[NSFileHandle alloc]init];
    }
    return _fileHandle;
}
- (NSFileManager *)mgr
{
    if (!_mgr) {
        _mgr = [NSFileManager defaultManager];
    }
    return _mgr;
}

- (USCRecognitionParam *)recognitionParam
{
    if (!_recognitionParam) {
        _recognitionParam = [[USCRecognitionParam alloc]init];
    }

    /***********init recognitionParam ***********/
    _recognitionParam.userName = [self.vprParams getUserName];
    _recognitionParam.vprTypeStr = [self.vprParams vprTypeString];

    _recognitionParam.recognitionType = 3;

    _recognitionParam.appkey = _appKey;
    _recognitionParam.secret = _secret;
    _recognitionParam.sampleRate = sampleRate;
    _recognitionParam.engine = _engine;
    _recognitionParam.engineParam = _param.engineParam;
    _recognitionParam.scene = [_param getStartScene];
    _recognitionParam.serviceAddress = address;
    _recognitionParam.port = port;
    _recognitionParam.resultFormat = _resultFormat;
    _recognitionParam.isPunctuation = _isPunctuation;
    _recognitionParam.isReturnNluResult = _isReturnNluResult;
    _recognitionParam.nluScenario = _nluScenario;

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
    self.delegate = nil;
    beepPlayer = nil;
    recognizationWatcher = nil;
    self.delegate = nil;
    _netWatcher = nil;
    address = nil;
    port = nil;
}
@end
