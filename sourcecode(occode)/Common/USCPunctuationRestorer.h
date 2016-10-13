//
//  USCPunctuationRestorer.h
//  usc
//
//  Created by jiqiangsheng on 14-4-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface USCPunctuationRestorer : NSObject 
@property (nonatomic, retain) NSMutableArray *arrayPd;
@property (nonatomic, copy) NSString *puntuation;

- (NSString *)restorePunctuation:(NSString *)text;

@end
