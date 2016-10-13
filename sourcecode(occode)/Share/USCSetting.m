//
//  USCSetting.m
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-5-27.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCSetting.h"
#import "USCVersion.h"
#import "USCLog.h"

@implementation USCSetting
+ (void)setLogFilePath:(NSString *)path
{
    [USCLog setLogFilePath:path];
}

+ (NSString *)getVersion
{
    return sdk_version;
}

+ (void)setLogLevel:(int)level
{
    [USCLog setLogLevel:level];
}

+ (int)logLvl
{
    return [USCLog loglevel];
}
@end
