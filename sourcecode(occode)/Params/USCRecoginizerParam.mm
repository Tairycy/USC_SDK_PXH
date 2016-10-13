//
//  USCRecoginizerParam.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-11.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCRecoginizerParam.h"
#import "USCScene.h"
#import "USCEngineParams.h"
#import "USCVADParam.h"

@interface USCRecoginizerParam()
{
    USCScene *_scene;
}
@end

@implementation USCRecoginizerParam

- (instancetype)init
{
    if (self = [super init]) {
        _scene = [USCScene sceneWithSceneName:@"" Id:SCENE_DISABLED];
        _engineParam = [[USCEngineParams alloc]init];
    }
    return self;
}

#pragma mark - public method 

- (USCScene *)getStartScene
{
    return _scene;
}

- (NSString *)getAppkey
{
    return _appkey;
}

- (void)setStartScene:(USCScene *)scene
{
    _scene.mSceneName = scene.mSceneName;
    _scene.mSceneId = scene.mSceneId;
    [_scene setEnabled:[scene isEnabled]];
}

//- (void)setFarVoiceField:(BOOL)farVoiceField
//{
////    _farVoiceField = farVoiceField;
////    _engineParam.farVoiceField = farVoiceField;
//}

- (void)setEngineParam:(USCEngineParams *)engineParam
{
    _engineParam = engineParam;
//    _engineParam.farVoiceField = _farVoiceField;
}

/*!
 *  @brief  engineParam
 *
 *  @return
 *
 *  @since
 */
- (USCEngineParams *)getEngineParams
{
    return _engineParam;
}

- (BOOL)setSampleRate:(int)rate
{
    return [_engineParam setSampleRateInt:rate];
}

- (BOOL)setModelType:(NSString*)engine
{
    return [_engineParam setModelType:engine];
}

- (BOOL)setLanguage:(NSString *)language
{
    if (!language) {
        return NO;
    }
    return  [_engineParam setLanguage:language];
}

- (BOOL)setVoiceField:(NSString *)voiceField
{
    return [_engineParam setVoiceField:voiceField];
}
//- (void)setFarFeild:(BOOL)farFeild
//{
//    if (farFeild) {
//        [_engineParam setVoiceField:1];
//    }
//    else
//    {
//        [_engineParam setVoiceField:0];
//    }
//}
#pragma mark - private method

@end
