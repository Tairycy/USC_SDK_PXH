//
//  USCRecognition.h
//  usc
//
//  Created by hejinlai on 12-11-9.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ASRClientWrapper.h"
#import "USCUtil.h"
#import "USCParams.h"
#import "USCVoiceprintParams.h"

@class USCRecoginizerParam,USCVoiceprintParams,USCRecognitionParam;

// @class - 识别类的代理
// @brief - 代理
@protocol USCRecognitionDelegate <NSObject>

/**
 *  识别线程开启成功
 */
- (void)onRecognitionStart;

/**
 *  返回結果
 *
 *  @param result 結果
 *  @param isLast 是否是最终结果
 */
- (void)onRecognitionResult:(NSString *)result isLast:(BOOL)isLast;
// 返回语音语义
/**
 *  返回语义结果
 *
 *  @param jsonResult JSON  解析成的字典
 */
- (void)onUnderstanderResult:(NSDictionary *)jsonResult;
/**
 *  返回声纹识别结果
 *
 *  @param dict 结果是一个json解析后的字典
 */
@optional
- (void)onVPRResult:(NSDictionary *)dict;
/**
 *  识别线程停止,包括正常完成停止或出错停止
 *
 *  @param error error为nil 表示正常结束,否则则返回错误信息
 */
- (void)onRecognitionStop:(NSError *)error;
/**
 *  返回sessionId
 *
 *  @param sessionId 一次网络请求的Identity
 */
- (void)onSessionId:(NSString *)sessionId;

/**
 *  音频数据超过限定时长
 */
- (void)onMaxSpeechTimeout;
@end

// @class - 识别类
// @brief - 识别类，继承NSOperation
@interface USCRecognition : NSOperation

@property (nonatomic, strong) NSMutableArray *recordingQueue;

@property (nonatomic, assign) BOOL setToStopped;

@property (nonatomic, weak) id<USCRecognitionDelegate> delegate;

@property (nonatomic, copy) NSString *appKey;
@property (nonatomic,strong) NSString *secret;

@property (nonatomic, assign) int sampleRate;

@property (nonatomic, assign) int netType;

@property (nonatomic, copy) NSString *engine;

@property (nonatomic, copy) NSString *serviceAddress;

@property (nonatomic, copy) NSString *port;

@property (nonatomic, strong) USCUtil *resultFormat;

@property (nonatomic,assign) BOOL isReturnNluResult;

@property (nonatomic,assign) BOOL isPunctuation;

@property (nonatomic,strong) NSString *nluScenario;

@property (nonatomic,assign) BOOL isVPR;

@property (nonatomic,strong) USCVoiceprintParams *vprParam;

- (void)setParam:(USCRecoginizerParam *)param;

// 识别线程的参数类
@property (nonatomic,strong) USCRecognitionParam *recognitionParam;

@end
