//
//  USCVADParamValue.h
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-13.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

@class USCVADParamValue;

@interface USCVADParamValue : NSObject
@property (nonatomic,assign) int key;
@property (nonatomic,assign) BOOL enabled;


- (void)reset;

- (void)disable;

- (BOOL)isEnabled;

@end

@interface USCVADParamValueInt : USCVADParamValue

@property (nonatomic,assign) int value;

- (instancetype)initWithKey:(int)key;

- (void)setValue:(int)value;

- (int)getValue;

@end


@interface USCVADParamValueFloat: USCVADParamValue

@property (nonatomic,assign) float  value;

- (instancetype)initWithKey:(int)key;

- (void)setValue:(float)value;

- (float)getValue;

@end