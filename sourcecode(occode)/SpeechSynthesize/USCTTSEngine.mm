//
//  Engine.m
//  usc
//
//  Created by yunzhisheng on 14-10-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCTTSEngine.h"
#import "USCLog.h"
#import "tts_client.h"
#import "USCUtil.h"
#import "set_option_key.h"

/**
 *  tts getResult返回类型
 */
enum{
    USC_SUCCESS = 0,
    RECEIVING_AUDIO_DATA = 1,// 有数据返回
    AUDIO_DATA_RECV_DONE = 2,// 数据返回完成
    SYNTH_PROCESS_ERROR = 3,// 出错
};

/***********语音合成参数key常量***********/
static const NSString *USCSyntheszieParam_Vol = @"vol"; // 音高 取值 -12~12 0为标准音高
static const NSString *USCSyntheszieParam_Spd = @"spd";
static const NSString *USCSyntheszieParam_Vcn = @"vcn";

@interface USCTTSEngine()
{
    NSString *host;
    NSString *port;
    NSString *_appkey;
    NSString *_secret;
    NSString *_format;
    NSString *_codec;
    BOOL isSynthesisStop;
    BOOL _isPublic;
    BOOL isCanceled;
    // 当前返回的数据是不是第一次返回数据
    BOOL isFirstData;
}
@property (nonatomic,strong) NSMutableData *allData;
@property (nonatomic,strong) NSMutableData *resultData;

/*!
 *  @brief  用来保存tts返回的数据，然后在传给播放线程
 *
 */
@property (nonatomic,strong) NSMutableData *mData;
@property (nonatomic,strong) NSMutableData *testData;
@property (nonatomic,strong) NSMutableArray *testArray;
@end

@implementation USCTTSEngine

#pragma mark -
#pragma mark public method
- (instancetype)initWithAppkey:(NSString *)appkey secret:(NSString *)secret
{
    if (self = [super init]) {
        _appkey = appkey;
        _secret = secret;
        host = @"vpr.hivoice.cn";
        port = @"80";
        _format = @"audio/x-wav;";
        _codec = @"opus";
        _isPublic = YES;
        isSynthesisStop = NO;
        isFirstData = NO;
        _allData = [[NSMutableData alloc]init];
        _vol = @"50";
        _spd = @"50";
        _vcn = @"xiaoli";
    }
    return self;
}

- (void)cancel
{
    isSynthesisStop = YES;
    self.delegate = nil;
}

- (void)start:(NSString *)text
{
    [self reset];
    self.text = text;
    [[[NSOperationQueue alloc]init] addOperation:self];
}

- (void)setTTSAddress:(NSString *)address public:(BOOL)isPublic
{
    if (isPublic) {
        return;
    }

    if (address) {
        host = [[address componentsSeparatedByString:@":"] firstObject];
        port = [[address componentsSeparatedByString:@":"] lastObject];
    }
    _isPublic = isPublic;
}

- (void)setOption:(int)paramID withValue:(NSString *)param
{
    if (!param || param.length <= 0) {
        return;
    }
    switch (paramID) {
        case 1:
            self.vol = param;
            break;
        case 2:
            self.vcn = param;
            break;
        case 3:
            self.spd = param;
        default:
            break;
    }
}

- (id)getOption:(int)key
{
    switch (key) {
        case 1:
            return self.vol;
            break;
        case 2:
            return self.vcn;
            break;
        case 3:
            return self.spd;
            break;
        default:
            break;
    }
    return nil;
}

#pragma mark -
#pragma mark private method
- (NSMutableData *)testData
{
    if (!_testData) {
        _testData = [NSMutableData data];
        NSMutableData *alldata = [NSMutableData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"audio.pcm" ofType:nil]];

        NSMutableArray *mArray =[NSMutableArray array];

        int  len = 160;
        for (int i = 0; i < 10; i++) {
            NSData *data = [alldata subdataWithRange:NSMakeRange(len * i, len)];
            [mArray addObject:data];
        }
    }
    return _testData;
}

- (NSMutableData *)mData
{
    if (!_mData) {
        _mData = [[NSMutableData alloc]init];
    }
    return _mData;
}

- (NSMutableData *)resultData
{
    if (!_resultData)
    {
        _resultData = [[NSMutableData alloc]init];
    }
    return _resultData;
}

/*!
 *
 *  @brief  判断当前的合成线程的代理是否为空
 */
- (BOOL )isCanceled
{
   return self.delegate == nil;
}

/*!
 *  @author usc_zy, 15-01-22 16:01:49
 *
 *  @brief 当前合成线程合成是否取消
 */
- (BOOL)isStop
{
    if([self isCanceled]) {
        return true;
    }

    return isSynthesisStop;
}

- (BOOL)setAudioFormat:(NSString *)format codec:(NSString *)codec
{
    if (!format || format.length == 0) {
        return NO;
    }

    if (!codec || codec.length == 0) {
        return NO;
    }

    _format = format;
    _codec = codec;

    return YES;
}

/*!
 *  @brief  第一次返回数据都要把数据分成四份返回。保证合成的返回的数据小于三次时也能播放
 *
 *  @param data 数据
 */
- (void)returnFirstDataWithData:(NSData *)data
{
    NSMutableData *mData = [NSMutableData dataWithData:data];
    int perSize = (int)data.length / 4;
    NSData *firstPartData = [mData subdataWithRange:NSMakeRange(0, perSize)];
    NSData *secondPartData = [mData subdataWithRange:NSMakeRange(perSize, perSize)];
    NSData *thirdPartData = [mData subdataWithRange:NSMakeRange(perSize * 2, perSize)];
    NSData *fourthPartData = [mData subdataWithRange:NSMakeRange(perSize * 3, mData.length - 3 * perSize)];

    NSArray *array = @[firstPartData,secondPartData,thirdPartData,fourthPartData];

    for (NSData *data in array) {
        if (self.delegate && [self.delegate respondsToSelector:@selector(resultData:)]) {
            [self.delegate resultData:data];
        }
    }
}


- (void)onDelegatEvent:(int)eventId object:(id)obj
{
    if (!self.delegate) {
        return;
    }

    switch (eventId) {
        case 0:
            if ([self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
                 [USCLog log_usc:@"synthesiszeEnd--3"];
                [self.delegate synthesiszeEnd:obj];
            }
            break;
        case 1:
            if ([self.delegate respondsToSelector:@selector(synthesizeBengin)]) {
                [self.delegate synthesizeBengin];
            }
            break;
        case 2:
            if ([self.delegate respondsToSelector:@selector(synthesiszeCancel)]) {
                [self.delegate synthesiszeCancel];
            }
            break;
        case 3:
            if ([self.delegate respondsToSelector:@selector(resultData:)]) {
                [self.delegate resultData:obj];
            }
            break;
        default:
            break;
    }
}

// 状态变量复位
- (void)reset
{
    isSynthesisStop = NO;
}

#pragma mark -
#pragma mark 用线程的方法
- (void)main
{
    // reset var
    isFirstData = YES;
    unsigned int text_len = 0;
    unsigned int audio_len = 0;
    int synth_status = RECEIVING_AUDIO_DATA;
    int status;

    // condition check
    if ([self isCancelled]) {
        return;
    }

    // text error
    if (!_text || _text.length == 0) {
        [self onDelegatEvent:0 object:[NSError errorWithDomain:@"合成文本错误" code:-1 userInfo:nil]];
        return;
    }

    [self onDelegatEvent:1 object:nil];
    const char* synthesizeText = [_text UTF8String];
    [USCLog log_usc:@"synthesizeText:%s",synthesizeText];

    if([self isStop])
        return;

    TTSHANDLE  handler = 0;
    if (NO) {
//        status = tts_create_service(&handler);
    }
    else
        status = tts_create_service_ext(&handler, [host UTF8String], [port UTF8String]);
       [USCLog log_usc:@"host1312=%@,port=%@",host,port];

    if (status != 0)
    {
        // create service error
        [self onDelegatEvent:0 object:[NSError errorWithDomain:@"创建合成服务失败" code:status userInfo:nil]];
        return;
    }
    else
        [USCLog log_d:@"usc_tts_create success\n--------"];

    tts_set_option(handler, "appkey", [_appkey UTF8String]);
    tts_set_option(handler, "audioFormat", [_format UTF8String]);
    tts_set_option(handler, "audioCodec", [_codec UTF8String]);
    tts_set_option(handler,"cryptVersion","1");
    tts_set_option(handler,"secret",[_secret UTF8String]);

     /***********1.1设置客户端信息***********/
    NSString *imei = [USCUtil getLocalMacAddress];
    if (!imei) {
        imei = @"unknow device";
    }

    status = tts_set_option_id(handler,OPT_IMEI_ID, [imei UTF8String]);
    status = tts_set_option_id(handler,OPT_CARRIER, [[USCUtil getCarrier] UTF8String]);

    /***********1.2设置合成参数***********/
    [USCLog log_usc:@"%s vol=%@,spd=%@,vcn=%@",__func__,_vol,_spd,_vcn];
    status = tts_set_option(handler, [USCSyntheszieParam_Vol UTF8String], [_vol UTF8String]);
    status = tts_set_option(handler, [USCSyntheszieParam_Spd UTF8String], [_spd UTF8String]);
    status = tts_set_option(handler, [USCSyntheszieParam_Vcn UTF8String], [_vcn UTF8String]);

     /***********2.开启合成***********/
    status = tts_start_synthesis(handler);

    if (status != 0) {
        // start service error
        [self onDelegatEvent:0 object:[NSError errorWithDomain:@"开启合成服务失败！" code:status userInfo:nil]];
        [USCLog log_usc:@"usc_tts_start_synthesis failed"];
        goto release_end;
    }
    else
       [USCLog log_d:@"usc_tts_start_synthesis success\n--------------"];

    if (_text == nil || _text.length == 0)
    {
         [USCLog log_usc:@"synthesis text error!\n--------------"];
        return;
    }

     /*********** 3.输入要合成文本***********/
    text_len = (unsigned int)strlen(synthesizeText);
    status = tts_text_put(handler,synthesizeText, text_len);
    if ( status != 0)
    {
        tts_stop_synthesis(handler);
        // text put error
        [self onDelegatEvent:0 object:[NSError errorWithDomain:@"输入合成文本出错" code:status userInfo:nil]];
//        goto stop_end;
        goto release_end;
    }

    while ([self isStop] == NO)
    {
        const void *data;
         /***********4. get tts data in process***********/
        data = tts_get_result(handler, &audio_len, &synth_status, &status);
        if (NULL != data)
        {
            NSData *tempData = [NSData dataWithBytes:data length:audio_len];
            [self.resultData appendData:tempData];
            // fetch the first tts data
            if (isFirstData)
                [self returnFirstDataWithData:tempData];
            else
            {
                // 返回数据
                if (self.delegate ) {
                    if([self.delegate respondsToSelector:@selector(resultData:)]) {
                        [self.delegate resultData:tempData];
                    }
                }
            }
            isFirstData = NO;
        }
        if (synth_status == AUDIO_DATA_RECV_DONE || status != 0)
            break;
    }

    status = tts_stop_synthesis(handler);
    if ( status != 0 ){
        if (self.delegate && [self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
            [self.delegate synthesiszeEnd:[NSError errorWithDomain:@"合成停止错误" code:status userInfo:nil]];
        }
    }

    goto release_end;
     /***********goto label***********/
//     stop_end:
//        if(handler != 0) {
//            int statusCode = tts_stop_synthesis(handler);
//            if( statusCode != 0){
//                if (self.delegate && [self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
//                    [self.delegate synthesiszeEnd:[NSError errorWithDomain:@"合成引擎stop 失败" code:statusCode userInfo:nil]];
//                }
//            }
//        }

     release_end:
        if(handler != 0) {
            int statusCode;
//            statusCode = tts_stop_synthesis(handler);
//            if( statusCode != 0){
//                if (self.delegate && [self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
//                    [self.delegate synthesiszeEnd:[NSError errorWithDomain:@"合成引擎stop 失败" code:statusCode userInfo:nil]];
//                }
//            }

             statusCode = tts_release_service(handler);
            if(statusCode == 0){
                if (self.delegate && [self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
                    [self.delegate synthesiszeEnd:nil];
                }
            }
            else{
                if (self.delegate && [self.delegate respondsToSelector:@selector(synthesiszeEnd:)]) {
                    [USCLog log_usc:@"synthesiszeEnd--5"];
                    [self.delegate synthesiszeEnd:[NSError errorWithDomain:@"合成引擎release 失败" code:statusCode userInfo:nil]];
                }
            }

            handler = 0;
        }
}
@end
