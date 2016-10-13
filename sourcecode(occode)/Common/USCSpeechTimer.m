//
//  USCSpeechTimer.m
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-6-8.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCSpeechTimer.h"

static NSDate *beginDate;
@implementation USCSpeechTimer
+ (void)begine
{
   beginDate = [NSDate date];
}

+ (int)currentDuration
{
    NSTimeInterval duration = [beginDate timeIntervalSinceNow];

    return -(int)duration;
}
@end
