//
//  USCUploadSceneTask.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-11.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

@class USCUploadSceneTask,USCScene;

#pragma mark -
#pragma mark 协议
@protocol USCUploadSceneTaskDelegate <NSObject>

/**
 *  上传全语音词表回调
 *
 *  @param sender 上传对象
 *  @param error  错误
 */
- (void)onUploadSceneDataEnd:(USCUploadSceneTask *)sender error:(NSError *)error;

@end

#pragma mark -
#pragma mark 类

// @class - 上传全语音词表类
// @brief - 发送请求上次全语音数据
@interface USCUploadSceneTask : NSObject

@property (nonatomic,weak) id<USCUploadSceneTaskDelegate> delegate;

/**
 *  上传全语音词表
 *
 *  @param serviceKay appkey
 *  @param scene      场景对象
 *  @param array      词表数组
 */
- (void)postData:(NSString *)serviceKay Scene:(USCScene *)scene Array:(NSArray *)array;

/**
 *  获取成功上传的场景对象
 *
 *  @return 场景对象
 */
-(USCScene *)getScene;
@end
