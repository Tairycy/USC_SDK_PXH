//
//  RecognizationWatcher.m
//  usc
//
//  Created by Andy on 14-1-7.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "RecognizationWatcher.h"

@implementation RecognizationWatcher

- (id)init
{
    self = [super init];
    if (self)
    {
        self.timeOutLimit = 30.0f;
        self.timeOutFlag = NO;
        self.isStarted = NO;
    }
    return self;
}

- (BOOL)isFinished
{
    return self.timeOutFlag;
}

- (void) runTimeOut
{
    self.timeOutFlag = YES;
    if (self.isStarted)
    {
        if (self.delegate && [self.delegate respondsToSelector:@selector(onRecognizationTimeout)])
        {
            [self.delegate onRecognizationTimeout];
        }
    }
}

- (void)start
{
    [self cancel];
    if (self.timeOutLimit < 0)
    {
        return;
    }

    [self performSelectorInBackground:@selector(beginTimer) withObject:nil];
    self.isStarted = YES;
    self.timeOutFlag = NO;
}

-(void)beginTimer
{
    if (!self.timer)
    {
        NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:self.timeOutLimit target:self selector:@selector(runTimeOut) userInfo:nil repeats:NO];
        self.timer = timer;
        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:self.timeOutLimit]];
    }
}

- (void)reset
{
    [self cancel];
}

- (void)cancel
{
    if (self.isStarted)
    {
        self.isStarted = NO;
        if (self.timer)
        {
            [self.timer invalidate];
            self.timer = nil;
        }
    }
    self.timeOutFlag = NO;
}

- (void)stop
{
    [self cancel];
}

- (void)dealloc
{
    self.delegate = nil;
//    [super dealloc];
}

@end
