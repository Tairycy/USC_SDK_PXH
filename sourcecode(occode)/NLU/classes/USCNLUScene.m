//
//  USCNLUScene.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-1.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCNLUScene.h"

@interface USCNLUScene()
{



}

@end

@implementation USCNLUScene


- (id)init
{
    if (self = [super init]) {
        _mIsEnable = NO;
        _mSceneId = SCENE_DISABLED;
    }
    return self;
}


- (instancetype)scene:(int)sceneId name:(NSString *)name
{
    _mSceneId = sceneId;
    _mSceneName = name;
    return self;
}
@end
