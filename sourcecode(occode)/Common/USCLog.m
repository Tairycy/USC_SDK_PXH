//
//  USCLog.m
//  usc
//
//  Created by Andy on 14-1-23.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCLog.h"

static BOOL isAllowLogShow = YES;
static int logLevel = 0;

@implementation USCLog

+(void)initialize
{
    isAllowLogShow = YES;
    logLevel = 0;
}

+ (int)loglevel
{
    return logLevel;
}

+ (void)setLogLevel:(Log_Level)level
{
    logLevel = level;
}

+ (void) log_v:(NSString *)format, ...
{
    if (logLevel < LVL_V ) {
        return;
    }
    if (!isAllowLogShow)
    {
        return;
    }
    if (!format)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outStr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@", outStr);
}

+ (void) log_d:(NSString *)format, ...
{
    if (logLevel < LVL_D ) {
        return;
    }
    if (!isAllowLogShow)
    {
        return;
    }
    if (!format)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outStr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@", outStr);
}

+ (void) log_i:(NSString *)format, ...
{
    if (logLevel < LVL_I ) {
        return;
    }
    if (!isAllowLogShow)
    {
        return;
    }
    if (!format)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outStr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@", outStr);
}

+ (void) log_w:(NSString *)format, ...
{
    if (logLevel < LVL_W ) {
        return;
    }
    if (!isAllowLogShow)
    {
        return;
    }
    if (!format)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outStr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@", outStr);
}

+ (void) log_e:(NSString *)format, ...
{
    if (logLevel < LVL_E ) {
        return;
    }
    if (!isAllowLogShow)
    {
        return;
    }
    if (!format)
    {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outStr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@", outStr);
}
/**
 *  这个是内部保留打印，用来打印内部信息
 *
 *  @param format format
 */
+ (void)log_usc:(NSString *)format,...
{
    if (logLevel != LVL_USC) {
        return;
    }
    if (!format) {
        return;
    }
    va_list arglist;
    va_start(arglist, format);
    NSString *outstr = [[NSString alloc] initWithFormat:format arguments:arglist];
    va_end(arglist);
    NSLog(@"%@",outstr);
}
@end
