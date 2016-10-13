//
//  USCSpeechTimer.h
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-6-8.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface USCSpeechTimer : NSObject
/**
 *  开始记录时间
 */
+ (void)begine;
/**
 *  到当前的时间
 *
 *  @return 时间
 */
+ (int)currentDuration;
@end
