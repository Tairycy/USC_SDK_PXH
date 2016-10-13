//
//  SynthesisThread.h
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCOfflineTTSCommon.h"

typedef enum
{
    SynthesisError = -1,
    SynthesisBegin = 0,
    SynthesisComplete,
    SynthesisCancel
}SynthesisStatus;

@protocol USCSynthesisThreadDelegate <NSObject>

/**
 *  合成开始
 */
- (void)onSynthesisBegin;

/**
 *  合成完成
 */
- (void)onSynthesisComplete;

/**
 *  返回合成的语音数据
 *
 *  @param data 一段语音数据
 */
- (void)onSynthesizeProcessData:(NSData *)data;

/**
 *  合成中途取消
 */
- (void)onSynthesisCancel;

/**
 *  合成中途出错
 */
- (void)onSynthesisError:(int)error;

@end

@interface USCSynthesisThread : NSOperation

/* 音频数组 */
@property (nonatomic,strong) NSMutableArray *audioDataArray;

/* 代理 */
@property (nonatomic,weak) id<USCSynthesisThreadDelegate> delegate;

/* 要合成的文字 */
@property (nonatomic,strong) NSString *synthesisText;

/* 模型文件路径 */
@property (nonatomic,strong) NSString *modelPath;

/* 语音音量 */
@property (nonatomic,strong) NSString *picthScale;

/* 语音语速 */
@property (nonatomic,strong) NSString *speakRate;

/**
 *  停止合成
 */
- (void)reqStop;

/* 合成状态 */
@property (nonatomic,assign) SynthesisStatus sysnthesisStatus;

- (void)destorySynthesis;

/*!
 *  @author usc_zy, 15-02-11 15:02:08
 *
 *  @brief  <#Description#>
 *
 *  @param ID     <#ID description#>
 *  @param option <#option description#>
 *
 *  @return <#return value description#>
 */
- (BOOL)setOption:(int)ID option:(NSString *)option;

@end
