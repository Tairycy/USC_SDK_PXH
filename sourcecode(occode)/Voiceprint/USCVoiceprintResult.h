//
//  USCVoiceprintResult.h
//  asr&nlu&tts
//
//  Created by iOSDeveloper-zy on 15-4-17.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
/**
 *  声纹结果类，声纹识别返回的结果
 */
@interface USCVoiceprintResult : NSObject
/**
 *  注册或登录使用的用户名
 */
@property (nonatomic,strong)  NSString *userName;
/**
 *  声纹识别网络请求的状态码
 */
@property (nonatomic,assign) int statusCode;
/**
 *  声纹识别的匹配度，默认为0，登录时这个参数才有意义，范围0-60，大于60才表示匹配成功
 */
@property (nonatomic,assign) double score;

@end
