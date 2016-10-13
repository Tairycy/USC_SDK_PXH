//
//  Engine.h
//  offline_tts_sdk
//
//  Created by yunzhisheng on 14-10-30.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCOfflineTTSCommon.h"

struct WaveHeader
{
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;			// sample_rate denotes the sampling rate.
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;			// subchunk2_size denotes the number of samples.
};

@class USCOfflineEngine;

@protocol USCOfflineEngineDelegate <NSObject>

/**
 *  返回合成的语音
 */
- (void)engine:(USCOfflineEngine *)engine synthesisData:(NSData *)data;

/**
 *  合成完成
 */
- (void)synthesisComplete;

/**
 *  合成出错
 *
 *  @param error error
 */
- (void)synthesisError:(int)error;

/**
 *  合成中途取消
 */
- (void)synthesisCancel;

@end

/**
 *  offlineTTS engine
 */
@interface USCOfflineEngine : NSObject

singleton_h(USCEngine)

@property (nonatomic,weak) id<USCOfflineEngineDelegate> delegate;

- (int)synthesisData:(NSString *)text withModelPath:(NSString *)modelPath;

/**
 *  取消合成
 */
- (void)cancelProcess;

/* 语速 */
@property (nonatomic,strong) NSString *speakingRate;

 /* 音量 */
@property (nonatomic,strong) NSString *pitch;

/*!
 *
 *  @brief  设置
 *
 *  @param ID     ID
 *  @param option value
 *
 *  @return 设置参数
 */
- (BOOL)setOption:(int)ID option:(NSString *)option;

@end
