//
//  NetWatcher.h
//  usc
//
//  Created by jiqiangsheng on 14-3-24.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCLog.h"

@protocol NetWatcherDelegate <NSObject>

@optional
- (void) onSetNetType:(int)sample;          //设置网络类型，实质上是这是8k或16k

@end

const int USCSAMPLE_8K = 8000;
const int USCSAMPLE_16K = 16000;

const float BEYOND_TIME = 4.0f;
const float LESS_TIME = 2.0f;

@interface NetWatcher : NSObject

@property (nonatomic, assign, readwrite) double startTime;
@property (nonatomic, assign, readwrite) double endTime;
@property (nonatomic, assign, readwrite) BOOL isAutoSwitch;
@property (nonatomic, assign, readwrite) int lastSampleRate;
@property (nonatomic, weak, readwrite) id<NetWatcherDelegate> delegate;

- (void) start;          //开始计时，当录音结束以后，开始计时
- (void) stop;           //当返回内容以后，停止计时

@end
