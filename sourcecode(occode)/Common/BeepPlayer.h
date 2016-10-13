//
//  MyPlayer.h
//  usc
//
//  Created by Andy on 14-1-9.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "USCLog.h"

@protocol BeepPlayerDelegate <NSObject>

@optional
-(BOOL)isAllowedPlayBeep;

@end

@interface BeepPlayer : NSObject
{
    NSString *filePath;
    NSURL *fileURL;
    AVAudioPlayer *player;
}

@property (nonatomic, weak) id<BeepPlayerDelegate> delegate;

- (void) play;

@end
 