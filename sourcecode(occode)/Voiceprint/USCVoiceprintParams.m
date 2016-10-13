//
//  USCVoiceprintParams.m
//  asr&nlu&tts
//
//  Created by iOSDeveloper-zy on 15-4-17.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCVoiceprintParams.h"

@interface USCVoiceprintParams()
{
    USCVPRType _type;
    BOOL _isEnable;
    NSString *_username;
}
@end

@implementation USCVoiceprintParams

- (NSString *)getUserName
{
    if (_username) {
        return _username;
    }
    else
        return nil;
}

- (BOOL)setVPRName:(NSString *)userName
{
    if (!userName) {
        return NO;
    }
    if (userName.length ==0) {
        return NO;
    }

    if (userName) {
        _username = userName;
        return YES;
    }
    return NO;
}

- (instancetype)init
{
    if (self = [super init]) {
        _type = USCVPRTypeLogin;
        _isEnable = YES;
        _username = @"";
    }
    return self;
}

- (USCVPRType)getVPRType
{
    return _type;
}

- (void)setVPREnable:(BOOL)enable
{
    _isEnable = enable;
}

- (BOOL)setVPRType:(USCVPRType)type
{
    _type = type;
    return YES;
}

- (BOOL)isVPREnable
{
    return _isEnable;
}

- (NSString *)vprTypeString
{
    switch ([self getVPRType]) {
        case USCVPRTypeLogin:
            return @"login";
            break;
        case USCVPRTypeRegister:
            return @"register";
        default:
             return @"login";
            break;
    }
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"username:%@,type:%@",[self getUserName],[self vprTypeString]];

}
@end
