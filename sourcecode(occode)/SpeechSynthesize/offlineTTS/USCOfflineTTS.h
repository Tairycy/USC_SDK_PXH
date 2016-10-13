//
//  OfflineTTS.h
//  offlineTTS_SDK
//
//  Created by yunzhisheng on 14-10-22.
//  Copyright (c) 2014年 WebSeat. All rights reserved.
//

#import <Foundation/Foundation.h>

//struct WaveHeader
//{
//    char chunk_id[4];
//    int chunk_size;
//    char format[4];
//    char subchunk1_id[4];
//    int subchunk1_size;
//    short int audio_format;
//    short int num_channels;
//    int sample_rate;			// sample_rate denotes the sampling rate.
//    int byte_rate;
//    short int block_align;
//    short int bits_per_sample;
//    char subchunk2_id[4];
//    int subchunk2_size;			// subchunk2_size denotes the number of samples.
//};

@protocol USCOfflineTTSDelegate <NSObject>

/**
 *  返回合成数据
 *
 *  @param data 数据
 */
- (void)onSynData:(NSData *)data;

/**
 *  模型加载完毕
 */
- (void)onInitFinish;

/**
 *  开始播放
 */
- (void)onPlayBegin;

/**
 *  开始收到合成的语音数据
 */
- (void)onBuffer;

/**
 *  播放取消
 */
- (void)onPlayCancel;

/**
 *  播放结束
 */
- (void)onPlayComplete;

/**
 *  返回语音数据
 *
 *  @param data 语音数据
 */
- (void)onPlayTTSData:(NSData *)data;

/**
 *  合成出错
 */
- (void)onError:(NSError *)error;

@end

/**
 *  offlineTTTS
 */
@interface USCOfflineTTS : NSObject

/**
 *  代理
 */
@property (nonatomic,weak) id delegate;

/**
 单例
 */
+ (instancetype)sharedOfflineTTS;

/**
 *  初始化TTSEngine
 *
 *  @param context   上下文
 *  @param modelPath 模型路径
 */
- (void)initTTSEngineWithContext:(id)context modelPath:(NSString *)modelPath;

@property (nonatomic,strong) NSString  *modelPath;
/**
 *  开始播放
 *
 *  @param ttsString TTS文字
 */
- (void)play:(NSString *)ttsString;

/**
 *  取消
 */
- (void)cancel;

/**
 *  停止
 */
- (void)stop;

/**
 * 播报语速设置，数值范围 0.1~2.5
 * @param d
 */
- (void)setVoiceSpeed:(float)speed;

/**
 * 调节音高的参数，为浮点数，数值范围 0.9～1.1
 * @param v
 */
- (void)setVoicePitch:(float)pitch;

/**
 * 参数设置
 */
- (void)setOption:(int)key value:(id)value;

/**
 *  销毁
 */
-(void)releaseTTSEngine;
@end
