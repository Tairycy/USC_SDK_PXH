//
//  Engine.h
//  usc
//
//  Created by yunzhisheng on 14-10-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

@class Engine;

typedef enum
{
    TTSParam_IMIE = 8,
    TTSParam_UserID = 14,
    TTSParam_ClientInfo = 15
}USCTTSParam;

@protocol USCTTSEngineDelegate <NSObject>

@optional
/**
 *  返回语音数据
 *
 */
- (void )resultData:(NSData *)data;

/**
 *  返回数据结束
 */
- (void)synthesiszeEnd:(NSError *)error;

/**
 *  开始合成
 */
- (void)synthesizeBengin;

/**
 *  合成取消回调
 */
- (void)synthesiszeCancel;

@end

@interface USCTTSEngine : NSOperation

@property (weak,atomic) id<USCTTSEngineDelegate> delegate;

 /***********tts engine param***********/
@property (nonatomic,strong) NSString *vol;
@property (nonatomic,strong) NSString *spd;
@property (nonatomic,strong) NSString *vcn;
@property (nonatomic,strong) NSString *text;

/**
 *  初始化
 *
 *  @param appkey appkey
 *
 *  @return 对象
 */
- (id)initWithAppkey:(NSString *)appkey secret:(NSString *)secret;

/**
 *  设置
 *
 *  @param fromat format
 *  @param codec  codec
 */
- (BOOL)setAudioFormat:(NSString *)format codec:(NSString *)codec;

/*!
 *  @brief  开启合成线程，开始合成
 *
 *  @param text 要合成的文本
 */
- (void)start:(NSString *)text;

/**
 *
 */
- (void)cancel;

/**
 *  设置参数
 */
- (void)setOption:(int)paramID withValue:(NSString *)param;

/**
 *  获取参数
 *
 *  @param key key
 *
 *  @return value
 */
- (id)getOption:(int)key;

/*!
 *  @brief  设置tts的服务器地址
 *
 *  @param address 地址
 *  @param common  公有云私有云
 *
 *  @since 1.5
 */
- (void)setTTSAddress:(NSString *)address public:(BOOL)isPublic;
@end
