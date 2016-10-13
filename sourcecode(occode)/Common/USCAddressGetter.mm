//
//  USCAddressGetter.m
//  usc
//
//  Created by jiqiangsheng on 14-4-11.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCAddressGetter.h"

NSString *const ENGLISH_ADDRESS = @"v2_eng.hivoice.cn";
NSString *const CANTONESE_ADDRESS = @"v2_cnt.hivoice.cn";
NSString *const CHINESE_ADDRESS = @"v2.hivoice.cn";
const short PORT = (short)80;

@implementation USCAddressGetter

- (USCAddress *)address:(NSString *)language
{
    USCAddress *mAddress = [[USCAddress alloc] init];
    if ([language isEqualToString:@"english"])
    {
        mAddress.server = ENGLISH_ADDRESS;
        mAddress.port = PORT;
    }
    else if ([language isEqualToString:@"cantonese"])
    {
        mAddress.server = CANTONESE_ADDRESS;
        mAddress.port = PORT;
    }
    else
    {
        mAddress.server = CHINESE_ADDRESS;
        mAddress.port = PORT;
    }
    return mAddress;
}
@end
