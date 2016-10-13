//
//  USCSceneManage.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-10.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

@class USCRecoginizerParam,USCScene;
// @class - 场景管理类
// @brief - 负责管理场景相关内容
@interface USCSceneManage : NSObject

/**
 *  上传场景
 *
 *  @param sceneName 场景名
 *  @param sceneData 场景数据
 *  @param delegate  代理
 */
- (void)uploadSceneData:(NSString *)sceneName sceneData:(NSArray *)sceneData lister:(id)delegate;

/**
 *  设置识别参数对象
 *
 *  @param params 参数
 */
- (void)setParams:(USCRecoginizerParam *)params;

/**
 *  根据场景名字找场景
 *
 *  @param name 场景名
 *
 *  @return 场景对象
 */
- (USCScene *)fineScene:(NSString *)name;

/**
 *  获取已经上传的场景格式
 *
 *  @return 个数
 */
- (int)getSceneCount;
@end
