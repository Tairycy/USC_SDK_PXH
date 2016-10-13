//
//  ASRVadWapper.h
//  usc
//
//  Created by hejinlai on 12-11-15.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#ifndef __usc__ASRVadWapper__
#define __usc__ASRVadWapper__
#include "LibVad.h"

class ASRVadWapper {
    
public:

    ASRVadWapper();
    ~ASRVadWapper();
    int init();
    void setVadTimeout(int frontTimeout, int backTimeout);
    /*!
     *  @author usc_zy, 15-01-13 21:01:18
     *
     */
    int setVadParams(int key, const void *value);
    virtual void reset();
    int isVadTimeout(char *pcm, int len, int *volumn);
    int isActive(char *pcm, int len, int *energy);
    int getVolume(char *buffer, int len);
    int checkPitchOffset(char *pcm, int len);

protected:
    usc::LibVAD vad;
};

#endif /* defined(__usc__ASRVadWapper__) */
