//
//  USCNLPPraser.m
//  USCNLPPraser
//
//  Created by jiqiangsheng on 14-2-26.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import <CoreLocation/CoreLocation.h>
#import <CommonCrypto/CommonDigest.h>
#import "USCNLUParser.h"
#import "USCOpenUDID.h"
#import "USCNLULog.h"
#import "USCReachability.h"
#import "USCNLUError.h"
#import "USCNLUVersion.h"
#import "USCNLUSetOptions.h"
#import "USCSpeechResult.h"
#import "USCNLUParam.h"
#import "USCNLUError.h"
#import "USCMarcos.h"

NSString *const METHOD = @"iss.getTalk";   //必填，方法名称，目前支持的值是：iss.getTalk
NSString *const VER = @"2.0";              //必填，服务接口版本号，目前支持的值为2.0
static NSString *URL_HEAD = @"http://scv2.hivoice.cn/service/iss";    //要发送的url的地址头
static NSString *URL_SERVICE = @"/service/iss"; // 拼接在HEAD后面
const int timeInterval = 300;               //更新gps时间间隔，单位秒
static double currentTime;

@interface USCNLUParser() <NSURLConnectionDataDelegate, CLLocationManagerDelegate>
{
    NSString *_appkey;        //必填
    NSString *_secret;        //必填，用户申请的secret
    NSString *_udid;          //选填，用户唯一的标识，可以是设备的id，用户id
    NSString *_city;          //选填，用户当前所在的城市
    NSString *_gps;           //选填，当前的gps数据，格式纬度，经度
    NSString *_time;          //选填，客户端当前时间，格式yyyy-MM-dd HH:mm:ss
    NSString *_appver;        //选填，客户端的版本号
    NSString *_text;          //必填，用户的请求文本，可以是用户输入的,也可以是由语音识别服务器提供
    NSString *_history;       //选填，历史记录，推荐客户端把上次请求的应答中的 history 信息复制到 当次请求中。

    NSString *_voiceId; // 语音日志ID

    NSString *_scenario ;// 场景

     NSString *_screen;// 屏幕大小

     NSString *_dpi;// 屏幕分辨率

    NSString  *_viewID; // 场景ID

    //这是一种服务器用来保留上下文的轻量 机制,这使用户能体验到上下文相关的语义服务。
    NSString *_appsig;        //必填，请求签名
    
    NSString *_methodPost;    //设置网络发送方式
    int _limitTime;           //网络请求超时时间
    
    NSString *_sendUrlStr;       //发送的组合以后，要发送的url
    NSURL *_url;
    NSMutableURLRequest *_request;
    NSURLConnection *_asyncConnection;
    
    NSMutableData *_receivedData;             //用于接收返回的数据
    NSUserDefaults *_userDefaults;            //用于保存数据，udid，city， gps，history，methodPost， limitTime
    NSOperationQueue *_operationQueue;        //操作队列
    
    BOOL isBegin;                             //是否开始解析
    
    CLLocationManager *locationManager;
    USCNLUSetOptions *_setOptions;
    BOOL _isFirstLaunch;                      //是否是第一次启动

    // 请求文本
    NSString *requestText;

    // 参数
    USCNLUParam *_nluParam;

    // 是否已经返回语义理解结果，如果已经返回，就不在返回定位结果
    BOOL _isReceivedResult;

}

@end

@implementation USCNLUParser

-(id)initWithAppkey:(NSString *)appkey secret:(NSString *)secret
{
    self = [super init];
    if (self)
    {
        _receivedData = [[NSMutableData alloc] init];
        _userDefaults = [NSUserDefaults standardUserDefaults];
        _setOptions = [USCNLUSetOptions sharedInstance];

        _appkey = appkey;
        _secret = secret;
        _udid = [self udid];
        _city = @"";
        _time = [self time];
        _appver = [self appver];
        _text = @"";
        _history = @"";
        _voiceId = @"";
        _scenario = @"";
        _screen = @"";
        _dpi = @"";
        _viewID = @"";
        
        _methodPost = @"POST";
        _limitTime = 600;
        
        _sendUrlStr = @"";
        
        locationManager = [[CLLocationManager alloc] init];
        locationManager.delegate = self;
        
        //将初始化数据保存在userDefaults中
        [self keepData];
        
        isBegin = NO;
        _operationQueue = [[NSOperationQueue alloc] init];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(enterBackground) name:UIApplicationDidEnterBackgroundNotification object:nil];        //当进入后台的时候，触发该函数
        
        currentTime = [[NSDate date] timeIntervalSince1970];
        
        _isFirstLaunch = YES;

        // 初始化完成后设置为NO
        _isReceivedResult = NO;
    }
    
    return self;
}
/**
 *  设置场景
 *
 *  @param scenario description
 */
- (void)setScenario:(NSString *)scenario
{
    if (scenario) {
        _scenario = scenario;
    }
}

- (void)setNLUParam:(USCNLUParam *)param
{
    if (param) {
        _nluParam = param;
    }
}

/**
 *  设置url
 *
 *  @param url url
 */
- (void)setURL:(NSString *)url
{
    if (url) {
        URL_HEAD = [url stringByAppendingString:URL_SERVICE];
//      URL_HEAD = @"http://10.30.0.241:8080/service/iss";
    }
}

//进入后台
-(void)enterBackground
{
    if (isBegin)    //进入后台的时候，如果正在解析，那么就取消解析
    {
        if (_delegate && [_delegate respondsToSelector:@selector(onEnterBackground)]) {
            [_delegate onEnterBackground];
        }
    }
}

/**
 *  设置属性
 */
- (void) setProperty:(NSString *)property forKey:(int)key
{
       [_setOptions setProperty:property forKey:key];
}

- (NSString *)propertyForKey:(int)key
{
    return [_setOptions propertyForKey:key];
}

//将将初始化数据保存在userDefaults中
-(void)keepData
{
    [_userDefaults setObject:_appkey forKey:@"appkey"];
    [_userDefaults setObject:_secret forKey:@"secret"];
    [_userDefaults setObject:_udid forKey:@"udid"];
    [_userDefaults setObject:_city forKey:@"city"];
    [_userDefaults setObject:_appver forKey:@"appver"];
    [_userDefaults setObject:_history forKey:@"history"];
    [_userDefaults setObject:_gps forKey:@"gps"];
    [_userDefaults setObject:_methodPost forKey:@"methodpost"];
    [_userDefaults setObject:[NSNumber numberWithInt:_limitTime] forKey:@"limittime"];
}

//每次开始启动解析之前更新数据
-(void)updateData
{
    _city = [_userDefaults objectForKey:@"city"];
    _gps = [_userDefaults objectForKey:@"gps"];
    _methodPost = [_userDefaults objectForKey:@"methodpost"];
    _limitTime = [[_userDefaults objectForKey:@"limittime"] intValue];
    _history = [_userDefaults objectForKey:@"history"];
    _time = [self time];
}

-(void)setTimeOut:(int)limitTime
{
    if (limitTime > 60)
    {
        limitTime = 60;
    }
    else if (limitTime < 5)
    {
        limitTime = 5;
    }
    _limitTime = limitTime;
    [_userDefaults setObject:[NSNumber numberWithInt:limitTime] forKey:@"limittime"];
}

//获取客户端版本
-(NSString *)appver
{
    NSDictionary* infoDict =[[NSBundle mainBundle] infoDictionary];
    NSString *appver = [infoDict objectForKey:@"CFBundleVersion"];
    [_userDefaults setObject:appver forKey:@"appver"];
    return appver;
}

//更新gps
-(void)updateGps
{
    //判断定位操作是否被允许
    if ([CLLocationManager locationServicesEnabled])
    {
        if ([locationManager respondsToSelector:@selector(requestAlwaysAuthorization)]) {
            [locationManager requestAlwaysAuthorization];
        }
        [locationManager startUpdatingLocation];
    }
}

//获取客户端当前时间
-(NSString *)time
{
    NSString *strDate = nil;
    NSDate *date = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    strDate = [dateFormatter stringFromDate:date];
    
    return strDate;
}

//获取手机udid号
-(NSString *)udid
{
    NSString *udid = [USCOpenUDID value];
    [_userDefaults setObject:udid forKey:@"udid"];
    return udid;
}

//获取appkey，method，text，ver，udid，appver组合后字符串再和secret组合的字符串
-(NSString *)appendString
{
    NSMutableString *appendStr = [[NSMutableString alloc] init];
    [appendStr appendString:_secret];
    [appendStr appendFormat:@"&%@", [self appendTempWithNeedingEncoded:NO], nil];
    [appendStr appendFormat:@"&%@", _secret, nil];
    
    return appendStr;
}

/*!
 *
 *  @brief  获取签名
 *
 *  @return 签名
 *
 *  @since 1.2
 */
-(NSString *)appsig
{
    NSString *appsig  = [self sha1AndhexWith:[self appendString]];
    return appsig;
}

/*!
 *
 *  @brief  sha1加密转16进制输出
 *
 *  @param string 原字符串
 *
 *  @return 转换后的字符串
 *
 *  @since 1
 */
- (NSString *)sha1AndhexWith:(NSString *)string
{
    const char *cstr = [string cStringUsingEncoding:NSUTF8StringEncoding];
    NSData *data = [NSData dataWithBytes:cstr length:[string length]];
    uint8_t digest[CC_SHA1_DIGEST_LENGTH];

    CC_SHA1(data.bytes, (unsigned int)(data.length), digest);

    Byte *bytes = digest;

    NSString *hexStr=@"";

    for(int i=0;i<20;i++)
    {
        NSString *newHexStr = [NSString stringWithFormat:@"%x",bytes[i]&0xff];//16进制数
        if([newHexStr length]==1)
        {
            hexStr = [NSString stringWithFormat:@"%@0%@",hexStr,newHexStr];
        }
        else
        {
            hexStr = [NSString stringWithFormat:@"%@%@",hexStr,newHexStr];
        }
    }

    NSMutableString* output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];

    for(int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
        [output appendFormat:@"%02x", digest[i]];

    return [output uppercaseString];

}
-(NSString *)hexStringFromString:(NSString *)string
{
    NSData *myD = [string dataUsingEncoding:NSUTF8StringEncoding];
    Byte *bytes = (Byte *)[myD bytes];
    //下面是Byte 转换为16进制。
    NSString *hexStr=@"";
    for(int i=0;i<[myD length];i++)
    {
        NSString *newHexStr = [NSString stringWithFormat:@"%x",bytes[i]&0xff];//16进制数
        if([newHexStr length]==1)
        {
            hexStr = [NSString stringWithFormat:@"%@0%@",hexStr,newHexStr];
        }
        else
        {
            hexStr = [NSString stringWithFormat:@"%@%@",hexStr,newHexStr];
        }
    } 
    return hexStr; 
}

//将中文字符串转化为uft8编码
-(NSString *)utf8String:(NSString *)string
{
    return (NSString*)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(nil, (CFStringRef)string, nil, (CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8));
}

// 将字符串加密，并返回sha1加密后的字符串
-(NSString *)calSha1With:(NSString *)string        //sha1加密方式
{
    const char *cstr = [string cStringUsingEncoding:NSUTF8StringEncoding];
    NSData *data = [NSData dataWithBytes:cstr length:[string length]];
    uint8_t digest[CC_SHA1_DIGEST_LENGTH];
    
    CC_SHA1(data.bytes, (unsigned int)(data.length), digest);
    
    NSMutableString* output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
    
    for(int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
        [output appendFormat:@"%02x", digest[i]];
    
    return output;
}

//获取签名和属性组合后发送的的url字符串内容，参数直接组合成url的时候，中文要转化为utf-8字符串，
//城市和输入的文本一般是中文，因此要转化
-(NSString *)urlStr
{
    NSMutableString *appendStr = [[NSMutableString alloc] init];
    [appendStr appendFormat:@"%@", URL_HEAD, nil];
    [appendStr appendFormat:@"?%@", [self appendTempWithNeedingEncoded:YES], nil];
    _appsig = [self appsig];
    [appendStr appendFormat:@"&appsig=%@", _appsig, nil];
    
    return appendStr;
}

#pragma mark 拼接url

//该函数只拼接appkey,appver,city,gps,history,method,text,time,udid,ver进行拼接，得到的格式如下
//appkey=xxxxx&appver=xxxx........&ver=xxxx
//参数表示是否需要给_text进行utf-8编码
-(NSString *)appendTempWithNeedingEncoded:(BOOL)isEncoded
{
    NSMutableString *appendStr = [[NSMutableString alloc] init];
        if (!_appkey)
            _appkey = @"";

        if (!_appver)
            _appver = @"";
    
        if (!_city)
            _city = @"";

        if (!_gps)
            _gps = @"";
    
        if (!_history)
            _history = @"";
    
        if (!_time)
            _time = @"";

        if (!_udid)
            _udid = @"";

        if (!_voiceId) {
            _voiceId = @"";
        }
        if (!_scenario) {
            _scenario = @"";
        }
        if (!_screen) {
            _screen = @"";
        }

        if (!_dpi) {
            _dpi = @"";
        }

        _viewID = [_nluParam getViewID];
        if (!_viewID) {
            _viewID = @"";
        }

                
    if (isEncoded)
    {
        _appkey = [self utf8String:_appkey];
        _appver = [self utf8String:_appver];
        _city = [self utf8String:_city];
        _gps = [self utf8String:_gps];
         _history = [self utf8String:_history];
        _text = [self utf8String:_text];
        _time = [self utf8String:_time];
        _udid = [self utf8String:_udid];

        _voiceId = [self utf8String:_voiceId];
        _scenario = [self utf8String:_scenario];
        _screen = [self utf8String:_screen];
        _dpi = [self utf8String:_dpi];
        _viewID = [self utf8String:_viewID];
//        NSLog(@"得到的viewID = %@",_viewID);
    }
    
    [appendStr appendFormat:@"appkey=%@", _appkey, nil];
    
    [appendStr appendFormat:@"&appver=%@", _appver, nil];
    
    [appendStr appendFormat:@"&city=%@", _city, nil];
    
    [appendStr appendFormat:@"&gps=%@", _gps, nil];
    
    [appendStr appendFormat:@"&history=%@", _history, nil];
    
    [appendStr appendFormat:@"&method=%@", METHOD, nil];
    
    [appendStr appendFormat:@"&text=%@", _text, nil];
    
    [appendStr appendFormat:@"&time=%@", _time, nil];
    
    [appendStr appendFormat:@"&udid=%@", _udid, nil];
    
    [appendStr appendFormat:@"&ver=%@", VER, nil];

    [appendStr appendFormat:@"&voiceId=%@",_voiceId,nil];

    [appendStr appendFormat:@"&scenario=%@",_scenario,nil];

    [appendStr appendFormat:@"&screen=%@",_screen,nil];

    [appendStr appendFormat:@"&dpi=%@",_dpi,nil];

    [appendStr appendFormat:@"&viewid=%@",_viewID,nil];
    
    return appendStr;
}

#pragma mark - 更新gps
- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status
{
//    NSLog(@"authorizationstatus = %d",status);
    if (status == kCLAuthorizationStatusDenied || status == kCLAuthorizationStatusNotDetermined) {
        [locationManager stopUpdatingLocation];
//        [self updateGps];
    }
}

//更新gps
-(void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{

//    NSLog(@"didUpdateLocations");
    //此处locations存储了持续更新的位置坐标值，取最后一个值为最新位置，如果不想让其持续更新位置，则在此方法中获取到一个值之后让locationManager stopUpdatingLocation
    CLLocation *currentLocation = [locations lastObject];
    CLLocationCoordinate2D coor = currentLocation.coordinate;
    //获取纬度
    NSString *latitude = [NSString stringWithFormat:@"%.5f", coor.latitude];
    //获取经度
    NSString *longitude = [NSString stringWithFormat:@"%.5f", coor.longitude];
    
    //产生所需要的gps格式
    _gps = [NSString stringWithFormat:@"%@,%@", latitude, longitude, nil];
    [_userDefaults setObject:_gps forKey:@"gps"];


    //获取城市
    CLGeocoder *geoCoder = [[CLGeocoder alloc] init];
    [geoCoder reverseGeocodeLocation:currentLocation completionHandler:^(NSArray *placemarks, NSError *error){
        if (!error)
        {
            CLPlacemark *place = [placemarks lastObject];
            _city = place.administrativeArea;
//            NSLog(@"成功获取城市 = %@",_city);
            [_userDefaults setObject:_city forKey:@"city"];
            
            BOOL updateGps = [[_setOptions propertyForKey:USCNLU_LOCATION_SERVICE] boolValue];
            if (updateGps)
            {
                if (_gps && ![_gps isEqualToString:@""] && _city && ![_city isEqualToString:@""])
                {
                    if (_isFirstLaunch)
                    {
                        [self updateData];
                        _sendUrlStr = [self urlStr];
                        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                            [self send];
                        });
                    }
                    _isFirstLaunch = NO;
                }
            }
        }
        else
        {
            USCLog(@"没有获取到城市");

            if (!_isReceivedResult) {

                NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_LOCATION_NOT_FOUND] code:USCNLU_LOCATION_NOT_FOUND userInfo:nil];
                if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)])
                {
                    [_delegate onNLUEnd:error];
                }
            }

        }

        [manager stopUpdatingLocation];      //停止更新地理位置
    }];
}

//gps更新失败
-(void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    if (error.code == kCLErrorDenied)
    {
        _isFirstLaunch = YES;
        isBegin = NO;
    }
}

//启动发送线程
-(void)parse:(NSString *)text
{
    USCLog(@"开始解析文本：%@",text);
    _isReceivedResult = NO;
    requestText = text;
    if (isBegin)
    {
        [self cancel];
    }

    USCReachability *reach = [USCReachability reachabilityForInternetConnection];
    if (![reach isReachable]) {
        NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_NET_UNREACHABLE] code:USCNLU_NET_UNREACHABLE userInfo:nil];
        if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)]) {
//            NSLog(@"网络连接error");
            [_delegate onNLUEnd:error];
        }
        return;
    }

    if ([text isEqualToString:@""] || !text)
    {
        NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_CONTENT_PRASED_NIL] code:USCNLU_CONTENT_PRASED_NIL userInfo:nil];
        if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)])
        {
            [_delegate onNLUEnd:error];
        }
        return;
    }
    _text = text;
    
    BOOL updateGps = [[_setOptions propertyForKey:USCNLU_LOCATION_SERVICE] boolValue];
//    NSLog(@"updateGPS = %d",updateGps);
    if (updateGps)
    {
        if (_isFirstLaunch)
        {
            [self updateGps];
        }
        else
        {
            //更新gps，城市
            double mCurrentTime = [[NSDate date] timeIntervalSince1970];
            if (mCurrentTime - currentTime >= timeInterval)
            {
                [self updateGps];
                currentTime = mCurrentTime;
            }
            
            [self updateData];
            _sendUrlStr = [self urlStr];
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [self send];
            });
        }
    }
    else
    {
        [self updateData];
        _sendUrlStr = [self urlStr];
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [self send];
        });
    }
}

// 取消解析
-(void)cancel
{
    if (isBegin)
    {
        [_operationQueue cancelAllOperations];
        [_asyncConnection cancel];
        isBegin = NO;
        NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_JOB_CANCELED] code:USCNLU_JOB_CANCELED userInfo:nil];
        if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)])
        {
            [_delegate onNLUEnd:error];
        }
    }
}
#pragma mark hidePublicMethod
- (NSURL *)getHttpUrl
{
    return _url;
}

#pragma mark - 发送请求

//发送信息，请求结果
-(void)send
{
    _url = [[NSURL alloc] initWithString:_sendUrlStr];
//    _nluParam;
//    NSLog(@"发送的网络请求url= %@",_url);


    _request = [[NSMutableURLRequest alloc] initWithURL:_url];
    [_request setCachePolicy:NSURLRequestUseProtocolCachePolicy];
    [_request setTimeoutInterval:_limitTime];

    //设置请求方式以及字符串的编码方式
    [_request setHTTPMethod:_methodPost];
    [_request setValue:@"UTF-8" forHTTPHeaderField:@"Charset"];

    _asyncConnection = [[NSURLConnection alloc] initWithRequest:_request delegate:self startImmediately:NO];
    [_asyncConnection setDelegateQueue:_operationQueue];
    [_asyncConnection start];
    
    isBegin = YES;
}

// 网络请求过程中，出现任何错误（断网，连接超时等）会进入此方法
-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    if ([connection isEqual:_asyncConnection])
    {
        NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_CONNECT_TIME_OUT] code:USCNLU_CONNECT_TIME_OUT userInfo:nil];
        dispatch_async(dispatch_get_main_queue(), ^{
            if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)]) {
                [_delegate onNLUEnd:error];
            }
        });
        isBegin = NO;
    }
}

// 收到服务器回应的时候调用该方法
-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
//    NSLog(@"收到相应--");
    //收到服务器回应以后，准备接收数据，开始创建_data来接收数据
    [_receivedData setLength:0];
}

// 接收到服务器传输数据的时候调用，此方法根据数据大小执行若干次
-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    if ([connection isEqual:_asyncConnection])
    {
        [_receivedData appendData:data];
    }
}

// 服务器返回数据完毕后回调
-(void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    if ([connection isEqual:_asyncConnection])
    {
        isBegin = NO;

        long dataSize = [_receivedData length];
         // 1.如果返回有数据
        if (dataSize > 0)
        {
            NSString *jsonString = [[NSString alloc] initWithBytes:[_receivedData bytes] length:[_receivedData length] encoding:NSUTF8StringEncoding];
            int rcCode = [self rcCodeInJSONString:jsonString];

            // 返回所有的内容和简略信息
            NSString *text = [self textInJSONString:jsonString];
            USCNLUParsedResult *result = [[USCNLUParsedResult alloc] init];
            result.jsonString = jsonString;
            result.text = text;

            USCSpeechResult *speechResult = [[USCSpeechResult alloc]init];
            speechResult.stringResult = jsonString;
            speechResult.responseText = text;
            speechResult.scenario = _scenario;
            speechResult.requestText = requestText;

            // 有理解结果,
            _isReceivedResult = YES;

            // 1.1 把结果返回出去
            dispatch_async(dispatch_get_main_queue(), ^{
                
                if (_delegate && [_delegate respondsToSelector:@selector(onReceivedResult:)]) {
                    [_delegate onReceivedResult:result];
                }
            });
            
            // 当操作失败的时候，首先根据错误码，得到相应的错误大致信息，如果有错误信息，那么输出错误信息
            NSMutableString *errorAllMessage = [[NSMutableString alloc] init];
            NSString *err = [USCNLUError errorMessageWith:rcCode];
            [errorAllMessage appendFormat:@"%@%@", err, @"---", nil];
            NSString *errorMessage = [self errorMessageInJSONString:jsonString];    //操作结果
            if (errorMessage)
            {

                [errorAllMessage appendFormat:@"%@", errorMessage, nil];
                
                NSError *error = [[NSError alloc] initWithDomain:errorAllMessage code:rcCode userInfo:nil];
                dispatch_async(dispatch_get_main_queue(), ^{
                    if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)]) {
                        [_delegate onNLUEnd:error];
                    }
                });
            }
            else // 表示语义返回结果正确，结束
            {
                dispatch_async(dispatch_get_main_queue(), ^{

                    if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)]) {
                        [_delegate onNLUEnd:nil];
                    }
                });
            }

            NSString *history = [self historyInJSONString:jsonString];    // 获取返回的history
            if (history)
            {
                _history = history;
                [_userDefaults setObject:history forKey:@"history"];
            }
        }
        else  // 2.无返回结果
        {
            NSError *error = [[NSError alloc] initWithDomain:[USCNLUError errorMessageWith:USCNLU_RETURN_FROM_SEVER_NIL] code:USCNLU_RETURN_FROM_SEVER_NIL userInfo:nil];
            dispatch_async(dispatch_get_main_queue(), ^{
                if (_delegate && [_delegate respondsToSelector:@selector(onNLUEnd:)]) {
                    [_delegate onNLUEnd:error];
                }
            });
        }
    }
}

//获取扼要文本信息
-(NSString *)textInJSONString:(NSString *)jsonString
{
    NSString *text = nil;
    int jsonStrLength = (int)[jsonString length];
    
    NSRange rangGeneral = [jsonString rangeOfString:@"general"];
    if (rangGeneral.location != NSNotFound)                     //如果存在general属性
    {
        NSString *generalStr = [jsonString substringWithRange:NSMakeRange(rangGeneral.location, jsonStrLength - rangGeneral.location)];
        
        NSRange rangText = [generalStr rangeOfString:@"text"];  //然后获取text属性坐在的位置
        NSRange rangBrace = [generalStr rangeOfString:@"}"];    //获取general字符串中的第一个右括号的位置
        if ((rangText.location != NSNotFound) && (rangBrace.location != NSNotFound))
        {
            NSRange rangTextContent = NSMakeRange(rangText.location + 7, rangBrace.location - rangText.location - 8);
            NSString *string = [generalStr substringWithRange:rangTextContent];
            text = string;
        }
    }
    return text;
}

//获取历史记录
-(NSString *)historyInJSONString:(NSString *)jsonString
{
    NSString *history = nil;
    int jsonStrLength = (int)[jsonString length];
    NSRange rangHistory = [jsonString rangeOfString:@"history"];
    if (rangHistory.location != NSNotFound)                    //如果存在history属性
    {
        NSString *historyStr = [jsonString substringWithRange:NSMakeRange(rangHistory.location, jsonStrLength - rangHistory.location)];
        NSRange rangBrace = [historyStr rangeOfString:@"}"];
        if (rangBrace.location != NSNotFound)
        {
            NSRange rangHistoryContent = NSMakeRange(10, rangBrace.location -11);
            NSString *string = [historyStr substringWithRange:rangHistoryContent];
            history = string;
        }
    }
    return history;
}

// 获取错误message
-(NSString *)errorMessageInJSONString:(NSString *)jsonString
{
    NSString *message = nil;
    int jsonStrLength = (int)[jsonString length];
    
    NSRange rangError = [jsonString rangeOfString:@"error"];
    if (rangError.location != NSNotFound)                     //如果存在error属性的话
    {
        NSString *errorStr = [jsonString substringWithRange:NSMakeRange(rangError.location, jsonStrLength - rangError.location)]; //获得错误信息
        
        NSRange rangMessage = [errorStr rangeOfString:@"message"];
        NSRange rangBrace = [errorStr rangeOfString:@"}"];
        if ((rangMessage.location != NSNotFound) && (rangBrace.location != NSNotFound))
        {
            NSRange rangMessageContent = NSMakeRange(rangMessage.location + 10, rangBrace.location - rangMessage.location - 12);
            NSString *string = [errorStr substringWithRange:rangMessageContent];
            message = string;
        }
    }
    return message;
}

//获取rc码
-(int)rcCodeInJSONString:(NSString *)jsonString
{
    int rcCode = -1;
    //由于rc字段比较靠前，因此我们不必使用整个字符串，一般json字符串的签几个字符是{"rc":0,"text":"
    int jsonStrLength = (int)[jsonString length];
    if (jsonStrLength > 0)
    {
        NSRange rangSample = NSMakeRange(0, 20);
        NSString *sampleStr = [jsonString substringWithRange:rangSample];
        NSRange rangRc = [sampleStr rangeOfString:@"rc"];
        NSRange rangText = [sampleStr rangeOfString:@"text"];
        if ((rangRc.location != NSNotFound) && (rangText.location != NSNotFound))
        {
            NSRange rangRccode = NSMakeRange(rangRc.location + 4, rangText.location - rangRc.location - 6);
            NSString *code = [sampleStr substringWithRange:rangRccode];
            rcCode = [code intValue];
        }
    }
    return rcCode;
}

-(void)dealloc
{
    [_operationQueue cancelAllOperations];
    [_operationQueue waitUntilAllOperationsAreFinished];
}
@end