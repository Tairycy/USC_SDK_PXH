//
//  USCSetting.h
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-5-27.
//  Copyright (c) 2015年 usc. All rights reserved.
//


//???: 这个接口下次开放出去


#import <Foundation/Foundation.h>

/**
 *  sdk 配置类,可以获取版本号，设置日志打印等级等
 */
@interface USCSetting : NSObject
/**
 *  获取当前sdk版本号
 *
 *  @return 版本号
 */
+(NSString *)getVersion;
/**
 *  设置日志打印等级
 *
 *  @param level 日志等价
 */
+ (void)setLogLevel:(int)level;
/**
 *  获取当前日志等级
 */
+(int)logLvl;
/**
 *  设置日志文件保存路径
 *
 *  @param path 日志文件路径
 */
+ (void)setLogFilePath:(NSString *)path;
@end
