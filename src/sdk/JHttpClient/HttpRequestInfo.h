#ifndef __HTTP_REQUEST_INFO_H__
#define __HTTP_REQUEST_INFO_H__

#include <map>
#include <string>
#include "utils.h"

struct HttpRequestInfoS{
	std::map <std::string,std::string> headers;
	JBuffer body;
	std::string url;
        int id;
	HttpRequestInfoS(){
		reset();
	}
	HttpRequestInfoS& operator=(const HttpRequestInfoS& srcHttpReq){
        body = srcHttpReq.body;
        headers = srcHttpReq.headers;
		return *this;
    }
	
	void setHeader(std::string name, std::string value){
		headers.insert(std::pair<std::string,std::string>(name,value));
	}
	
	void appendBody(char* data, int len){
		if(data!=NULL && len>0){
			body.append(data,len);
			char contentLen[100] = {0};
			sprintf(contentLen,"%d", len);
			setHeader("Content-Length",contentLen);
		}else{
			setHeader("Content-Length","0");
		}
	}
	char* getBodyPtr(){
		return body.getBufferPtr();
    }
	int getBodyLen(){
		return body.getBufferRealSize();
	}
	void reset(){
		headers.clear();
		url = "";
		body.resetBuffer();
		id = -1;
	}
};

struct HttpResponseInfoS{
	std::map <std::string,std::string> headers;
	JBuffer body;
	int statusCode;
	int id;
	HttpResponseInfoS(){
		reset();
	}
	
  	HttpResponseInfoS& operator=(const HttpResponseInfoS& srcHttpRes){
		body = srcHttpRes.body;
		statusCode = srcHttpRes.statusCode;
		headers = srcHttpRes.headers;
		return *this;
	}
	
    HttpResponseInfoS(const HttpResponseInfoS& inputInfo){	
		headers = inputInfo.headers;
		statusCode = inputInfo.statusCode;
		body = inputInfo.body; 
    }
	std::string getHeader(std::string name){
		return findKeyFromMap( &headers, name);
	}
    void appendBody(char* data, int len){
		if(data!=NULL && len>0)
			body.append(data,len);
	}
	
	char* getBodyPtr(){
		return body.getBufferPtr();
    }
	
	int getBodyLen(){
		return body.getBufferRealSize();
	}
	
	void reset(){
		headers.clear();
        body.resetBuffer();
		statusCode = 200;
		id = -1;
	}
};

typedef struct HttpRequestInfoS HttpRequestInfo; 
typedef struct HttpResponseInfoS HttpResponseInfo;

#endif
