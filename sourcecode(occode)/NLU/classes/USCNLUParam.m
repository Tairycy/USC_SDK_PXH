//
//  USCNLUParam.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-8.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCNLUParam.h"

@interface USCNLUParam()
{
    NSString *_nluServer;

    int _viewID;
}
@end

@implementation USCNLUParam

- (instancetype)init
{
    if (self = [super init])
    {
        _nluServer = @"http://scv2.hivoice.cn/service/iss";
    }
    return self;
}

#pragma mark - private method


#pragma mark - public method
- (BOOL)setNLUServer:(NSString *)server NLUPort:(int)port
{
    if (server == nil) {
        return NO;
    }

    [_nluServer stringByAppendingFormat:@"http://%@:%d/service/iss",server,port];
    return YES;
}

- (NSString *)getNLUServer
{
    return _nluServer;
}

- (NSString *)getCurrentTime
{
    NSDate *date = [NSDate date];
    NSDateFormatter *formmatter = [[NSDateFormatter alloc]init];
    [formmatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *time = [formmatter stringFromDate:date];
    return time;
}

- (void)setViewId:(int)viewId
{
    _viewID = viewId;
}

- (NSString *)getViewID
{
    return [NSString stringWithFormat:@"%d",_viewID];
}
@end

