//
//  USCSpeechUnderstander.m
//  nlu&asr
//
//  Created by yunzhisheng on 14-12-1.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCSpeechUnderstander.h"
#import "USCRecognizer.h"
#import "USCVersion.h"
#import "USCNLUParser.h"
#import "USCLog.h"
#import "USCSpeechResult.h"
#import "USCSceneManage.h"
#import "USCRecoginizerParam.h"
#import "USCNLUParam.h"
#import "USCScene.h"
#import "USCSpeechConstant.h"

 /***********param***********/
/*
 engine
 language
*/
@interface USCSpeechUnderstander()<USCNLUParserDelegate,USCRecognizerDelegate>
{
    // 识别对象
    USCRecognizer *_recognizer;
    // 解析对象
    USCNLUParser *_nluParser;
    // 语义理解返回的对象
    USCNLUParsedResult *_nluParsedResult;
    // 场景
    NSString *scenerio;
    // 要理解的文字
    NSString *requestText;
    // 场景管理对象
    USCSceneManage *_sceneMgr;
    // 识别参数对象
    USCRecoginizerParam *_param;
    // 语义参数
    USCNLUParam *_nluParam;
    BOOL _isSetEngine;
    // 每次请求是否返回语义理解结果
    BOOL _isReturnNluResult;
    // 是否远讲
    BOOL _farField;

     /***********newinterface***********/
    int _frontTime,_backTime;
}
@property (nonatomic,strong) NSMutableString *asrResultString;

@property (nonatomic,strong) USCIAudioSource *recordAudioSource;

@end

@implementation USCSpeechUnderstander

#pragma mark - public method
- (id)initWithContext:(NSString *)context appKey:(NSString *)appKey secret:(NSString *)secret
{
    self = [super init];
    if (self) {
        _sceneMgr = [[USCSceneManage alloc]init];

        _param = [[USCRecoginizerParam alloc]init];
        _param.appkey = appKey;

        _nluParam = [[USCNLUParam alloc]init];

        [_sceneMgr setParams:_param];

        _recognizer = [[USCRecognizer alloc]initWithAppKey:appKey];
        _recognizer.delegate = self;

        /***********给识别对象设置识别参数 15-1-13 ***********/
//        [_recognizer setParam:_param];
        [_recognizer setOption:USC_ASR_RECOGNIZER_PARAM value:_param];
        
//        [_recognizer start];   //初始化的时候不在开启引擎

        _nluParser = [[USCNLUParser alloc]initWithAppkey:appKey secret:secret];
        _nluParser.delegate = self;
        [_nluParser setNLUParam:_nluParam];

        _isSetEngine = NO;
        _isReturnNluResult = YES;
        _farField = NO;

        /***********newinterface***********/
        _frontTime = 3000;
        _backTime = 1000;
    }
    return self;
}

- (void)start
{
    self.asrResultString = [[NSMutableString alloc]initWithString:@""];
    if (!_recognizer) {
        return;
    }

    // 判断在开始识别前是否已经设置engine
    if (!_isSetEngine) {
//        [_recognizer setEngine:@""];
        [_recognizer setOption:USC_ASR_DOMAIN value:@""];
    }
    [_recognizer start];
}

- (void)stop
{
    if (_recognizer) {
        [_recognizer stop];
    }
}

- (void)cancel
{
    if (_recognizer)
    {
        [_recognizer cancel];
    }

    if (_nluParser) {
        [_nluParser cancel];
    }
}

- (void)setOption:(int)key value:(id)value
{
    NSLog(@"setOption:%d value:%@",key,value);
    if (_recognizer) {
        [_recognizer setOption:key value:value];
    }
}

- (id)getOption:(int)key
{
    if (_recognizer) {
        return  [_recognizer getOption:key];
    }
    return nil;
}

//???:
- (int)getStatus
{
    if (_recognizer) {
        return [_recognizer getStatus];
    }
    return -1;
}

//???:
- (int)init:(NSString *)jsonString
{
    if (_recognizer) {
     return [_recognizer init:jsonString];
    }
    return -1;
}

//???:
- (int)postInfo:(NSString *)jsonString
{
    return  0;
}

- (void)uploadUserData:(NSDictionary *)userData
{
    if (_recognizer) {
        [_recognizer uploadUserData:userData];
    }
}

- (int)setAudioSource:(USCIAudioSource *)audioSource
{
    if (_recognizer) {
        return [_recognizer setAudioSource:audioSource];
    }
    return -1;
}

#pragma mark -
#pragma mark Private Method
- (NSDictionary *)dictWithJsonString:(NSString *)jsonStr
{
    NSError *error;
    NSData *data = [jsonStr dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:&error];
    if (error) {
        return nil;
    }
    return dict;
}

#pragma mark -
#pragma mark lazy load
- (NSMutableString *)asrResultString
{
    if (!_asrResultString) {
        _asrResultString = [[NSMutableString alloc]init];
    }
    return _asrResultString;
}

#pragma mark -
#pragma mark  Other Method
- (void)recognizeAudioFile:(NSString *)audioFilePath
{

    if (_recognizer) {
        [_recognizer recognizeAudioFile:audioFilePath];
    }
}


//
//- (void)setPlayingBeep:(BOOL)isAllowed
//{
//    if (_recognizer) {
//        [_recognizer setPlayingBeep:isAllowed];
//    }
//}
//
//- (void)disableUnderstanderResult
//{
//    _isReturnNluResult = NO;
//    if (_recognizer) {
//        [_recognizer disableUnderstanderResult];
//    }
//}
//
//+ (void)showLog:(BOOL)show
//{
//
//}
//
//- (void)textUnderstander:(NSString *)text
//{
//    if (_nluParser) {
//        [_nluParser cancel];
//        [_nluParser parse:text];
//    }
//}
//
//- (USCSceneManage *)getSceneManger
//{
//    return _sceneMgr;
//}
//
//- (void)start:(USCScene *)scene
//{
//    if (!_recognizer) {
//        return;
//    }
//
//    if(scene)
//    {
//        [_param setStartScene:scene];
//        [_recognizer setParam:_param];
//    }
//    [self start];
//}
//
//
//+ (NSString *)getVersion
//{
//    return sdk_version;
//}
//
//- (void)setProperty:(NSString *)property forKey:(int)key
//{
//    switch (key) {
//        case USC_SERVICE_ADDRESS_PORT:
//            [_recognizer setProperty:property forKey:USC_SERVICE_ADDRESS_PORT];
//            break;
//        case USC_NLU_SERVICE_URL:
//            [_nluParser setURL:property];
//        default:
//            break;
//    }
//}


#pragma mark -  All Delegate
#pragma mark ASR Delegate

- (void)onEvent:(int)type timeMs:(int)timeMs
{
    [USCLog log_usc:@"%s",__func__];
    if (_delegate && [_delegate respondsToSelector:@selector(onEvent:timeMs:)]) {
        if (type == USC_ASR_EVENT_VAD_TIMEOUT) {
            [USCLog log_usc:@"%s-%s",__FILE__,__FUNCTION__];
            [_recognizer stop];
        }
        [_delegate onEvent:type timeMs:timeMs];
    }
}

- (void)onError:(int)type error:(NSError *)error
{
    if (_delegate && [_delegate respondsToSelector:@selector(onError:error:)]) {
        [_delegate onError:type error:error];
    }
}

- (void)onResult:(int)type jsonString:(NSString *)jsonString
{
    if (_delegate && [_delegate respondsToSelector:@selector(onResult:jsonString:)]) {
        [_delegate onResult:type jsonString:jsonString];
    }
}

-(NSString *)addActionPxb {
    return @"are u ok";
}


#pragma  mark - Nlu Delegate
- (void)onNLUEnd:(NSError *)error
{
    [self onError:USC_ASR_NLU_EVENT_END error:error];
}

- (void)onReceivedResult:(USCNLUParsedResult *)result
{
    USCSpeechResult *speechResult = [[USCSpeechResult alloc]init];
    speechResult.requestText = self.asrResultString;
    speechResult.responseText = result.text;
    speechResult.stringResult = result.jsonString;
    speechResult.scenario = scenerio;

    [self onResult:USC_ASR_RESULT_NLU jsonString:[NSString stringWithFormat:@""]];
}

#pragma makr hidePublic Method
- (NSURL *)getHttpUrl
{
    return [_nluParser getHttpUrl];
}

- (void)dealloc
{
    _recognizer = nil;
    _nluParser = nil;
}
@end
