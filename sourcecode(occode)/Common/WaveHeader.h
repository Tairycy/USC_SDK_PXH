//
//  WaveHeader.h
//  usc
//
//  Created by hejinlai on 13-11-8.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//

#ifndef __usc__WaveHeader__
#define __usc__WaveHeader__

#include <iostream>

void *createWaveHeader(unsigned long fileLength, short channel, int sampleRate, short bitPerSample);

int pcm_to_wav(const char* pcm_name, const char* wav_name, const int dw_size ,
               const int dw_samples_per_sec, const int ui_bits_per_sample );
#endif /* defined(__usc__WaveHeader__) */
