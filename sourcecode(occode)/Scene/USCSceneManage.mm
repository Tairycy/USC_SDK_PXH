//
//  USCSceneManage.m
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-10.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import "USCSceneManage.h"
#import "USCScene.h"
#import "USCRecoginizerParam.h"
#import "USCUploadSceneTask.h"
#import "USCErrorCode.h"

/**
 * 全语音场景ID -1为不使用。
 */
static int SCENE_MAX_COUNT = 6;

@interface USCSceneManage()
{
    // 放scene得数据
    NSMutableArray *_mItems;

    // 识别参数对象
    USCRecoginizerParam *_mParam;

}
@end

@implementation USCSceneManage

- (instancetype)init
{
    if (self = [super init]) {
        _mItems = [[NSMutableArray alloc]init];
        _mParam = [[USCRecoginizerParam alloc]init];
    }
    return self;
}

- (int)getSceneCount
{
    return _mItems.count;
}

#pragma mark - private method
- (USCScene *)getItemByIndex:(int)i
{
    if (_mItems.count > i && i > -1) {
        return [_mItems objectAtIndex:i];
    }
    return nil;
}

- (USCScene *)findSceneById:(int)Id
{
    if(Id == SCENE_DISABLED) {
        return nil;
    }

    for (USCScene *scene in _mItems) {
        if (scene.mSceneId == Id) {
            return scene;
        }
    }
    return nil;
}

- (int)getNewSceneId
{
    int newId = SCENE_DISABLED;

    for( int i = 1; i < SCENE_MAX_COUNT;i ++) {
        USCScene *scene = [self findSceneById:i];

        if(scene == nil)
        {
            newId = i;
            break;
        }
    }
    return newId;
}

/**
 *
 * @param name
 * @return
 */
- (USCScene *)createScene:(NSString *)name
{
    if (!name || name.length == 0) {
        return nil;
    }

    USCScene *scene = [self fineScene:name];
    if (scene != nil) {
        return scene;
    }

    int newId = [self getNewSceneId];
    if (newId == SCENE_DISABLED) {
        return nil;
    }

    USCScene *scene2 = [USCScene sceneWithSceneName:name Id:newId];
    [_mItems addObject:scene2];

    return scene2;
}


- (void)remove:(USCScene *)scene
{
    for (USCScene *item in _mItems) {
        if (item.mSceneName == scene.mSceneName) {
            [_mItems removeObject:item];
        }
    }
}

- (void)removeByName:(NSString *)name
{
    USCScene *scene = [self fineScene:name];
    if (scene != nil) {
        [self remove:scene];
    }
}



#pragma mark - public method

- (USCScene *)fineScene:(NSString *)name
{
    for (USCScene *scene  in _mItems) {
        if ([scene.mSceneName isEqualToString:name]) {
            return scene;
        }
    }
    return nil;
}


- (void)setParams:(USCRecoginizerParam *)params
{
    _mParam = params;
}

- (void)uploadSceneData:(NSString *)sceneName sceneData:(NSArray *)sceneData lister:(id)delegate
{
    USCUploadSceneTask *task = [[USCUploadSceneTask alloc]init];
    task.delegate = delegate;
    USCScene *scene = [self fineScene:sceneName];

    if (scene == nil)
    {
        if ([self getSceneCount] >= SCENE_MAX_COUNT) {

            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_SCENE_OUT_MAX_COUNT];
             NSError *error = [NSError errorWithDomain:msg code:UPLOAD_SCENE_OUT_MAX_COUNT userInfo:nil];
            [delegate onUploadSceneDataEnd:task error:error];
            return;
        }

        scene = [self createScene:sceneName];
        if (scene == nil) {
            NSString *msg = [USCErrorCode getErrorMsg:UPLOAD_SCENE_OUT_MAX_COUNT];
            NSError *error = [NSError errorWithDomain:msg code:UPLOAD_SCENE_OUT_MAX_COUNT userInfo:nil];
            [delegate onUploadSceneDataEnd:task error:error];
        }
    }

    [task postData:[_mParam getAppkey] Scene:scene Array:sceneData];
}

@end
