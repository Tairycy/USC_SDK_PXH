#ifndef __JHTTPCLIENT_H__
#define __JHTTPCLIENT_H__

#include <string>
#include <map>
#include <queue>
#include <signal.h>

#include "HttpRequestInfo.h"
#include "utils.h"

#ifdef JOE_DEBUG
extern FILE* fp;
#endif

#define	MAX_CONN_TIMEOUT	6

class sdk_Log
{
public:
	sdk_Log() : m_strLog("["), m_bIsOver(false){};
	~sdk_Log(){};

private:
	std::string		m_strLog;
	bool			m_bIsOver;

public:
	void Reset();
	void Write(const char* step, const char* status, int error = 0, int socket_error = 0);
	void Add(const char* key, const char* value);
	void Over();
	const char* GetLog();
};

namespace JDpipeLined{

	const int NORMAL = 0;
	const int NO_WHOLE_RSP = 7;
	const int IS_CANCEL = 8;

	const int OTHER_ERROR = -95;
	const int SELECT_ERROR = -96;
	const int SELECT_TIMEOUT = -97;
	const int TIMEOUT = -98;
	const int INTERRUPT = -99;

	const int SOCKET_ERROR_ = -1;
	const int CONNECT_ERROR_ = -2;
	const int CONNECT_SELECT_ERROR_ = -3;
	const int CONNECT_SELECT_TIMEOUT_ = -4;
	const int SEND_ERROR_ = -5;
	const int SEND_SELECT_ERROR_ = -6;
	const int SEND_SELECT_TIMEOUT_ = -7;
	const int RECV_ERROR_ = -8;
	const int RECV_SELECT_ERROR_ = -9;
	const int RECV_SELECT_TIMEOUT_ = -10;
	const int SET_SOCKET_OPTION_ERROR_ = -11;
	const int SET_FCNTL_ERROR_ = -12;
	const int GET_HOST_NAME_ERROR_ = -13;
// Struct representing an HTTP response status line
struct HttpStatus
{
	std::string version;
	unsigned int code;
	std::string message;
	bool fillLine(const std::string& start);
	void reset(){
		version="";
		code=-1;
		message = "";
	}
};

class HttpRespSM{
public:
    HttpRespSM(){
	reset();
    }
    ~HttpRespSM(){}

    static const int IDLE;
    static const int START_FINDING;
    static const int FOUND_RESP_HEADER;
    static const int FOUND_STATUS_HEAD;
    static const int FOUND_WHOLE_RESP;
    static const int ERROR_HAPPEN;
	static const int CHUNKED_RESP;

    int pushData(char* data, int len);
    int moveOnSM();
    int getRespBlk(HttpResponseInfo& resp){
        if(currentState==FOUND_WHOLE_RESP){
			resp = httpResp;
			return 0;
        }else{
            return -1;
        }
    }
    void resetState(){
		if(currentState==FOUND_WHOLE_RESP){
			int bufferLen = recvDataOrig.getBufferRealSize();
			char* tmp = recvDataOrig.getBufferPtr();
			if(bufferLen>probeOrigDataOffset){
				JBuffer leftBuffer;
				leftBuffer.append(tmp+probeOrigDataOffset,bufferLen-probeOrigDataOffset);
				reset();
				httpResp.reset();
				httpStatus.reset();
				headers.clear();
				recvDataOrig = leftBuffer;
			}else{
				recvDataOrig.resetBuffer();
				headers.clear();
				httpResp.reset();
				content_length = 0;
			}
			probeOrigDataOffset = 0;
			currentState = IDLE;
		}
    }
    int dataInBuffer(){
    	return recvDataOrig.getBufferRealSize();
    }

    int getCurrentState(){
		return currentState;
    }
private:
    std::string readLine(char* buff, int len, int& offset);
    int currentState;
    void reset(){
		currentState = IDLE;
		recvDataOrig.resetBuffer();
		content_length = 0;
		probeOrigDataOffset = 0;
    }
    JBuffer recvDataOrig;
    int content_length;
    int probeOrigDataOffset;
	HttpResponseInfo httpResp;
	HttpStatus httpStatus;
	std::map<std::string, std::string> headers;
	bool chunked;
};


class HttpClient{
public:
    HttpClient(const std::string& host, const std::string& port="80");
    ~HttpClient();
	void SetTimeoutLimit(int t1, int t2, int t3, int t4, int t5);
    int InitAndConnect();
	int SendHttpRequest(HttpRequestInfo* httpRequestInfo, int& httpRespGotten, bool IsSleep = true);
	int EndAndHandle();
	std::queue<HttpResponseInfo>* GetHttpResponse(int& status, sdk_Log& log, int num = 10000, int *retLength = NULL);
	std::queue<HttpResponseInfo>* GetHttpResponseQueue();
	int GetOneHttpResponse(sdk_Log& log, int *retLength = NULL);
	int GetFirstHttpResponse(HttpResponseInfo& resp);
	int GetAllRespBodyLen();
	int GetFd();
	int GetLastError();
	void SetCancel(bool flag);
	bool IsCancel();
private:
	int GetHostByName(const char * host, void * in);
    int ConnectServer(const char* server_ip, const unsigned short server_port);
    void DisconnectServer();
    int GenSendBuffer(HttpRequestInfo* httpRequestInfo, JBuffer& sendBuffer);
    std::string GenHeadersBuffer(HttpRequestInfo* httpRequestInfo);
    int SendData(char* data2send, size_t len);
	int ProbeSocket(int socket, int& leftTime, char* buffer, int len, int& recvLen, int timeout);
	int ReadAndTouchSM(int timeout);
    int send_n ( int socket, char* buffer, int len);
	int GetError(const char* errorMessage);
	int SetSocketTimeout(int s, int timeoutMS);
public:
/*
	static const int NORMAL; 
	static const int NO_WHOLE_RSP;
	static const int IS_CANCEL;

	static const int INTERRUPT;
	static const int TIMEOUT;
	static const int SELECT_ERROR;
	static const int SELECT_TIMEOUT;
	static const int OTHER_ERROR;

	static const int SOCKET_ERROR_;
	static const int CONNECT_ERROR_;
	static const int CONNECT_SELECT_ERROR_;
	static const int CONNECT_SELECT_TIMEOUT_;
	static const int SEND_ERROR_;
	static const int SEND_SELECT_ERROR_;
	static const int SEND_SELECT_TIMEOUT_;
	static const int RECV_ERROR_;
	static const int RECV_SELECT_ERROR_;
	static const int RECV_SELECT_TIMEOUT_;
	static const int SET_SOCKET_OPTION_ERROR_;
	static const int SET_FCNTL_ERROR_;
	static const int GET_HOST_NAME_ERROR_;*/

private:
	int				m_nHandshakeTimeOut;
	int				m_nAckTimeOut;
	int				m_nOneRespTimeOut;
	int				m_nEndTimeOut;
	int				m_nSleepTime;

    std::string host_;
    std::string port_;
    int currentStatus_;
    std::queue<HttpResponseInfo> response_infos_;
    int fd_;
    bool disconnected_;
	JBuffer recvBuffer;
	std::map<std::string, std::string> headers;
	HttpRespSM httpRespSM;
	int respWaitNum;
	int m_nAllRespBodyLen;
	bool m_bIsCancel;
	int m_nLastErrorNo;
};

};


#endif
