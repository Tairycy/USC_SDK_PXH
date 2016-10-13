//
//  WaveHeader.cpp
//  usc
//
//  Created by hejinlai on 13-11-8.
//  Copyright (c) 2013年 yunzhisheng. All rights reserved.
//

#include <stdlib.h>
#include "WaveHeader.h"

// wav头部结构体
struct wave_header {
    
    char riff[4];
    unsigned long fileLength;
    char wavTag[4];
    char fmt[4];
    unsigned long size;
    unsigned short formatTag;
    unsigned short channel;
    unsigned long sampleRate;
    unsigned long bytePerSec;
    unsigned short blockAlign;
    unsigned short bitPerSample;
    char data[4];
    unsigned long dataSize;
    
};

#define     UNIT        4
#define     WORD        short
#define     DWORD       int
// wav语音文件头部
struct RIFF_HEADER
{
    char szRiffID[4];  // 'R','I','F','F'
    DWORD dwRiffSize;
    char szRiffFormat[4]; // 'W','A','V','E'
};

struct WAVE_FORMAT
{
    WORD wFormatTag;
    WORD wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD wBlockAlign;
    WORD wBitsPerSample;
};

struct FMT_BLOCK
{
    char  szFmtID[4]; // 'f','m','t',' '
    DWORD  dwFmtSize;
    WAVE_FORMAT wavFormat;
};

struct DATA_BLOCK
{
    char szDataID[4]; // 'd','a','t','a'
    DWORD dwDataSize;
};

union DWORD_CHAR
{
    int  nValue;
    char charBuf[4];
};

bool writeFile2Int(FILE *fp,int nWhere,int nValue)
{
    if(fp==NULL)
    {
        return false;
    }
    fseek(fp,nWhere,SEEK_SET);
    DWORD_CHAR dc;
    dc.nValue=nValue;
    fwrite(dc.charBuf,1,4,fp);
    return true;
}


int writeWaveHead(FILE *fp, const int dw_size, const int dw_samples_per_sec,
                  const int ui_bits_per_sample)
{
    if (NULL == fp)
    {
        return -1;
    }
    if (0 > dw_size || 0 > dw_samples_per_sec || 0 > ui_bits_per_sample)
    {
        return -2;
    }
    //写WAV文件头
    RIFF_HEADER rh;
    memset(&rh,0,sizeof(rh));
    strncpy(rh.szRiffFormat,"WAVE",4);
    strncpy(rh.szRiffID,"RIFF",4);

    fwrite(&rh,1,sizeof(rh),fp);

    FMT_BLOCK fb;
    strncpy(fb.szFmtID,"fmt ",4);
    fb.dwFmtSize = dw_size;
    fb.wavFormat.wFormatTag = 0x0001;
    fb.wavFormat.wChannels = 1;
    fb.wavFormat.wBitsPerSample = ui_bits_per_sample;
    fb.wavFormat.dwSamplesPerSec = dw_samples_per_sec;
    fb.wavFormat.wBlockAlign = fb.wavFormat.wChannels*fb.wavFormat.wBitsPerSample/8;   //4;
    fb.wavFormat.dwAvgBytesPerSec = fb.wavFormat.dwSamplesPerSec * fb.wavFormat.wBlockAlign;
    fwrite(&fb,1,sizeof(fb),fp);
    char buf[]={"data0000"};
    fwrite(buf,1,sizeof(buf),fp);
    return 1;
}

int writeWaveBody(FILE *fp, long filelength)
{
    if (NULL == fp)
    {
        return -1;
    }
    if (0 > filelength)
    {
        return -2;
    }
    //更新WAV文件dwRiffSize字段中的值
    int nWhere = 4;
    writeFile2Int(fp,nWhere, filelength - 8);

    //更新WAV文件DataChunk中Size字段的值
    nWhere=sizeof(RIFF_HEADER)+sizeof(FMT_BLOCK)+4;
    writeFile2Int(fp,nWhere,filelength - (sizeof(RIFF_HEADER)+sizeof(FMT_BLOCK)+8) );
    return 1;
}


int pcm_to_wav(const char* pcm_name, const char* wav_name, const int dw_size = 16,
               const int dw_samples_per_sec = 16000, const int ui_bits_per_sample = 16)
{
    if (NULL == pcm_name || NULL == wav_name || 0 > dw_size ||
        0 > dw_samples_per_sec || 0 > ui_bits_per_sample)
    {
        // 错误的参数
        return -1;
    }

    FILE *fpS;
    FILE *fpD;
    
    fpS = fopen(pcm_name, "rb");
    fpD = fopen(wav_name, "wb+");

    if(fpS==NULL || fpD==NULL)
    {
        // 文件打开错误
        return -2;
    }
    fseek(fpS, 0, SEEK_END);
    long filelength = ftell(fpS);

    int ret = 0;
    ret = writeWaveHead(fpD, dw_size, dw_samples_per_sec, ui_bits_per_sample);
    if (ret < 0)
    {
        // 文件头部写入错误
        return -3;
    }

    ret = writeWaveBody(fpD,filelength);
    if (ret < 0)
    {
        // 语音内容拼接错误
        return -4;
    }

    fseek(fpS,44,SEEK_SET);

    char buf[UNIT];
    while(UNIT==fread(buf,1,UNIT,fpS))
    {
        fwrite(buf,1,UNIT,fpD);
    }
    fclose(fpS);
    fclose(fpD);
    
    return 1;
}


//void *createWaveHeader(unsigned long fileLength, short channel, int sampleRate, short bitPerSample)
//{
//    struct wave_header *header = (wave_header *)malloc(sizeof(struct wave_header));
//    
//    if (header == NULL) {
//        return  NULL;
//    }
//    
//    // RIFF
//    header->riff[0] = 'R';
//    header->riff[1] = 'I';
//    header->riff[2] = 'F';
//    header->riff[3] = 'F';
//    
//    // file length
//    header->fileLength = fileLength + (44 - 8);
//    
//    // WAVE
//    header->wavTag[0] = 'W';
//    header->wavTag[1] = 'A';
//    header->wavTag[2] = 'V';
//    header->wavTag[3] = 'E';
//    
//    // fmt
//    header->fmt[0] = 'f';
//    header->fmt[1] = 'm';
//    header->fmt[2] = 't';
//    header->fmt[3] = ' ';
//    
//    header->size = 16;
//    header->formatTag = 1;
//    header->channel = channel;
//    header->sampleRate = sampleRate;
//    header->bitPerSample = bitPerSample;
//    header->blockAlign = (short)(header->channel * header->bitPerSample / 8);
//    header->bytePerSec = header->blockAlign * header->sampleRate;
//    
//    // data
//    header->data[0] = 'd';
//    header->data[1] = 'a';
//    header->data[2] = 't';
//    header->data[3] = 'a';
//    
//    // data size
//    header->dataSize = fileLength;
//    
//    return header;
//}