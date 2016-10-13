//
//  USCErrorCode.h
//  usc
//
//  Created by hejinlai on 13-2-22.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>

/* 模型错误 */
static const  int ERROR_GET_MODEL = -91001;
static const int ERROR_LOAD_MODEL = -91002;
static  const int ERROR_SYNRHESISTEXT = -91003;
/* 合成错误 */
static  const int SYSTHESIS_ENGINE_ERROR = -91102;
/* 播放错误 */
static  const int PLAY_ERROR = -91200;
/* 没有授权 */
static const int AUTHORIZE_ERROR = -91300;

static const int SYNTHESIZE_CANCLEED = -91400;

//
static const int SYNTHESIZE_TEXTPUT_ERROR = -91500;


@interface USCTTSErrorCode : NSObject
+ (NSString *)getErrorMsg:(int)code;

@end
