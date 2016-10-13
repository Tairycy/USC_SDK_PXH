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

+ (BOOL)detectResultJSONCount:(NSString *)resultStr
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

+ (NSString *)separateJSONString:(NSString *)string
{
    NSMutableString *allJsonResultStr = [NSMutableString string];// server all json result
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
        [locationArray addObject:[NSNumber numberWithInt:(int)checkResult.range.location]];
    }
    
    NSInteger processLen =0;
    NSMutableArray *jsonArray = [NSMutableArray array];
    // 计算出每段json
    for (int i = 0; i <locationArray.count; i++) {
        NSString *jsonString;
        if (i == 0) {
            NSRange range = NSMakeRange(0, ([locationArray[i] intValue] + 1));
            jsonString = [string substringWithRange:range];
            processLen = range.location + range.length;
        }
        else{
            int len = [locationArray[i] intValue] - [locationArray[i - 1] intValue];
            NSRange range = NSMakeRange([locationArray[i-1] intValue] + 1, len);
            jsonString = [string substringWithRange:range];
            processLen = range.location + range.length;
        }
        [jsonArray addObject:jsonString];
        
        if ([USCCommonTool asrTextResultWith:jsonString]) {
            [allJsonResultStr appendString:[self asrTextResultWith:jsonString]];
        }
    }// for
    
    if (processLen < string.length) {
        NSString *str = [string substringFromIndex:processLen];
        if ([USCCommonTool asrTextResultWith:str]) {
            [allJsonResultStr appendString:[self asrTextResultWith:str]];
        }
    }
    return allJsonResultStr;
}

// 根据一个asr json返回asr text
+ (NSString *)asrTextResultWith:(NSString *)asrJson
{
    NSError *error;
    NSData *tempData =  [asrJson dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *tempDict = [NSJSONSerialization JSONObjectWithData:tempData options:NSJSONReadingAllowFragments error:&error];
    if ([tempDict objectForKey:@"asr_recongize"]) {
        return (NSString *)[tempDict objectForKey:@"asr_recongize"];
    }
    return @"";
}
@end
