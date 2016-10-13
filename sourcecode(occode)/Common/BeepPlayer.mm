//
//  MyPlayer.m
//  usc
//
//  Created by Andy on 14-1-9.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "BeepPlayer.h"

@implementation BeepPlayer
    
@synthesize delegate = _delegate;

-(id)init
{
    self = [super init];
    if (self)
    {
        NSError* error;
        filePath = [[NSBundle mainBundle] pathForResource:@"beep" ofType:@"wav"];
        if (filePath)
        {
            fileURL = [NSURL fileURLWithPath:filePath];
            player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:&error];//使用本地URL创建
            player.volume = 1.0f;
        }
        else
        {
//            [USCLog log_i:@"没有找到播放文件", nil];
        }
    }
    return self;
}

- (void) play
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(isAllowedPlayBeep)])
    {
        if ([self.delegate isAllowedPlayBeep])
        {
            // 设置默认从扬声器播放，如果带上耳机，那么从耳机播放
            UInt32 doChangeDefault = 1;
            AVAudioSession *audioSession;
//            [audioSession overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:nil];
            AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, sizeof(doChangeDefault), &doChangeDefault);
            [player prepareToPlay];
            [player play];
        }
    }
}

- (void)dealloc
{
    self.delegate = nil;
    player = nil;
}

@end
