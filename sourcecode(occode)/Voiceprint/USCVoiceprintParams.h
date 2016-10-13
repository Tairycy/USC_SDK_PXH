//
//  USCVoiceprintParams.h
//  asr&nlu&tts
//
//  Created by iOSDeveloper-zy on 15-4-17.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCConstant.h"
// @class - 声纹识别参数类
// @brief - 包含声纹识别过程中要设置的参数
@interface USCVoiceprintParams : NSObject
/**
 *  获取声纹参数类型 ，包括login和register
 *
 *  @return 类型
 */
- (USCVPRType)getVPRType;
/**
 *  设置声纹参数类型
 *
 *  @param type 类型
 *
 *  @return 成功返回YES
 */
- (BOOL)setVPRType:(USCVPRType)type;
/**
 *  当前声纹参数是否可用
 *
 *  @return YES 或 NO
 */
- (BOOL)isVPREnable;
/**
 *  设置声纹参数可用否
 *
 *  @param enable YES 或 NO
 */
- (void)setVPREnable:(BOOL)enable;
/**
 *  设置用户名
 *
 *  @param userName 用户名
 *
 *  @return YES 或 NO
 */
- (BOOL)setVPRName:(NSString *)userName;
/**
 *  获取用户名
 *
 *  @return 用户名
 */
- (NSString *)getUserName;
/**
 *  获取当前声纹参数类型的字符串形式
 *
 *  @return 声纹参数
 */
- (NSString *)vprTypeString;

@end
