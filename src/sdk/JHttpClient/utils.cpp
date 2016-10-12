#include "utils.h"
#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#include <windows.h>
#endif


JBuffer::JBuffer(){
	buffer = NULL;
	reset();
}

JBuffer& JBuffer::operator=(const JBuffer& jSrcBuffer){

	reset();
    char* tmpBody = jSrcBuffer.getBufferPtr();
	int tmpLen = jSrcBuffer.getBufferRealSize();
	append(tmpBody,tmpLen);
	return *this;
}

JBuffer::~JBuffer(){
	if(buffer!=NULL)
		delete[] buffer;
}

void JBuffer::append(char* data, int len){
	if(len>=capability-realSize){
		//printf("enlargeBuffer capability : %d, realSize : %d, len : %d.\n", capability, realSize, len);
	    enlargeBuffer(len*2+capability);
	}
	//printf("capability : %d, realSize : %d, len : %d.\n", capability, realSize, len);
	memcpy(buffer+realSize,data,len);
	realSize += len;
}

void JBuffer::resetBuffer(){
	reset();
}

char* JBuffer::getBufferPtr()const{
	return buffer;
}

int JBuffer::getBufferRealSize()const{
   	return realSize;
}

std::string findKeyFromMap(std::map<std::string,std::string> *dstMap,std::string key){
    std::map<std::string,std::string>::iterator it;
    it = dstMap->find(key);

    if(it!=dstMap->end())
        return it->second;
    else
		return "";
}
     
long getCurrentTime()    
{
#ifndef WIN32
	struct timeval tv;    
	gettimeofday(&tv,NULL);    
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#else
	return GetTickCount();
#endif
}  
