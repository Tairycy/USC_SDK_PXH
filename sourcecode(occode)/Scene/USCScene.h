//
//  USCScene.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-10.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

static const int SCENE_DISABLED = -1;

// @class - 场景对象
// @brief - 描述一个场景信息
@interface USCScene : NSObject
/**
 *  场景的名字
 */
@property (nonatomic,strong) NSString *mSceneName;
/**
 *  场景的id
 */
@property (nonatomic,assign) int mSceneId;

/**
 *  根据场景名和id创建场景
 *
 *  @param name 名字
 *  @param Id   id
 *
 *  @return 场景对象
 */
+(instancetype)sceneWithSceneName:(NSString *)name Id:(int)Id;

/**
 *  场景是否可用
 *
 *  @return yes or no
 */
- (BOOL)isEnabled;

/**
 *  设置场景的是否可用
 *
 *  @param enabled yes or no
 */
- (void)setEnabled:(BOOL)enabled;

/**
 *  返回场景名字
 *
 *  @return 场景名字
 */
- (NSString *)getSceneName;

/**
 *  返回场景ID
 *
 *  @return 场景ID
 */
- (int)getSceneID;
@end

