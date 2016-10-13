//
//  USCNLPSetOptions.m
//  USCNLPPraser
//
//  Created by jiqiangsheng on 14-3-13.
//  Copyright (c) 2014年 jiqiangsheng. All rights reserved.
//

#import "USCNLUSetOptions.h"
#import "USCNLUError.h"

static USCNLUSetOptions *sharedSetOptions = nil;

@implementation USCNLUSetOptions

+(USCNLUSetOptions *)sharedInstance
{
    @synchronized(self)
    {
        if (!sharedSetOptions)
        {
            sharedSetOptions = [[self alloc] init];
        }
    }
    return sharedSetOptions;
}

+(id)allocWithZone:(struct _NSZone *)zone
{
    @synchronized(self)
    {
        if (!sharedSetOptions)
        {
            sharedSetOptions = [super allocWithZone:zone];
        }
    }
    return sharedSetOptions;
}

-(id)init
{
    self = [super init];
    
    if (self)
    {
        updateGPS = YES;
    }
    
    return self;
}

- (void) setProperty:(NSString *)property forKey:(int)key
{
    switch (key)
    {
        case USCNLU_LOCATION_SERVICE:
        {
            updateGPS = [property boolValue];
        }
            break;
            
        default:
            break;
    }
}

- (NSString *)propertyForKey:(int)key
{
    NSString *value = nil;
    switch (key)
    {
        case USCNLU_LOCATION_SERVICE:
        {
            value = [NSString stringWithFormat:@"%d", updateGPS];
        }
            break;
            
        default:
        {
            value = nil;
        }
            break;
    }
    return value;
}

@end
