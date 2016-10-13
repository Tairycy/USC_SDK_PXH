//
//  USCRecognitionParam.m
//  asr&nlu&tts
//
//  Created by iOSDeveloper-zy on 15-5-8.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCRecognitionParam.h"

@implementation USCRecognitionParam

- (void)setVprType:(USCVPRType)vprType
{
    _vprType = vprType;
    switch (vprType) {
        case USCVPRTypeLogin:
            _vprTypeStr = @"lgoin";
            break;
        case USCVPRTypeRegister:
            _vprTypeStr = @"register";
        default:
            break;
    }
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"type=%d,appkey=%@,sampleRate=%d,engine=%@，serviceAddress=%@,port=%@,isPunctuation=%d,userName=%@,vprTypeStr=%@,vprtype=%d",self.recognitionType,self.appkey,self.sampleRate,self.engine,self.serviceAddress,self.port,self.isPunctuation,self.userName,self.vprTypeStr,self.vprType];
}
@end
