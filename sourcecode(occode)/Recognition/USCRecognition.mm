//
//  USCRecognition.m
//  usc
//
//  Created by hejinlai on 12-11-9.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "set_option_key.h"
#import "USCUtil.h"
#import "USCRecognition.h"
#import "USCMarcos.h"
#import "USCErrorCode.h"
#import <arpa/inet.h>
#import <netdb.h>
#import "USCVersion.h"
#import "USCRecoginizerParam.h"
#import "log.h"
#import "USCEngineParams.h"
#import "USCScene.h"
#import "USCCommonTool.h"
#import "USCLog.h"
#import "USCRecognitionParam.h"

#define ASRKEY @"asr_recongize"

const  char *ASR_OPT_AUDIO_ENCODE_OUPS_16K = "opus";
const  char *ASR_OPT_AUDIO_ENCODE_OUPS_8K = "opus-nb";
//const  char *ASR_OPT_RSP_ENTITY = "req_audio_url";
const  char *ASR_USRDATA_FLAG_OPEN = "open";
const  char *ASR_USRDATA_FLAG_CLOSE = "close";

@interface USCRecognition()
{
    BOOL isPartailFirst;
    BOOL hasPartail;
    NSString *_serviceAddress;
    NSString *_port;
    USCUtil *_resultFormat;
    USCRecoginizerParam *_param;
    USCScene *_scene;
    ASRClientWrapper _asrClient;
    USCEngineParams *engineParams;
    id _recognitionParam;
}
@end

@implementation USCRecognition
- (id)init
{
    if (self = [super init]) {
        _setToStopped = NO;
        _engine = @"general";     //识别引擎默认是general
        _sampleRate = 16000;      //采样率默认是16000
        USCLog(@"recognition -> init");
        self.port = @"80";
        self.isPunctuation = YES;
        self.isReturnNluResult = YES;
        self.nluScenario = @"";
        self.isVPR = NO;
    }
    return self;
}

#pragma mark -
#pragma mark public method
- (void)setParam:(USCRecoginizerParam *)param
{
    if (!param) {
        return;
    }
    _param = param;
    /***********获取engineParam***********/
    engineParams = [_param getEngineParams];

    _scene = [param getStartScene];
    if (!_scene){
    }
}

#pragma mark -
#pragma mark private method
/**
 *
 *  更新场景对象
 *
 *  @return 暂时不适用
 */
- (void)updateAsrScene:(ASRClientWrapper )asrclient scene:(USCScene *)scene
{
    if (scene != nil  && [scene isEnabled]) {
        int res = asrclient.setValueInt(ASR_SCENE_CONTEXTID, [scene getSceneID]);
    }
}

- (void)doRecognitionResult:(NSString *)result isLast:(BOOL)isLast
{

//    if (self.recognitionParam.recognitionType == 3) {
//        if (_delegate && [_delegate respondsToSelector:@selector()]) {
//            <#statements#>
//        }
//    }
//    NSLog(@"返回识别结果：%@---%d",result,self.recognitionParam.recognitionType);
    if (_delegate && [_delegate respondsToSelector:@selector(onRecognitionResult:isLast:)] && ![self isCancelled])
    {
        [_delegate onRecognitionResult:result isLast:isLast];
    }
}

- (void)setEngine:(NSString *)engine
{
    _engine = engine;
}
- (void)doMaxSpeechTimeout
{
    if (_delegate && [_delegate respondsToSelector:@selector(onMaxSpeechTimeout)] && ![self isCancelled]) {
        [_delegate onMaxSpeechTimeout];
    }
}

- (void)doSessionId:(const char *)sessionId
{
    if (_delegate && [_delegate respondsToSelector:@selector(onSessionId:)])
    {
        [_delegate onSessionId:[NSString stringWithUTF8String:sessionId]];
    }
}

- (void)doRecognitionStop:(NSError *)error
{
    if (_delegate && [_delegate respondsToSelector:@selector(onRecognitionStop:)] && ![self isCancelled]) {
        [_delegate onRecognitionStop:error];
    }
       return;
}

#pragma mark set recognition parameter
// set asr parameter
- (void)setASRParam
{
    _asrClient.setValueInt(205, 1);// set md5
    _asrClient.setValueString(204,[self.secret UTF8String]);

    // 2.engineParams,设置识别参数
    if([engineParams isEnabled])
        _asrClient.setValueString(OPT_ENGINE_PARAMETER, [[self.recognitionParam.engineParam toString] UTF8String]);

    _asrClient.setValueInt(ASR_PCM_COMPRESS_ID, 8);
    _asrClient.setValueInt(ASR_WAITING_RESULT_TIMEOUT_ID, 20);
    _asrClient.setValueString(ASR_RESULT_FORMAT_ID, "json");

    // 5.设置场景
    if ([_scene isEnabled])
        _asrClient.setValueInt(ASR_SCENE_CONTEXTID, [_scene getSceneID]);

    // 6.设置引擎
    if([engineParams isEnabled])
        _asrClient.setValueString(ASR_MODLE_TYPE, [_engine UTF8String]);

    // mac addr
    NSString *mac_addr = [USCUtil getLocalMacAddress];

    if (self.isPunctuation)
        _asrClient.setValueString(ASR_PUNCTUATION_ENABLE, "true");
    else
        _asrClient.setValueString(ASR_PUNCTUATION_ENABLE, "false");

    const char * imei = NULL;
    if (mac_addr)
        imei = [mac_addr UTF8String];
    else
        imei = "unknow iOS device";

    // mac address -> imei
    _asrClient.setValueString(OPT_IMEI_ID, imei);

    // mac address -> user id
    _asrClient.setValueString(OPT_USER_ID, imei);

    // collected info
    _asrClient.setValueString(OPT_COLLECTED_INFO, [[self getCollectedInfo] UTF8String]);
    g_recognizer_code = g_conn_code = g_os_errorno = 0;
}

// set nlue parameter
- (void)setNLUParam
{
    // get city
    NSString *str = [NSString stringWithFormat:@"filterName=%@;returnType=%@;city=%@;gps=%@;udid=%@;history=%@;scenario=%@;screen=%@;dpi=%@;time=%@;",@"search",@"json",[[NSUserDefaults standardUserDefaults] objectForKey:@"currentcity"],[[NSUserDefaults standardUserDefaults] objectForKey:@"currentgps"],[[NSUserDefaults standardUserDefaults] objectForKey:@"udid"],@"",self.recognitionParam.nluScenario,@"",@"",[USCCommonTool getTime]];
    _asrClient.setValueString(USC_NLU_PARAMETER, [str UTF8String]);
//    [USCLog log_d:@"%s nluparam=%@",__func__,str];
}

// set vpr parameter
- (void)setVPRParam
{
    _asrClient.setValueInt(1015, 8);
    _asrClient.setValueInt(205, 1);// set md5
    _asrClient.setValueString(204,[self.secret UTF8String]);

    NSString *type = @"";
    NSString *username = [self.recognitionParam userName];

    switch ([self.recognitionParam vprType]) {
        case USCVPRTypeLogin:
            type = @"matchSingle";
            break;
        case USCVPRTypeRegister:
            type = @"register";
            break;
        default:
            break;
    }
    
    NSString *paraStr = [NSString stringWithFormat:@"filterName=vpr;userName=%@;type=%@;returnType=json;",self.recognitionParam.userName,type];
//    NSLog(@"vpr 参数：%@",paraStr);
    // 设置声纹参数
    _asrClient.setValueString(201, [paraStr UTF8String]);
    [USCLog log_d:@"%s-- vprParameter:%@",__func__,paraStr];
}

// 设置通用的参数appkey
- (void)setCommonParam
{
    _asrClient.setValueString(OPT_APP_KEY, [self.recognitionParam.appkey UTF8String]);
//    [USCLog log_w:@"appkey:%@",self.recognitionParam.appkey];
}

- (void)setServiceAddress
{
    self.serviceAddress = self.recognitionParam.serviceAddress;
    self.port = self.recognitionParam.port;
}

- (void)main
{
    @autoreleasepool {
        // notificate recognizer that recognition start
        if (self.delegate && [self.delegate respondsToSelector:@selector(onRecognitionStart)]) {
            [self.delegate onRecognitionStart];
        }
        
        // 1.set service address
        [self setServiceAddress];

        // 1.init with server address and port
        int initCode =  _asrClient.init([[self getDomain:self.serviceAddress] UTF8String], [self.port intValue]);

        // 2.set param
        [self setCommonParam];

        if (self.recognitionParam.recognitionType == 3) {
            [self setVPRParam];
        }
        else{
            [self setASRParam];
            if (self.recognitionParam.isReturnNluResult) {
                [self setNLUParam];
            }
        }

        // 2.start ASRClient开始识别
        int startCode = _asrClient.start();
        [USCLog log_w:@"startCode=%d",startCode];

        if (startCode != 0)
            [self doRecognitionStop:[NSError errorWithDomain:@"识别线程start出错" code:startCode userInfo:nil]];

        // 返回sessionID
        const char *sessionId = _asrClient.getSessionId();
        if (sessionId) {
            [self doSessionId:sessionId];
        }

        // 判断是否取消
        if ([self isCancelled])
            return;

        isPartailFirst = YES;
        hasPartail = NO;

        while (true)
        {
            NSData *audioData = nil;
            @synchronized(_recordingQueue)
            {// begin @synchronized
                if (_recordingQueue.count > 0)
                {
                    audioData = [_recordingQueue objectAtIndex:0];
//                    NSLog(@"取到数据%lu,recordqueue count = %lu ,%@",(unsigned long)audioData.length,(unsigned long)_recordingQueue.count,[NSThread currentThread]);
                    [_recordingQueue removeObjectAtIndex:0];
                }
            }

            if (audioData != nil)
            {
                char *pcmData = (char *)audioData.bytes;
                int pcmSize = (int)audioData.length;

                // 3.识别语音数据
                int recognizeCode = _asrClient.recognize(pcmData, pcmSize);
                if (recognizeCode == SUCC_CODE::ASRCLIENT_RECOGNIZER_PARTIAL_RESULT)
                {
                    /***********part result **********/
                    const char *result = _asrClient.getResult();
                    [USCLog log_d:@"%s-part result=%s",__func__,result];
//                    NSLog(@"part result=%s",result);
                    NSString *partResult = [NSString stringWithUTF8String:result];

                    [self doRecognitionResult:partResult isLast:NO];

//                    NSData *data = [partResult dataUsingEncoding:NSUTF8StringEncoding];
//
//                    NSError *error = nil;
//                    NSDictionary *resObj = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:&error];
//                    if (resObj) {
//                        NSString *part = [resObj objectForKey:ASRKEY];
//                        if (part)
//                            [self doRecognitionResult:part isLast:NO];
//                    }
                }
                else if (recognizeCode < 0)
                {
                    [self doRecognitionStop:[NSError errorWithDomain:@"识别线程识别出错" code:recognizeCode userInfo:nil]];
                }
            }
            else
            {
                if (_setToStopped)
                    break;
                else{
//                    NSLog(@"recognition sleep %@",[NSThread currentThread]);
                    [NSThread sleepForTimeInterval:0.05];
                }
            }
            if ([self isCancelled])
                return;
        }

        // 4.stop
        int stopCode = _asrClient.stop();
        if ([self isCancelled])
            return;

        // 5.get last result
        const char *result;
        if(self.recognitionParam.recognitionType == 3)
        {
            result = _asrClient.getOption(51); // vpr
        }
        else
        {
            result = _asrClient.getResult(); // asr

        }
//         NSLog(@"last result=%s",result);
        
//        pxb log
        [USCLog log_usc:@"last result:%s",result];

        NSString *resultString = [NSString stringWithUTF8String:result];
        /***********universal***********/
        if (!resultString && resultString.length == 0) {
            [self doRecognitionStop:[NSError errorWithDomain:@"最后一次结果出错" code:-1 userInfo:nil]];
        }
        [self doRecognitionResult:resultString isLast:YES];

//        pxb log
        
        
        
        
        
//        NSMutableDictionary *lastMutableDict = nil;
//        if ([self detectResultJSONCount:resultString]) {
//            lastMutableDict  =  [self separateJSONString:resultString];
//        }
//        else
//        {
//            NSData *data = [resultString dataUsingEncoding:NSUTF8StringEncoding];
//            NSError *error = nil;
//            NSDictionary *resObj = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:&error];
//            NSMutableDictionary *mResultDict = [NSMutableDictionary dictionaryWithDictionary:resObj];
//            lastMutableDict = mResultDict;
//        }
//        [USCLog log_usc:@"lastMutableDict=%@",lastMutableDict];
//
//        NSError *stopError = nil;
//        if(self.recognitionParam.recognitionType == 3) {// vpr
//            if (!lastMutableDict)
//            {
//                [self.delegate onVPRResult:nil];
//                stopError = nil;
//            }
//            else
//            {
//                if ([lastMutableDict objectForKey:@"errorMsg"]) {
//                    stopError  = [NSError errorWithDomain:[lastMutableDict objectForKey:@"errorMsg"] code:[[lastMutableDict objectForKey:@"status"] intValue] userInfo:nil];
//                }
//                else
//                {
//                    [self.delegate onVPRResult:lastMutableDict];
//                }
//            }
//        }
//        else// asr
//        {
//            if (!lastMutableDict)
//            {
//                [self doRecognitionResult:@"" isLast:YES];
//            }
//
//            if ([lastMutableDict objectForKey:ASRKEY] != nil) {
//                [self doRecognitionResult:[lastMutableDict objectForKey:ASRKEY] isLast:YES];
//            }
//            else{
//                [self doRecognitionResult:@"" isLast:YES];
//            }
//
//            [lastMutableDict removeObjectForKey:ASRKEY];
//            // if have nlu result ,return
//            if (lastMutableDict && lastMutableDict.count > 0)
//                if (_delegate && [_delegate respondsToSelector:@selector(onUnderstanderResult:)])
//                    [_delegate onUnderstanderResult:lastMutableDict];
//        }
        [self doRecognitionStop:nil];
    }

    [USCLog log_usc:@"recognition---stop"];

}

// detect last result json count
- (BOOL)detectResultJSONCount:(NSString *)resultStr
{
    NSString *parten =@"\\}\\{";
//    NSString *parten = @"\\{[^{]*:[^}]*\\}";
    NSError* error = NULL;
    NSRegularExpression *reg = [NSRegularExpression regularExpressionWithPattern:parten options:NULL error:&error];
    NSArray* match = [reg matchesInString:resultStr options:NSMatchingCompleted range:NSMakeRange(0, [resultStr length])];
    if (match.count >= 1) {
        return YES;
    }
    return NO;
}

- (NSMutableDictionary *)separateJSONString:(NSString *)string
{
    [USCLog log_usc:@"separate json string---------"];
    NSMutableString *allJsonResultStr = [NSMutableString string];// server all json result
    NSMutableDictionary *resultMDict;// return this dict
    //    NSString *parten =@"\\{[^{]*:[^}]*\\}";
    NSString *parten =@"\\}\\{";
    NSError* error = NULL;
    NSRegularExpression *reg = [NSRegularExpression regularExpressionWithPattern:parten options:NULL error:&error];
    NSArray* match = [reg matchesInString:string options:NSMatchingCompleted range:NSMakeRange(0, [string length])];// all json in match array
    if (match.count == 0) {
        return nil;
    }

    NSMutableArray *locationArray = [NSMutableArray array];
    // 计算每块json的location
    for (NSTextCheckingResult *checkResult in match) {
        [locationArray addObject:[NSNumber numberWithInt:checkResult.range.location]];
    }

    NSMutableArray *jsonArray = [NSMutableArray array];
    // 计算出每段json
    for (int i = 0; i < locationArray.count; i++) {
        NSString *jsonString;
        if (i == 0) {
            NSRange range = NSMakeRange(0, ([locationArray[i] intValue] + 1));
            jsonString = [string substringWithRange:range];
        }
        else{
            int len = [locationArray[i] intValue] - [locationArray[i - 1] intValue];
            NSRange range = NSMakeRange([locationArray[i-1] intValue] + 1, len);
            jsonString = [string substringWithRange:range];
        }
        [jsonArray addObject:jsonString];
        NSData *tempData =  [jsonString dataUsingEncoding:NSUTF8StringEncoding];
        NSDictionary *tempDict = [NSJSONSerialization JSONObjectWithData:tempData options:NSJSONReadingAllowFragments error:&error];

        if ([tempDict objectForKey:ASRKEY]) {
            [self doRecognitionResult:[tempDict objectForKey:ASRKEY] isLast:NO];
        }
    }// for

    // 处理最后这个json 可能是nlu或者asr的
    [jsonArray addObject:[string substringFromIndex:[[locationArray lastObject] intValue] + 1]];

    [USCLog log_usc:@"separated json string array= %@",jsonArray];

    NSData *tempData =  [[jsonArray lastObject] dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *tempDict = [NSJSONSerialization JSONObjectWithData:tempData options:NSJSONReadingAllowFragments error:&error];
    return [NSMutableDictionary dictionaryWithDictionary:tempDict];
}

#pragma mark - get client inforamtion

/*!
 *
 *  @brief  收集手机信息
 *
 *  @return 手机信息
 */
- (NSString *)getCollectedInfo
{
    NSMutableString *collected = [[NSMutableString alloc] init];
    // 包名
    [collected appendString:[USCUtil getPackageName]];
    [collected appendString:@":"];
    // 手机系统
    //    [collected appendFormat:@"%i", DEVICE_OS_iOS];
    [collected appendString:@":"];

    // 运营商
    NSString *carrier = [USCUtil getCarrier];
    [collected appendString:carrier];
    [collected appendString:@":"];

    // 网络类型
    [collected appendFormat:@"%i", _netType];
    [collected appendString:@":"];

    // version
    [collected appendFormat:@"%@", sdk_version];

    // \t 分隔符
    [collected appendString:@"\t"];

    // error code
    [collected appendFormat:@"%i:%i:%i", g_recognizer_code, g_conn_code, g_os_errorno];

    return collected;
}

/*!
 *
 *  @brief  获取最后一次的识别结果
 *
 *  @param lastResult
 *
 *  @return
 */
- (NSString *)getLastResult:(NSString *)lastResult
{
    NSDictionary *js = [NSJSONSerialization JSONObjectWithData:[NSKeyedArchiver archivedDataWithRootObject:lastResult] options:NSJSONReadingMutableLeaves error:nil];
    return [js objectForKey:@"asr_recongize"];
}

/*!
 *
 *  @brief  根据ip获取域名
 *
 *  @param address ip
 *
 *  @return 域名
 */
- (NSString *)getDomain:(NSString *)address
{
    if (!self.serviceAddress) {
        self.serviceAddress = @"tr.hivoice.cn";
    }
    NSLog(@"self.serviceAddress = %@", self.serviceAddress);
    struct hostent *phost = gethostbyname([self.serviceAddress UTF8String]);
    char *server = NULL;
    if (phost != NULL){
        struct in_addr *serverInAddr = (struct in_addr *)phost->h_addr_list[0];
        server = inet_ntoa(*serverInAddr);
    }

    if (server == NULL)
        server = const_cast<char *>("117.121.49.41");
    return [NSString stringWithUTF8String:server];
}

- (void)dealloc
{
    self.delegate = nil;
}

// new methad

@end
