//
//  ASRVadWapper.cpp
//  usc
//
//  Created by hejinlai on 12-11-15.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#include "ASRVadWapper.h"
#include "string.h"
#include "mathops.h"
//#import "asrclient.h"
#import "errorNo.h"

#define INPUT_BUFFER_LEN	(8000*2)	// 500ms
#define VAD_BUFFER_LEN		(320)	// 250ms

ASRVadWapper::ASRVadWapper()
{

}

int ASRVadWapper::init()
{    
    return vad.init();
}

void ASRVadWapper::setVadTimeout(int frontTimeout, int backTimeout)
{
    vad.setVadSilTime(frontTimeout, backTimeout);
}

void ASRVadWapper::reset()
{
    vad.reset();
}

int ASRVadWapper::isActive(char *pcm, int len, int *energy)
{
    int status = vad.process((short*)pcm, len/(sizeof(short)), energy);

    //0 holding 1 back 2 max 3 front
    if(status == 1 || status == 2){
        return 0;
    }
    return 1;
}

int ASRVadWapper::isVadTimeout(char *pcm, int len, int *volumn)
{
    // 拷贝录音数据，防止程序崩溃
    char *pcmData = new char[len];
    memcpy(pcmData, pcm, len);
    
    int offset = 0;
    do
    {
        int readLen = (len-offset)>VAD_BUFFER_LEN?VAD_BUFFER_LEN:(len-offset);
        // printf("readLen = %d",readLen);
        int r = isActive(pcmData + offset, readLen, volumn);
        if (r == 0)
        {
            // 释放资源
            delete [] pcmData;
            return  VAD_TIMEOUT;
        }
			
			offset += readLen;
    }
    while(offset < len);
        
    // 释放资源
    delete [] pcmData;
  	return 0;
}

/*
 音量大小，范围：0~100
 */
int ASRVadWapper::getVolume(char *buffer, int len)
{
    double volume = 0.0;
    for (int i = 0; i < len; i += 2) {
        int v1 = buffer[i] & 0xFF;
        int v2 = buffer[i + 1] & 0xFF;
        int temp = v1 + (v2 << 8);
        if (temp >= 0x8000) {
            temp = 0xffff - temp;
        }
        volume += abs(temp);
    }
    volume = volume * 2 / len;
    double v = (log10(1 + volume) * 10 - 20.0) * 5;
    int result = (int)v;
    result = result < 0 ? 0:result;
    result = result > 100 ? 100:result;
    return result;
}

int ASRVadWapper::checkPitchOffset(char *pcm, int len)
{
    int position = vad.checkPitchOffset((short *)pcm, len/(sizeof(short)));
    return position * 2;
}

/*!
 *  @author usc_zy, 15-01-13 21:01:00
 *
 *  @brief  设置vad 参数
 *
 *  @param key   key
 *  @param value value
 *
 *  @return 结果
 *
 *  @since 1.5
 */
int ASRVadWapper::setVadParams(int key, const void *value)
{
    return vad.setparas(key, value);
}

ASRVadWapper::~ASRVadWapper()
{

}