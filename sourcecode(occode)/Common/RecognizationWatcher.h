//
//  RecognizationWatcher.h
//  usc
//
//  Created by Andy on 14-1-7.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol RecognizationWatcherDelegate <NSObject>

@optional
- (void)onRecognizationTimeout;

@end

@interface RecognizationWatcher : NSObject
{
    float _timeOutLimit;
    BOOL _timeOutFlag;
    BOOL _isStarted;
    NSTimer *_timer;
}
@property(nonatomic, weak, readwrite)id<RecognizationWatcherDelegate> delegate;
@property(nonatomic, assign, readwrite)float timeOutLimit;
@property(nonatomic, assign, readwrite)BOOL timeOutFlag;
@property(nonatomic, assign, readwrite)BOOL isStarted;      //表示识别监视器开始计时
@property(nonatomic, strong, readwrite)NSTimer *timer;

- (BOOL)isFinished;
- (void)start;
- (void)reset;
- (void)cancel;
- (void)stop;

@end
