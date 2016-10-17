//
//  USCUdid.m
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-3-12.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCCommonTool.h"
#import "USCOpenUDID.h"

@implementation USCCommonTool

//获取手机udid号
+ (NSString *)getUdid
{
    NSString *udid = [USCOpenUDID value];
    [[NSUserDefaults standardUserDefaults] setObject:udid forKey:@"udid"];
    return udid;
}

//获取客户端当前时间
+ (NSString *)getTime
{
    NSString *strDate = nil;
    NSDate *date = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    strDate = [dateFormatter stringFromDate:date];
    return strDate;
}
@end
