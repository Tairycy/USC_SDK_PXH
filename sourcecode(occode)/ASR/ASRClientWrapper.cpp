//
//  ASRClientWrapper.cpp
//  asr
//
//  Created by yunzhisheng on 12-8-15.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#include "ASRClientWrapper.h"
#include <iostream>
#import "errorNo.h"
#import "set_option_key.h"


#define INPUT_BUFFER_LEN	(8000*2)	// 500ms
#define VAD_BUFFER_LEN		(4000*2)	// 250ms

ASRClientWrapper::ASRClientWrapper()
    : count(0)
    , asrService(NULL)
{
    
}

ASRClientWrapper::~ASRClientWrapper()
{
    asrDestroyAsrService(asrService);
	asrService = NULL;
}

int ASRClientWrapper::init(const char* ip, short port)
{ 
	int r = SUCC_CODE::ASRCLIENT_RECOGNIZER_OK;

  	asrService = (asr_client*)asrCreateAsrService(ip,port);
  	if(asrService == NULL){
        r = NO_HANDLE_ERROR;
    }
  	return r;
}


int ASRClientWrapper::setValueInt(int id , int value)
{
	int r = NO_HANDLE_ERROR;
	
	if( asrService != NULL)
	{
		r = asrService->SetValueInt(id, value);
	}
	return r;
    
}

int ASRClientWrapper::setValueString(int id , const char* s)
{
	int r = NO_HANDLE_ERROR;
	
	if( asrService != NULL)
	{
		r = asrService->SetValueString(id, s);
	}
	return r;
}


int ASRClientWrapper::start()
{
	int r = NO_HANDLE_ERROR;
    
  	if( asrService != NULL)
  	{
		count = 0;
  		r = asrService->Start();
  	}
  	return r;
}

int ASRClientWrapper::isActive(char* pcm, int len)
{
    int r = NO_HANDLE_ERROR;
	
  	if( asrService != NULL)
  	{
        // 拷贝录音数据，防止程序崩溃
        char *pcmData = (char *)malloc(len);
        memcpy(pcmData, pcm, len);
        
  		int offset = 0;
//		r = SUCC_CODE::ASRCLIENT_RECOGNIZER_OK;
		do
		{
			int readLen = (len-offset)>VAD_BUFFER_LEN?VAD_BUFFER_LEN:(len-offset);
            r = asrService->IsActive(pcmData + offset, readLen);
			if (r == 0)
			{
               	 	// 释放资源
                		free(pcmData);
				return  VAD_TIMEOUT;
			}
			
			offset += readLen;
		}while(offset < len);
        
        // 释放资源
        free(pcmData);
  	}
  	return r;
}

int ASRClientWrapper::recognize(char* pcm, int len)
{
	int r = NO_HANDLE_ERROR;
	
  	if( asrService != NULL)
  	{
        // 拷贝录音数据，防止程序崩溃
        char *pcmData = (char *)malloc(len);
        memcpy(pcmData, pcm, len);
        
  		int offset = 0;
//		r = SUCC_CODE::ASRCLIENT_RECOGNIZER_OK;
		do
		{
			int readLen = (len-offset)>INPUT_BUFFER_LEN?INPUT_BUFFER_LEN:(len-offset);
            r = asrService->Recognizer(pcmData + offset, readLen);
			if (r<0)
			{
                // 释放资源
                free(pcmData);
				return r;
			}
			
			offset += readLen;
		}while(offset < len);

        r = asrService->QueryResult();
        // 释放资源
        free(pcmData);
  	}
  	return r;
}

int ASRClientWrapper::stop()
{
	int r = NO_HANDLE_ERROR;
  	if( asrService != NULL)
  	{
  		r = asrService->Stop();
  	}
	return r;
}

int ASRClientWrapper::cancel()
{
	int r = NO_HANDLE_ERROR;
  	if( asrService != NULL)
  	{
  		r = asrService->Cancel();
  	}
	return r;
}

const char* ASRClientWrapper::getResult()
{
    const char *r = NULL;
  	if( asrService != NULL)
  	{
  		r = asrService->GetResult();
//        printf("%s-%s-%s",__FILE__,__func__,r);
  	}

//     printf("getResult : %s\n", r);
	return r;
}

const char* ASRClientWrapper::getOption(int id)
{
    const char *r = NULL;
    if (asrService != NULL) {
        r = asrService->GetOptionValue(id);
    }
    return r;
}

int ASRClientWrapper::getLastError()
{
    int r = NO_HANDLE_ERROR;
  	if( asrService != NULL)
  	{
  		r = asrService->GetLastError();
  	}
	return r;
}

const char* ASRClientWrapper::getSessionId()
{
    const char *r = NULL;
  	if( asrService != NULL)
  	{
  		r = asrService->GetOptionValue(USC_GET_SESSION_ID_2);
  	}
	return r;
}


