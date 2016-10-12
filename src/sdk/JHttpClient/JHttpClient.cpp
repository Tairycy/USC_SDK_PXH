#include "JHttpClient.h"
#include "log.h"
#include "http_def.h"
#include "getTime.h"
#include "function.h"

#include <string.h>
#include <cctype>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>

#if (defined LINUX) || (defined iOS)
#if (!defined ANDROID_ENV) && (!defined iOS)
#include <error.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h> 
#include <net/if.h>
typedef int SOCKET;
#elif defined (WIN32)
#include <Winsock2.h>
#include <ws2tcpip.h>
#endif

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#ifdef	OS_HAS_POLL
#include <poll.h>
#endif

#ifdef JOE_DEBUG
FILE* fp = NULL;
#endif

namespace JDpipeLined{


	const int HttpRespSM::IDLE = 0;
	const int HttpRespSM::START_FINDING = 1;
	const int HttpRespSM::FOUND_STATUS_HEAD = 2;
	const int HttpRespSM::FOUND_RESP_HEADER = 3;
	const int HttpRespSM::FOUND_WHOLE_RESP = 4;
	const int HttpRespSM::ERROR_HAPPEN = 5;
	const int HttpRespSM::CHUNKED_RESP = 6;

	/**********************************string process*********************************/

	bool HttpStatus::fillLine(const std::string& line)
	{
		// not all error code from zsf have http error information, so, comment them by lqy.
		if (std::count(line.begin(), line.end(), ' ') >= 2
			&& line[0] != ' '/*&& line[line.length() - 1] != ' '*/) {
			size_t pos1, pos2;
			pos1 = line.find(' ');
			version = line.substr(0, pos1);
			++pos1;
			if (isspace(line[pos1])) {
				char errorInfo[1000];
				sprintf(errorInfo, "Bad status line (double space) \"%s\"", line.c_str());
				LOGMYE("J-FILLLINE", errorInfo, 1);
				return false;
			}
			pos2 = line.find(' ', pos1);
			code = atoi(line.substr(pos1, pos2 - pos1).c_str());
			if (code < 100 || code > 699) {
				char errorInfo[1000];
				sprintf(errorInfo, "Bad status line (invalid status code) \"%s\"", line.c_str());
				LOGMYE("J-FILLLINE", errorInfo, 1);
				return false;
			}
			++pos2;
			if (isspace(line[pos2])) {
				char errorInfo[1000];
				sprintf(errorInfo, "Bad status line (double space) \"%s\"", line.c_str());
				LOGMYE("J-FILLLINE", errorInfo, 1);
				return false;
			}
			message = line.substr(pos2);
			return true;
		}
		else {
			char errorInfo[1000];
			sprintf(errorInfo, "Bad status line (not enough spaces) \"%s\"", line.c_str());
			LOGMYE("J-FILLLINE", errorInfo, 1);
			return false;
		}
	}

	std::string HttpRespSM::readLine(char* buffer, int len, int& offset){

		if (buffer == NULL || len <= 0){
			offset = 0;
			return "";
		}
		//find "\r\n" and return the whole line
		char* lineEnd = strstr(buffer, "\r\n");
		int lineContent = lineEnd - buffer;
		std::string line = "";
		if (lineEnd != NULL && lineContent < len){
			if (lineContent > 0){
				line.assign(buffer, lineContent);
			}
			else{
				line = "";
			}
			offset = lineContent + 2;
		}
		else{
			offset = 0;
		}

		return line;
	}

	int HttpRespSM::pushData(char* data, int len){

		if (data != NULL&&len > 0){
			recvDataOrig.append(data, len);
		}

		return 0;
	}

	int HttpRespSM::moveOnSM(){
		char* probeOrigDataPtrStart = recvDataOrig.getBufferPtr();
		char* probeOrigDataPtr = probeOrigDataPtrStart;
		//probeOrigDataPtr += probeOrigDataOffset;
		int data2ParseLen = recvDataOrig.getBufferRealSize() - probeOrigDataOffset;
		int index = 0;
		int leftLen = data2ParseLen;
		while (index < data2ParseLen){
			probeOrigDataPtr = probeOrigDataPtrStart;
			probeOrigDataPtr += probeOrigDataOffset;
			int leftLenOnce = data2ParseLen - index;
			if (currentState == IDLE || currentState == START_FINDING){
				int lineOffset = 0;
				std::string line = readLine(probeOrigDataPtr, leftLenOnce, lineOffset);
				if (line != ""&&lineOffset > 0){
					if (leftLen >= lineOffset){
						bool stat = httpStatus.fillLine(line);
						if (!stat){
							currentState = ERROR_HAPPEN;
							break;
						}
						index += lineOffset;
						leftLen -= lineOffset;
						currentState = FOUND_STATUS_HEAD;
						probeOrigDataOffset += lineOffset;
					}
					else{
						LOGMYE("J-MOVEONSM", "something wrong.", 1);
					}
				}
				else{
					index += leftLen;
					leftLen = 0;
					currentState = START_FINDING;
				}
			}
			else if (currentState == FOUND_STATUS_HEAD){

				bool headers_finished = false;
				const char cl_name[] = "content-length:";
				const size_t cl_name_length = strlen(cl_name);
				const char te_name[] = "transfer-encoding:";
				const size_t te_name_length = strlen(te_name);
				const char te_value[] = "chunked";
				const char ct_name[] = "Connection:";
				const size_t ct_name_length = strlen(ct_name);
				const char ct_value[] = "close";
				size_t pos;
				std::string previous_name;

				int lineOffset = 0;
				std::string header = readLine(probeOrigDataPtr, leftLenOnce, lineOffset);

				if (header != ""&&lineOffset > 0){
					if (leftLen >= lineOffset){
						//get header name and value
						if (!isspace(header[0])) {
							// Header names are case-insensitive
							std::string lc_header(header);
							std::transform(lc_header.begin(), lc_header.end(), lc_header.begin(), tolower);
							if (lc_header.substr(0, cl_name_length) == cl_name) {
								// Content length
								std::string::iterator cl_it =
									std::find_if(lc_header.begin() + cl_name_length, lc_header.end(), isdigit);
								if (cl_it != lc_header.end()) {
									content_length = atoi(lc_header.c_str() + (cl_it - lc_header.begin()));
								}
							}
							else if (lc_header.substr(0, te_name_length) == te_name) {
								// Transfer coding
								std::string::iterator te_it =
									std::find_if(lc_header.begin() + te_name_length, lc_header.end(), isalpha);
								if (te_it != lc_header.end()) {
									chunked = lc_header.substr(te_it - lc_header.begin(), strlen(te_value)) == te_value;
									if (chunked){
										//we don't support chunked method yet
										LOGMYE("J-MOVEONSM", "GET RESPONSE CHUNKED!", 1);
										content_length = 0;
									}
								}
							}
							else if (lc_header.substr(0, ct_name_length) == ct_name) {
								// Transfer coding
								std::string::iterator te_it =
									std::find_if(lc_header.begin() + ct_name_length, lc_header.end(), isalpha);
								if (te_it != lc_header.end()) {
									chunked = lc_header.substr(te_it - lc_header.begin(), strlen(ct_value)) == ct_value;
									if (chunked){
										//we don't support chunked method yet
										LOGMYE("J-MOVEONSM", "CLOSE CONNECTION!", 0);
										content_length = 0;
									}
								}
							}
							// Split the header into name and value
							pos = header.find(":");
							if (pos != header.npos) {
								std::string name = header.substr(0, pos);
								std::string::iterator value_it =
									std::find_if(header.begin() + pos + 1, header.end(), std::not1(std::ptr_fun(isspace)));
								std::string value;
								if (value_it != header.end()) {
									value = header.substr(value_it - header.begin());
								}
								headers.insert(std::pair<std::string, std::string>(name, value));
								previous_name = name;
							}
							else {
								char errorInfo[1000];
								sprintf(errorInfo, "Couldn't find colon in header \"%s\"", header.c_str());
								LOGMYE("J-MOVEONSM", errorInfo, 1);
								currentState = ERROR_HAPPEN;
								break;
							}
						}
						else {
							// Another line of a multi-line header
							if (previous_name.length()) {
								std::map<std::string, std::string>::iterator it = headers.find(previous_name);
								if (it != headers.end()) {
									it->second += header.substr(1);
								}
								else {
									char errorInfo[1000];
									sprintf(errorInfo, "Couldn't find header name \"%s\"", previous_name.c_str());
									LOGMYE("J-MOVEONSM", errorInfo, 1);
									currentState = ERROR_HAPPEN;
									break;
								}
							}
							else {
								char errorInfo[1000];
								sprintf(errorInfo, "Space at beginning but no previous line in header \"%s\"", header.c_str());
								LOGMYE("J-MOVEONSM", errorInfo, 1);
								currentState = ERROR_HAPPEN;
								break;
							}
						}

						index += lineOffset;
						leftLen -= lineOffset;
						currentState = FOUND_STATUS_HEAD;
						probeOrigDataOffset += lineOffset;
					}
					else{
						//something wrong, has to be fix!!!
						LOGMYE("J-MOVEONSM", "something wrong, has to be fix!!!", 1);
					}
				}
				else if (lineOffset > 0){
					index += lineOffset;
					leftLen -= lineOffset;
					currentState = FOUND_RESP_HEADER;
					probeOrigDataOffset += lineOffset;
					//if there is no content, we can move on the state
					if (content_length == 0){
						goto SET_DATA;
					}
				}
				else{
					index += leftLen;
					leftLen = 0;
					currentState = FOUND_STATUS_HEAD;
				}

			}
			else if (currentState == FOUND_RESP_HEADER){
				if (leftLen < content_length){
					index += leftLen;
					leftLen = 0;
				}
				else{
					//set status, headers and body
				SET_DATA:			httpResp.statusCode = httpStatus.code;
					// Headers
					for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {

						httpResp.headers.insert(std::pair <std::string, std::string>(it->first, it->second));
					}
					//body
					httpResp.body.resetBuffer();
					httpResp.body.append(probeOrigDataPtr, content_length);
					index += content_length;
					leftLen -= content_length;
					currentState = FOUND_WHOLE_RESP;
					probeOrigDataOffset += content_length;

					break;
				}
			}
			else if (currentState == FOUND_WHOLE_RESP || currentState == ERROR_HAPPEN){
				currentState = ERROR_HAPPEN;
				LOGMYE("J-MOVEONSM", "ERROR_HAPPEN", 1);
				break;
			}
		}

		return currentState;
	}

	std::string HttpClient::GenHeadersBuffer(HttpRequestInfo* httpRequestInfo){
		std::string line = "";
		std::map <std::string, std::string> headers = httpRequestInfo->headers;
		std::map <std::string, std::string>::iterator header_iter;
		for (header_iter = headers.begin(); header_iter != headers.end(); header_iter++){
			std::string name = header_iter->first;
			std::string value = header_iter->second;
			line += name + ":" + value + "\n";
		}
		line += "\n";
		return line;
	}
	
	int HttpClient::GenSendBuffer(HttpRequestInfo* httpRequestInfo, JBuffer& sendBuffer){
		//gen command line
		std::string method = "POST";
		std::string uri = httpRequestInfo->url;
		std::string line = method + " " + uri + " HTTP/1.1";
		line += "\n";
		//gen headers
		line += "Host: " + host_;
		if (port_.c_str() != "80"){
			line += ":" + port_;
		}
		line += "\n";
		line += GenHeadersBuffer(httpRequestInfo);

		sendBuffer.append((char*)line.c_str(), line.length());
		//add body
		char* body = httpRequestInfo->getBodyPtr();
		int len = httpRequestInfo->getBodyLen();
		sendBuffer.append(body, len);
		return 0;
	}



	/**********************************init function*********************************/

	HttpClient::HttpClient(const std::string& host, const std::string& port)
		:host_(host), port_(port), disconnected_(true), m_nAllRespBodyLen(0), m_nLastErrorNo(0)
	{
		m_bIsCancel = false;
		char log[128] = { 0 };
		sprintf(log, "create HttpClient : %x, %s, %s", this, host.c_str(), port.c_str());
		LOGMYI("JHTTP", log, 2);
		respWaitNum = 0;
		fd_ = -1;
		m_nHandshakeTimeOut = N_HANDSHAKE_TO;
		m_nAckTimeOut = N_ACK_TO;
		m_nOneRespTimeOut = N_ONERESP_TO;
		m_nEndTimeOut = N_LAST_TO;
		m_nSleepTime = N_SLEEP_TIME;
	}

	HttpClient::~HttpClient()
	{
		DisconnectServer();
		char log[128] = { 0 };
		sprintf(log, "release ~HttpClient : %x", this);
		LOGMYI("JHTTP", log, 2);
	}

	void HttpClient::SetTimeoutLimit(int t1, int t2, int t3, int t4, int t5)
	{
		m_nHandshakeTimeOut = t1;
		m_nAckTimeOut = t2;
		m_nOneRespTimeOut = t3;
		m_nEndTimeOut = t4;
		m_nSleepTime = t5;
	}

	int HttpClient::ConnectServer(const char* server_ip, const unsigned short server_port)
	{
		m_nLastErrorNo = 0;
		char err_msg[128] = { 0 };
		DisconnectServer();

		struct sockaddr_in asr_server;
		fd_ = socket(AF_INET, SOCK_STREAM, 0);
		if (fd_ < 0) {
			GetError("socket error");
			return SOCKET_ERROR_;
		}
		int set = 1;
#ifdef iOS
		setsockopt(fd_, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
		memset(&asr_server, 0, sizeof(asr_server));
		if (GetHostByName(server_ip, &asr_server) < 0)
			return GET_HOST_NAME_ERROR_;
		asr_server.sin_family = AF_INET;
		asr_server.sin_port = htons(server_port);

#ifdef LINUX
		int flag = fcntl(fd_, F_GETFL, 0);
		if (fcntl(fd_, F_SETFL, flag|O_NDELAY) < 0) {
			GetError("fcntl error");
			DisconnectServer();
			return SET_FCNTL_ERROR_;
		}
#endif
		if (setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, (const char *)&set, sizeof(set)) != 0)
		{
			GetError("setNoDelay error");
			return SET_SOCKET_OPTION_ERROR_;
		}

		int rc = connect(fd_, (const struct sockaddr*)&asr_server, sizeof(struct sockaddr_in));
		if(rc<0 && (errno!=EAGAIN && errno!=EINPROGRESS))
		{
			GetError("connect error");
			DisconnectServer();
			return CONNECT_ERROR_;
		}
		sprintf(err_msg, "connect socket success, fd = %d, httpclient addr is %x", fd_, this);
		LOGMYI("JHTTP", err_msg, 2);
		fd_set wfds;
		struct timeval tv;

		FD_ZERO(&wfds);
		FD_SET(fd_, &wfds);
		tv.tv_sec = m_nHandshakeTimeOut / 1000;
		tv.tv_usec = (m_nHandshakeTimeOut % 1000) * 1000;
		int ret;
		while (true)
		{
			if (IsCancel())
				return IS_CANCEL;
			ret = select(fd_ + 1, NULL, &wfds, NULL, &tv);
			if (ret >=  0)
				break;
			if (errno != 115)
			{
				GetError("select error after connect");
				return CONNECT_SELECT_ERROR_;
			}
		}
		// three times handshake, no seccess! 6s
		if (tv.tv_sec == 0 && tv.tv_usec == 0)
			return CONNECT_SELECT_TIMEOUT_;
#ifdef	LINUX

		if (fcntl(fd_, F_SETFL, flag) < 0) {

			GetError("fcntl error");
			DisconnectServer();
			return SET_FCNTL_ERROR_;
		}
#endif

		return NORMAL;
	}

	void HttpClient::DisconnectServer(){
		if (fd_ > 0)	{
#ifdef	WIN32
			closesocket(fd_);
#else
			close(fd_);
#endif
		}
		fd_ = 0;
	}

	int HttpClient::InitAndConnect()
	{
		m_nLastErrorNo = 0;
		int stat = ConnectServer(host_.c_str(), atoi(port_.c_str()));
		if (stat == NORMAL){
			disconnected_ = false;
		}
		return stat;
	}

	int HttpClient::EndAndHandle(){
		DisconnectServer();
		return 0;
	}

	int HttpClient::GetHostByName(const char * host, void * in)
	{
		int ret = -1;
#ifdef WIN32
		struct addrinfo *answer, *curr;
		int code = getaddrinfo(host, NULL, NULL, &answer);
		if (code == 0)
		{
			for (curr = answer; curr != NULL; curr = curr->ai_next)
			{
				memcpy(&((struct sockaddr_in *)in)->sin_addr.s_addr,
					&((struct sockaddr_in *)(curr->ai_addr))->sin_addr,
					sizeof(unsigned long));
				freeaddrinfo(answer);
				ret = 0;
				break;
			}
		}
#else
		/*
		*	Gethostbyname is a multi-thread unsafe function, so we need to judge it. add by lqy...
		*	Forgive me , there is no better function.
		*	If we used getaddrinfo, we have to 2~3 seconds clog.
		*/
		struct hostent *answer;
		if ((answer = gethostbyname(host)) != NULL &&
			answer != NULL && answer->h_addr_list[0] != NULL &&
			memcpy(&((struct sockaddr_in *)in)->sin_addr, answer->h_addr_list[0], sizeof(unsigned long)) != NULL)
		{
			ret = 0;
		}
#endif
		if (ret < 0)
			GetError("gethostbyname error!");
		return ret;
	}

	int HttpClient::SetSocketTimeout(int s, int timeoutMS)
	{
		int ret = 0;
#ifdef WIN32
		int timeout = timeoutMS;
		ret = setsockopt((SOCKET)s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
		struct timeval timeout = { timeoutMS * 1000, 0 };
		ret = setsockopt((SOCKET)s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
#endif
		if (ret < 0)
			GetError("set socket timeout error!");
		return ret;
	}


	/*******************************************************************/
	
	int HttpClient::send_n(int socket, char* buffer, int len)
	{
		char log_msg[128] = { 0 };
		sprintf(log_msg, "send_n, fd = %d, buffer len = %d, timeout = %d", socket, len, m_nAckTimeOut);
		LOGMYI("JHTTP", log_msg, 2);
		char err_msg[128] = { 0 };
		int rt = 0, sent = 0;
		int timeout = m_nAckTimeOut;
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		do
		{
			int retval;	
			fd_set wfds;

			FD_ZERO(&wfds);
			FD_SET(socket, &wfds);

			retval = select(socket + 1, NULL, &wfds, NULL, (timeout == -1) ? NULL : &tv);
			if (retval <= 0) {
				
				if (errno == EINTR || errno == EINPROGRESS)
					continue;
				else if (tv.tv_sec == 0 && tv.tv_usec == 0)
				{
					GetError("select timeout after send");
					return SEND_SELECT_TIMEOUT_;
				}
				else
				{
					GetError("select error after send");
					return SEND_SELECT_ERROR_;
				}
			}

			SetSocketTimeout(socket, m_nAckTimeOut);
			sprintf(log_msg, "send, fd = %d, buffer len = %d", socket, len - sent);
			LOGMYI("JHTTP", log_msg, 3);
			rt = send(socket, buffer + sent, len - sent, 0);

			//printf("real sent data len %d, input data len is %d\n",rt, len);
			if (rt > 0)
				sent += rt;
#ifdef LINUX
			else if ( rt < 0 && errno == EINTR )
				continue;
#endif
			else{
				sprintf(err_msg, "send_n send error, fd = %d, errorno = %d", socket, errno);
				LOGMYE("JHTTP", err_msg, 1);
				return SEND_ERROR_;
			}
		} while (sent < len);

		return sent;
	}

	int HttpClient::SendData(char* data2send, size_t len)
	{
		if (len == 0)
			return NORMAL;
		size_t bytes_sent = 0;
		size_t data_once_send_len = len;
		int stat = NORMAL;
		stat = send_n(fd_, data2send, data_once_send_len);
		if (stat >= 0)
			stat = NORMAL;
		return stat;
	}

	int HttpClient::ProbeSocket(int s, int& leftTime, char* buffer, int len, int& recvLen, int timeout_)
	{
		int timeout = timeout_;
		char err_msg[128] = { 0 };
		sprintf(err_msg, "ProbeSocket, fd = %d, len = %d, timeout = %d.", fd_, len, timeout);
		LOGMYI("JHTTP", err_msg, 2);
		int retval;

		fd_set rfds;
		struct timeval tv;
		FD_ZERO(&rfds);
		FD_SET(s, &rfds);
		// Wait up to five seconds. 
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		retval = select(s + 1, &rfds, NULL, NULL, (timeout == -1) ? NULL : &tv);
		leftTime = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
		//printf("retval : %d\tleftTime : %d.\n", retval, leftTime);
		if ((retval == 0) || (retval < 0 && (errno == EINTR || errno == EINPROGRESS))) {
			leftTime = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
			if (leftTime == 0)
				return SELECT_TIMEOUT;
			return INTERRUPT;
		}
		else if (retval < 0)
		{
			GetError("select error ProbeSocket");
			return SELECT_ERROR;
		}
		int start_time = getCurrentTimeM();
		int rt = recv(s, buffer, len, 0);
		sprintf(err_msg, "recv fd = %d, len = %d", fd_, len);
		LOGMYI("JHTTP", err_msg, 2);
		if (rt > 0)
			recvLen = rt;
		else if (rt == 0 || errno == EINPROGRESS)
			return INTERRUPT;
		else{
			GetError("recv");
			return RECV_ERROR_;
		}
		return NORMAL;
	}

	int HttpClient::ReadAndTouchSM(int timeout){
		//try to read data
		int bufferLen = 500;
		char db[500] = { 0 };
		char* buffer = NULL;
		int recvLen = 0;
		int leftTime = 0;
		char err_msg[128] = { 0 };
		sprintf(err_msg, "ReadAndTouchSM, fd_ = %d", fd_);
		LOGMYI("JHTTP", err_msg, 3);
		int sm_stat;
		if (httpRespSM.getCurrentState() != HttpRespSM::IDLE || httpRespSM.dataInBuffer() == 0)
		{
			int stat = ProbeSocket(fd_, leftTime, db, bufferLen, recvLen, timeout);
			//printf("stat : %d\n", stat);
			if (stat == NORMAL && recvLen > 0){
				buffer = db;
				//printf("TTTTTTTTTTTTTTT\t\t\t\t\trecvLen : %d\n", recvLen);
			}
			else if (stat == INTERRUPT){
				buffer = NULL;
				stat = NORMAL;
			}
			else{
				sm_stat = stat;
				buffer = NULL;
				//stat = SOCKET_ERROR_;
				char err_msg[128] = { 0 };
				sprintf(err_msg, "ReadAndTouchSM, fd_ = %d, error! stat = %d.", fd_, stat);
				LOGMYE("JHTTP", err_msg, 1);
				return sm_stat;
			}
			if (buffer != NULL)
				httpRespSM.pushData(buffer, recvLen);
		}
		sm_stat = httpRespSM.moveOnSM();
		return sm_stat;
	}

	int HttpClient::SendHttpRequest(HttpRequestInfo * httpRequestInfo, int& httpRespGotten, bool IsSleep)
	{
		LOGMYI("JHTTP", "SendHttpRequest", 2);
		JBuffer sendBuffer;
		GenSendBuffer(httpRequestInfo, sendBuffer);
		int stat = SendData(sendBuffer.getBufferPtr(), sendBuffer.getBufferRealSize());
		if (stat != NORMAL){
			return stat;
		}
		respWaitNum++;
		if (IsSleep)
			sleepMS(m_nSleepTime);
		if (IsCancel())
			return IS_CANCEL;
		int sm_stat = ReadAndTouchSM(N_BEFORE_SEND_TIME);
		httpRespGotten = 0;
		if (sm_stat == HttpRespSM::FOUND_WHOLE_RESP){
			LOGMYI("JHTTP", "get one resp", 2);
			respWaitNum--;
			HttpResponseInfo resp;
			httpRespSM.getRespBlk(resp);
			response_infos_.push(resp);
			m_nAllRespBodyLen += resp.getBodyLen();
			httpRespSM.resetState();
			httpRespGotten = 1;
		}
		else if (sm_stat == SELECT_ERROR){
			stat = SEND_SELECT_ERROR_;
		}
		else{
			stat = NORMAL;
		}
		return stat;
	}

	int HttpClient::GetFirstHttpResponse(HttpResponseInfo& resp)
	{
		if (response_infos_.size() > 0){
			resp = response_infos_.front();
			m_nAllRespBodyLen -= response_infos_.front().getBodyLen();
			response_infos_.pop();
			return 0;
		}
		else{
			return -1;
		}
	}

	std::queue<HttpResponseInfo>* HttpClient::GetHttpResponse(int& status, sdk_Log& log, int num, int *retLength)
	{
		int sm_stat, stat = 0;
		if (retLength != NULL)
			*retLength = m_nAllRespBodyLen;
		int start_time = getCurrentTimeS(), this_time;
		int times = 1;
		if (respWaitNum > 1)
			times = respWaitNum / 4 + 1;
		times = times < num ? times : num;
		int timeout = 0;
		timeout = (m_nOneRespTimeOut*times < m_nEndTimeOut) ? m_nOneRespTimeOut*times : m_nEndTimeOut;
		//printf("timeout : %d, respWaitNum : %d, num : %d.\n", timeout, respWaitNum, num);
		while (respWaitNum > 0 && num)
		{
			LOGMYI("JHTTP", "To get one resp!", 3);
			if (IsCancel()) {
				stat = IS_CANCEL;
				break;
			}
			this_time = getCurrentTimeS();
			if (this_time - start_time > timeout / 1000)
			{
				LOGMYE("JHTTP", "GetHttpResponse Timeout!", 0);
				stat = TIMEOUT;
				break;
			}
			sm_stat = ReadAndTouchSM(m_nOneRespTimeOut);
			if (sm_stat == HttpRespSM::FOUND_WHOLE_RESP)
			{
				//log.Write("GetHttpResponse", "end", sm_stat, m_nLastErrorNo);
				LOGMYI("JHTTP", "GetHttpResponse, get one response!", 2);
				respWaitNum--;
				num--;
				HttpResponseInfo resp;
				httpRespSM.getRespBlk(resp);
				response_infos_.push(resp);
				if (retLength != NULL)
				{
					*retLength += resp.getBodyLen();
				}
				m_nAllRespBodyLen += resp.getBodyLen();
				httpRespSM.resetState();
				// 如果出错，就停掉
				if (resp.statusCode != 200)
					break;
			}
			else if (sm_stat < 0)
			{
				LOGMYE("JHTTP", "select timeout before recv", 0);
				if (sm_stat == SELECT_ERROR)
					stat = RECV_SELECT_ERROR_;
				else if (sm_stat == SELECT_TIMEOUT)
					stat = RECV_SELECT_TIMEOUT_;
				else
					stat = sm_stat;
				break;
			}
			else if (sm_stat == HttpRespSM::ERROR_HAPPEN)
			{
				LOGMYE("JHTTP", "Get one resp, client error -2", 1);
				stat = OTHER_ERROR;
				break;
			}
			else{
				stat = NORMAL;
			}
		}
		m_nAllRespBodyLen = 0;
		// if respWaitNum > 0 , illustrate there is error in process.
		if (respWaitNum >= 0 && stat >= 0 && stat != IS_CANCEL) {
			//status = NORMAL;
			status = stat;
			return &response_infos_;
		}
		else {
			status = stat;
			return NULL;
		}
	}

	std::queue<HttpResponseInfo>* HttpClient::GetHttpResponseQueue()
	{
		return &response_infos_;
	}

	/***********************************get sth function********************************/

	int HttpClient::GetAllRespBodyLen()
	{
		return m_nAllRespBodyLen;
	}

	int HttpClient::GetFd()
	{
		return fd_;
	}

	int HttpClient::GetLastError()
	{
		return m_nLastErrorNo;
	}

	void HttpClient::SetCancel(bool flag)
	{
		m_bIsCancel = flag;
	}

	bool HttpClient::IsCancel()
	{
		return m_bIsCancel;
	}

	int HttpClient::GetError(const char* errorMessage)
	{
		char err_msg[256] = { 0 };
		sprintf(err_msg, "%s\tfd = %d\tthis = %d\n", errorMessage, fd_, this);
		LOGMYE("JHTTP", err_msg, 0);
		m_nLastErrorNo = errno;
		return errno;
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~sdk_log~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void sdk_Log::Reset()
{
	m_bIsOver = false;
	m_strLog = "[";
}

void sdk_Log::Add(const char* key, const char* value)
{
	m_strLog += "\"";
	m_strLog += key;
	m_strLog += "\"";
	m_strLog += ":";
	m_strLog += "\"";
	m_strLog += value;
	m_strLog += "\"";
}

void sdk_Log::Write(const char* step, const char* status, int error, int socket_error)
{
	if (step == NULL || status == NULL)
		return;
	m_strLog += "{";
	Add("time", GetSystemTimeS().c_str());
	m_strLog += ",";
	Add("step", step);
	m_strLog += ",";
	Add("status", status);
	if (error != 0)
	{
		char tmp[20] = { 0 };
		sprintf(tmp, "%d", error);
		m_strLog += ",";
		Add("error code", tmp);
	}
	if (socket_error != 0)
	{
		char tmp[20] = { 0 };
		sprintf(tmp, "%d", socket_error);
		m_strLog += ",";
		Add("socket error", tmp);
	}
	m_strLog += "},";
}

void sdk_Log::Over()
{
	std::string::size_type pos = 0;
	if ((pos = m_strLog.find_last_of(",")) != std::string::npos)
	{
		m_strLog.erase(pos, 1);
	}
	m_strLog += "]";
	m_bIsOver = true;
}

const char* sdk_Log::GetLog()
{
	if (!m_bIsOver)
		Over();
	return m_strLog.c_str();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~static function~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
