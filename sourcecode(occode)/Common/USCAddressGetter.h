//
//  USCAddressGetter.h
//  usc
//
//  Created by jiqiangsheng on 14-4-11.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCAddress.h"

@interface USCAddressGetter : NSObject
@property (nonatomic, retain)USCAddress *address;

- (USCAddress *)address:(NSString *)language;    //返回一个地址对象，该对象包含服务器地址和端口号

@end
