//
//  USCVADParam.h
//  asr&nlu&tts
//
//  Created by yunzhisheng-zy on 15-1-13.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "USCVADParamValue.h"

/*!
 *  @author usc_zy, 15-01-13 10:01:42
 *
 *  @brief  VAD参数类 vad 参数类是一个单例类，整个程序运行期间只有一个实例。
 *
 *  @since 1.5
 */
@interface USCVADParam : NSObject
{
}

@property (nonatomic,assign) int frontTimeout;
@property (nonatomic,assign) int backTimeout;

@property (nonatomic,strong) USCVADParamValueFloat *MINBACKENG,*MINBACKENGH,*PITCHTH,*LOWHIGHTH,*SINGLEMAX,*NOISE2YTH,*NOISE2YTHVOWEL,*VOICEPROBTH;
@property (nonatomic,strong) USCVADParamValueInt *PITCHSTNUMTH,*PITCHENDNUMTH,*MAXSILLEN,*MINSIGLEN,*USEPEAK,*NOISE2YST,*PITCHLASTTH;

+ (instancetype)shareVADParams;

- (void)showParams;

@end
