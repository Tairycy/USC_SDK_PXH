#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <map>
#include <string.h>
#include <string>

const int INIT_SIZE=4096;

class JBuffer{
public:
    JBuffer();
    JBuffer& operator=(const JBuffer& jSrcBuffer) ;
    ~JBuffer();
    void append(char* data, int len);
    void resetBuffer();
    char* getBufferPtr()const;
    int getBufferRealSize()const;
private:
    char* buffer;
    int realSize;
    int capability;
    void reset(){
		if(buffer!=NULL)
			delete[] buffer;
		buffer = new char[INIT_SIZE];
		memset(buffer,0x00,INIT_SIZE);
		capability = INIT_SIZE;
		realSize = 0;
    }
    void enlargeBuffer(int size){
		if(capability!=0){
			char* tmp = new char[size];
			memset(tmp,0x00,size);
			memcpy(tmp,buffer,realSize);
			delete[] buffer;
			capability =size;
			buffer = tmp;
		}else{
			reset();
		}
    }
};

std::string findKeyFromMap(std::map<std::string,std::string> *dstMap,std::string key);
long getCurrentTime();
#endif
