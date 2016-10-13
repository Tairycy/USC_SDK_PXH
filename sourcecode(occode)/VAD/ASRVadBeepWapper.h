//
//  ASRVadBeepWapper.h
//  usc
//
//  Created by Andy on 14-1-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#ifndef usc_ASRVadBeepWapper_h
#define usc_ASRVadBeepWapper_h
#import <Foundation/Foundation.h>
#import "ASRVadWapper.h"
#import "USCVADParam.h"
#import "USCVADParamValue.h"

class ASRVadWapper;

const long BITNUM_PERSECOND = 16000 * 2;            //每一秒钟录入的字节数
const float DEFAULT_CACHE_TIME = 1.2f;              //默认缓冲时间

@protocol ASRVadBeepWapperDelegate<NSObject>

- (void)onVADTimeOut;
- (void)onUpdateVolume:(int)volume;
- (void)onRecordingData:(NSData *)data isValid:(BOOL)isValid;
- (BOOL)isAllowedPlayBeep;

@end

class ASRVadBeepWapper: public ASRVadWapper
{

public:
    ASRVadBeepWapper();
    ~ASRVadBeepWapper();
    id<ASRVadBeepWapperDelegate> delegate;
    long planCacheSize;     //表示计划时间内缓冲字节数，表示缓冲时间内的字节数
    bool isBegin;       //表示是否开始识别
    NSMutableData *cacheData;          //用来存放缓冲数据

    NSMutableData *tempTestData; // 测试vad 传递的数据

    // vad 参数类
    USCVADParam *vadParams;
    
    void reset();
    void setCacheTime(float cacheTime);
    void write(NSData *data);
    void frontVadCallBack(NSData *data);
    void flush();


    int setOption(int key,NSString *value);

    /*!
     初始化上设置
     :returns:
     */
    void setVADParams();

    /*!
     *  @author usc_zy, 15-01-13 21:01:00
     *
     *  @brief  设置vad参数
     *
     *  @param value value
     *
     *  @return
     *
     *  @since
     */
    int setoption(USCVADParamValueFloat *value);
    int setoption(USCVADParamValueInt *value);
};


#endif
