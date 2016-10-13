//
//  NLUParsedResult.h
//  Builder
//
//  Created by jiqiangsheng on 14-3-3.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import <Foundation/Foundation.h>

// @class - 语义返回结果类
// @brief - 语义理解返回的结果
@interface USCNLUParsedResult : NSObject

/**
 *  返回的json格式
 */
@property (nonatomic, copy, readwrite) NSString *jsonString;

/**
 *  文本
 */
@property (nonatomic, copy, readwrite) NSString *text;

@end
