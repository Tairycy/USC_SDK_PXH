//
//  USCLog.h
//  usc
//
//  Created by Andy on 14-1-23.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    LVL_USC = 120629,
    LVL_N = 0,// 不打印日志
    LVL_E = 1,// 日志打印等级
    LVL_W = 2,
    LVL_I = 3,
    LVL_D = 4,
    LVL_V = 5

}Log_Level;

/**
 *  日志类
 */
@interface USCLog : NSObject

+ (void) log_v:(NSString *)format, ...;
+ (void) log_d:(NSString *)format, ...;
+ (void) log_i:(NSString *)format, ...;
+ (void) log_w:(NSString *)format, ...;
+ (void) log_e:(NSString *)format, ...;
+ (void) log_usc:(NSString *)format,...;


+ (void)setLogLevel:(Log_Level)level;

+ (void)setLogFilePath:(NSString *)path;

+ (int)loglevel;
@end
