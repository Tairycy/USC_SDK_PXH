//
//  USCNLUParam.h
//  nlu&asr
//
//  Created by yunzhisheng-zy on 14-12-8.
//  Copyright (c) 2014年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

// @class - 语义理解参数类
// @brief - 保存语义理解中的各个参数
@interface USCNLUParam : NSObject

- (BOOL)setNLUServer:(NSString *)server NLUPort:(int)port;

- (NSString *)getNLUServer;

- (NSString *)getCurrentTime;

- (void)setViewId:(int)viewId;

- (NSString *)getViewID;

@end
