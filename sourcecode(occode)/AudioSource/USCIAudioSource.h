//
//  UScAudioSource.h
//  asr_nlu_tts
//
//  Created by iOSDeveloper-zy on 15-6-18.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface USCIAudioSource : NSObject

/**
 *  打开录音设备
 *
 *  @return 0 表示成功，否则返回错误码
 */
- (int)openAudioIn;

/**
 *  打开放音设备
 *
 *  @return 0 表示成功，否则返回错误码
 */
- (int)openAudioOut;

/**
 *  读取 size 大小的声音到buffer里
 *
 *  @param buffer
 *  @param size
 *
 *  @return 实际读取的字节数。
 *  注意:当返回值<0表示录音结束.等于0表示当前没有数据可读,sdk内部会循环等待.
 */
- (int)readData:(NSData *)buffer size:(int)size;

/**
 *  写入size大小的buffer到放音设备
 *
 *  @param buffer 数据
 *  @param size   数据大小
 *
 *  @return 实际写入的字节数
 */
- (int)writeData:(NSData *)buffer size:(int)size;

/**
 *  关闭录音设备
 */
- (void)closeAudioIn;

/**
 *  关闭放音设备
 */
- (void)closeAudioOut;

- (int)numOfArray;
@end
