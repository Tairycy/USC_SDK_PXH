//
//  USCLog.m
//  usc
//
//  Created by Andy on 14-1-23.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCNLULog.h"

static BOOL isAllowed = NO;

@implementation USCNLULog

+(void)initialize
{
    isAllowed = NO;
}

+ (void) setAllowLogShowWith:(BOOL)allowed
{
    isAllowed = allowed;
}

+ (void) log_v:(NSString *)format, ...
{
    if (!isAllowed)
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
    if (!isAllowed)
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
    if (!isAllowed)
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
    if (!isAllowed)
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
    if (!isAllowed)
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


@end
