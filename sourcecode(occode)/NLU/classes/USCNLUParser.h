//
//  USCNLPPraser.h
//  USCNLPPraser
//
//  Created by jiqiangsheng on 14-2-26.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "USCNLUParsedResult.h"

typedef enum
{
    USC_NLU_SERVICE_URL = 1000
}USC_NLU_PROPERTY;

@class USCNLUParam;

@protocol USCNLUParserDelegate <NSObject>

/**
 *  语义理解结束
 *
 *  @param error error
 */
-(void)onNLUEnd:(NSError *)error;

/**
 *  语义理解返回结果
 *
 *  @param result 返回的结果
 */
-(void)onReceivedResult:(USCNLUParsedResult *)result;

/**
 *  进入后台
 */
-(void)onEnterBackground;

@end

// @class - 语义理解类
// @brief - 根据提供的文字返回理解的结果文字
@interface USCNLUParser : NSObject

@property (nonatomic,weak) id<USCNLUParserDelegate>  delegate;

/**
 *  初始化
 *
 *  @param appkey   appkey
 *  @param secret   secret
 *  @param delegate 代理
 *
 *  @return 对象
 */
- (id)initWithAppkey:(NSString *)appkey secret:(NSString *)secret;

/**
 *  设置理解超时时间
 *
 *  @param limitTime 时间
 */
- (void)setTimeOut:(int)limitTime;

/**
 *  设置nlu参数
 *
 *  @param param 参数
 */
- (void)setNLUParam:(USCNLUParam *)param;

/**
 *  启动理解
 *
 *  @param text 理解文字
 */
- (void)parse:(NSString *)text;

/**
 *  取消语义理解
 */
- (void)cancel;

/**
 *  获取把版本号
 *
 *  @return 版本号
 */
- (NSString *)version;

/**
 *  设置选项
 *
 *  @param property 属性值
 *  @param key      属性
 */
- (void)setProperty:(NSString *)property forKey:(int)key;

/**
 *  获取属性值
 *
 *  @param key key
 *
 *  @return value
 */
- (NSString *)propertyForKey:(int)key;

/**
 *  设置场景
 *
 *  @param scenario 场景
 */
- (void)setScenario:(NSString *)scenario;

/**
 *  设置要发送的url
 *
 *  @param url url
 */
- (void)setURL:(NSString *)url;

#pragma mark - hidePublicMethod

- (NSURL *)getHttpUrl;
@end
