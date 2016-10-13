//
//  USCAsrResultFormatCantonese.m
//  usc
//
//  Created by jiqiangsheng on 14-4-16.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCAsrResultFormatCantonese.h"

@implementation USCAsrResultFormatCantonese

- (id)init
{
    self = [super init];
    if (self)
    {
        self.patternSentence = @"([[咩嗎]|乜野|乜]$|^(点解))";
    }
    
    return self;
}

@end
