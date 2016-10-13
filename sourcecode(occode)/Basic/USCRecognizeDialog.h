//
//  USCRecognizeView.h
//  asr_old_vad_project
//
//  Created by yunzhisheng-zy on 15-1-10.
//  Copyright (c) 2015年 usc. All rights reserved.
//
#define USC_BG_WIDTH_NEW 271
#define USC_BG_HEIGHT_NEW 190

#define USC_BG_WIDTH 286
#define USC_BG_HEIGHT 217

#define USC_BUTTON_WIDTH_NEW 136
#define USC_BUTTON_HEIGHT_NEW 44

#define USC_BUTTON_WIDTH 138
#define USC_BUTTON_HEIGHT 38

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@protocol USCRecognizeViewDelegate <NSObject>

/*
 部分识别结果回调，isLast表示是否是最后一次
 */
- (void)onResult:(NSString *)result isLast:(BOOL)isLast;

/*
 识别结束回调，error为nil表示成功，否则表示出现了错误
 */
- (void)onEnd:(NSError *)error;

@end

/*!
 *
 *  @brief  语音识别控件
 *
 */
@interface USCRecognizeDialog :UIView


#pragma mark -
#pragma mark method

@property (nonatomic,weak) id<USCRecognizeViewDelegate> delegate;

/*!
 *
 *  @brief  验证
 *
 *  @param appKey appkey ，请到官网生气
 *  @param point  识别控件的中心点
 *
 *  @return 识别控件
 */
- (id)initWithAppkey:(NSString *)appKey center:(CGPoint)point;

/*!
 *
 *  @brief  显示控件，开始识别
 *
 */
- (void)start;

/*!
 *
 *  @brief  设置前后端点
 *
 *  @param frontTime 说话之前的停顿超时时间，默认3000ms
 *  @param backTime  说话之后的停顿超时时间，默认1000ms
 *
 */
- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime;

/*
 设置识别超时时间
 */
- (void)setRecognizationTimeout:(float)recognizationTime;

/*
 设置录音采样率，支持8000和16000，默认为16000
 */
- (void)setSampleRate:(int)rate;

/*
 设置识别参数
 */
- (BOOL)setEngine:(NSString *)engine;

/*
 当前的版本号
 */
+ (NSString *)getVersion;

/*
 取消识别
 */
- (void)cancel;

/*
 设置是否允许播放提示音
 */
- (void) setPlayingBeep:(BOOL)isAllowed;

/*
 设置语言
 */
- (void) setLanguage:(NSString *)language;

/*
 设置标点符号
 */
- (void) setPunctuation:(BOOL)isEnable;

/*
 设置属性
 */
- (void)setProperty:(NSString *)property forKey:(int)key;

@end
