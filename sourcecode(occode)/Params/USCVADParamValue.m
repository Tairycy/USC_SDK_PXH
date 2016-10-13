//
//  USCVADParamValue.m
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-13.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCVADParamValue.h"


@interface USCVADParamValue()
{

}
@end

@implementation USCVADParamValue

- (instancetype)init
{
    if (self = [super init]) {
        _enabled = false;
        _key = 0;
    }
    return self;
}

- (instancetype)initWithKey:(int)key
{
    if (self = [super init]) {
        _key = key;
    }
    return self;
}

- (void)reset
{
    [self disable];
}

- (void)disable
{
    _enabled = false;
}

- (BOOL)isEnabled
{
    return _enabled;
}
@end

/*!
 *  @author usc_zy, 15-01-13 17:01:21
 *
 *  @brief  USCVADParamValueInt 类
 *
 *  @since 1.5
 */
@implementation USCVADParamValueInt

- (instancetype)initWithKey:(int)key
{
    self = [super initWithKey:key] ;
    return self;
}

- (void)setValue:(int)value
{
//    NSLog(@"setvalue int = %d",value);
    _value = value;
    self.enabled = true;
}

- (int)getValue
{
   return _value;
}
@end

/*!
 *  @author usc_zy, 15-01-13 17:01:09
 *
 *  @brief  USCVADParamValueFloat
 *
 *  @since 1.5
 */
@implementation USCVADParamValueFloat

- (instancetype)initWithKey:(int)key
{
    self = [super initWithKey:key] ;
    return self;
}

- (void)setValue:(float)value
{
//    NSLog(@"setvalue float = %f",value);
    _value = value;
    self.enabled = true;
}

- (float)getValue
{
    return _value;
}
@end
