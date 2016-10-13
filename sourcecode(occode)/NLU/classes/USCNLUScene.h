//
//  USCNLUScene.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-1.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

// @class - 场景类
// @brief - 场景模型类
static int SCENE_DISABLED = -1;
@interface USCNLUScene : NSObject
{
}
@property (nonatomic,strong) NSString  *mSceneName;
@property (nonatomic,assign) int mSceneId;
@property (nonatomic,assign) BOOL mIsEnable;
@end
