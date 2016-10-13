//
//  USCLog.h
//  usc
//
//  Created by Andy on 14-1-23.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface USCNLULog : NSObject
+ (void) log_v:(NSString *)format, ...;
+ (void) log_d:(NSString *)format, ...;
+ (void) log_i:(NSString *)format, ...;
+ (void) log_w:(NSString *)format, ...;
+ (void) log_e:(NSString *)format, ...;

+ (void) setAllowLogShowWith:(BOOL)allowed;

@end
