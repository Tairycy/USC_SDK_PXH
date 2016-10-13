//
//  USCNLPSetOptions.h
//  USCNLPPraser
//
//  Created by jiqiangsheng on 14-3-13.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "USCNLUParser.h"

@interface USCNLUSetOptions : NSObject
{
    BOOL updateGPS;
}

+(USCNLUSetOptions *)sharedInstance;

- (void) setProperty:(NSString *)property forKey:(int)key;

- (NSString *)propertyForKey:(int)key;

@end
