//
//  USCScene.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-10.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCScene.h"

@interface USCScene()
{
    BOOL _mIsEnabled;
}
@end

@implementation USCScene

+(instancetype)sceneWithSceneName:(NSString *)name Id:(int)Id
{
    USCScene *scene = [[USCScene alloc]init];

    scene.mSceneName = name;
    scene.mSceneId = Id;

    return scene;
}

- (instancetype)init
{
    if (self = [super init]) {
        _mIsEnabled = NO;
        _mSceneId = SCENE_DISABLED;
        _mSceneName = @"";
    }
    return self;
}
- (BOOL)isEnabled
{
    return _mIsEnabled;
}

- (NSString *)getSceneName
{
    return _mSceneName;
}

- (int)getSceneID
{
    return _mSceneId;
}

- (void)setEnabled:(BOOL)enabled
{
    _mIsEnabled = enabled;
}

@end
