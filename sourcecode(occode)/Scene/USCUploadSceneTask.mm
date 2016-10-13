//
//  USCUploadSceneTask.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-11.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCUploadSceneTask.h"
#import "USCScene.h"
#import "USCUtil.h"
#import "USCErrorCode.h"

static NSString *userDataServer = @"http://10.10.11.104:8889/data-process-service/rtc";

@interface USCUploadSceneTask()
{
    // 上次数据拼接的字符串
    NSString *_mUploadData;

    // 场景
    USCScene *_mScene;
}

// 服务器返回的数据
@property (nonatomic,strong) NSMutableData *receiveData;

@end

@implementation USCUploadSceneTask

#pragma mark - lazy
- (NSMutableData *)receviveData
{
    if (!_receiveData) {
        _receiveData = [[NSMutableData alloc]init];
    }
    return _receiveData;
}

#pragma mark - public method

- (instancetype)init
{
    if (self = [super init]) {
        _mScene = nil;
    }
    return self;
}

-(USCScene *)getScene
{
    return _mScene;
}

- (void)postData:(NSString *)serviceKay Scene:(USCScene *)scene Array:(NSArray *)array
{
    _mScene = scene;
    [_mScene setEnabled:NO];

    NSMutableString *userData = [[NSMutableString alloc]initWithString:userDataServer];
    [userData appendFormat:@"?ak=%@&imei=%@&an=wechar&si=%d&av=1.0&sn=abcdefg&trace=1",serviceKay,[USCUtil getLocalMacAddress],scene.mSceneId];
    userDataServer = userData;

    NSMutableString *sb = [[NSMutableString alloc]init];
    [sb appendString:@"data=<SCENE>\n"];
    for (NSString *str in array) {
        [sb appendFormat:@"%@\n",str];
    }
    [sb appendString:@"</SCENE>"];

    [self postData:sb];
}

#pragma mark - private method

- (void)uploadDataEnd:(int)code
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(onUploadSceneDataEnd:error:)]) {
        NSString *msg = [USCErrorCode getErrorMsg:code];
        [self.delegate onUploadSceneDataEnd:self error:[NSError errorWithDomain:msg code:code userInfo:nil]];
    }
}

// 上传
- (void)postData:(NSString *)data
{
    _mUploadData = data;

    [self start];
}

/**
 *  发送请求开始上传
 */
- ( void)start
{
    if (_mUploadData.length == 0 || !_mUploadData) {
        NSError *error = [NSError errorWithDomain:@"上传得数据为空或不存在" code:-100000 userInfo:nil];
        if (self.delegate && [self.delegate respondsToSelector:@selector(onUploadSceneDataEnd:error:)]) {
            [self.delegate onUploadSceneDataEnd:self error:error];
        }
        return;
    }

    char *sourceData = const_cast<char*>([_mUploadData UTF8String]);
    int sourceLen = (int)strlen(sourceData);

    NSURL *url = [NSURL URLWithString:userDataServer];

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc]initWithURL:url];

    [request setHTTPMethod:@"POST"];

    [request setHTTPBody:[NSData dataWithBytes:sourceData length:sourceLen]];

    [request setTimeoutInterval:30];

    NSURLConnection *conn = [[NSURLConnection alloc]initWithRequest:request delegate:self];
    [conn start];
}

#pragma mark - urlconnnection delegate

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    NSHTTPURLResponse *res = (NSHTTPURLResponse *)response;
    [self.receviveData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    [self.receiveData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    NSError *error2 = [NSError errorWithDomain:@"上传用户数据，网络出错" code:0 userInfo:nil];
    if (_delegate && [_delegate respondsToSelector:@selector(onUploadSceneDataEnd:error:)]) {
        [_delegate onUploadSceneDataEnd:self error:error2];
    }
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    NSString *result = [[NSString alloc] initWithData:self.receiveData  encoding:NSUTF8StringEncoding];
    int errorCode = 0;

    NSArray *array = [result componentsSeparatedByString:@"\""];


    int codeInt = [[array objectAtIndex:3] intValue];

    if (codeInt == 0) {
        [_mScene setEnabled:YES];
        errorCode = RECOGNIZER_OK;

    }else if(codeInt == -1)
    {
        errorCode = UPLOAD_SCENE_GENERAL_ERROR;

    }else if(codeInt == -5)
    {
        errorCode = UPLOAD_SCENE_INVALID_KEY;

    }else if (codeInt == -8 )
    {
        errorCode = UPLOAD_SCENE_STREAM_IO_ERR;
    }
    else if (codeInt == -11)
    {
        errorCode = UPLOAD_SCENE_UNKNOWN_ERR;
    }
    else if (codeInt == -12)
    {
       errorCode = UPLOAD_SCENE_DATA_SIZE_IS_FORBIDDEN;
    }
    else if (codeInt == -13)
    {
        errorCode = UPLOAD_SCENE_INVALID_VER;
    }
    else if (codeInt == -6)
    {
      errorCode = UPLOAD_SCENE_DATA_TOO_FAST;
    }
    else
    {
        errorCode = UPLOAD_SCENE_DATA_NETWORK_ERROR;
    }

    // 传给代理对象
    [self uploadDataEnd:codeInt];
}
@end
