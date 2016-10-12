#include "sdk_req.h"
#include "tokenWrap.h"
#include "errorNo.h"
#include "http_def.h"
#include "set_option_key.h"
#include "url_code.h"
#include "log.h"
#include "getTime.h"
#include "md5.h"
#include "sdk_crypt.h"
#include "function.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>

#ifndef WIN32
#include <sys/time.h>
#endif

//using namespace std;

static int TranslateErrorCode(int Error);


sdk_req::sdk_req()
{
	m_strLogioUri = string(U_HTTP_LABEL) + U_HTTP_HOST + ":" + U_HTTP_PORT + U_HTTP_LOGIO_URL_LAST;
	m_strReqUri = string(U_HTTP_LABEL) + U_HTTP_HOST + ":" + U_HTTP_PORT + U_HTTP_REQ_URL_LAST;
	InitParamater();
}

sdk_req::sdk_req(string uri)
{
	m_strLogioUri = uri + U_HTTP_LOGIO_URL_LAST;
	m_strReqUri = uri + U_HTTP_REQ_URL_LAST;
	InitParamater();
}

void sdk_req::InitParamater()
{
	m_bIsCancel = m_bIsNeedMd5 = m_bIsInit = m_bIsLogIn = m_bIsBeginGetText = m_bIsGetResult = m_bIsGetTmpResult = false;
	m_nLastError = m_nStartTimes = m_pTokenHandle = m_nRespStep = m_nReqStep = m_nBufferLength = m_nNetworkType = m_nProbeNum4TTS = 0;
	m_nSleepTime = 20;
	m_chBuffer = NULL;
	m_reqHttpRequest = NULL;
	m_strBuffer = "";
	m_strLastpBuffer = "";
	m_nServiceType = 0;
	m_strAppKey = "";
	m_strUserID = "";
	m_strCryptVersion = "1";
	m_strAudioVersion = "";
	m_strToken = "";
	m_strIMEI = "";
	m_strUDID = "";
	m_strPackageName = "";
	m_strClientInfo = "";
	m_strTrafficParameter = "";
	m_strSessionRequestID = "";
	m_strSecret = "";
	m_strTickTime = "";
	m_strRetToken = "";
	m_nHandshakeTimeOut = N_HANDSHAKE_TO;
	m_nAckTimeOut = N_ACK_TO;
	m_nOneRespTimeOut = N_ONERESP_TO;
	m_nEndTimeOut = N_LAST_TO;
}

sdk_req::~sdk_req()
{
	if (m_pTokenHandle != NULL)
	{
		releaseTokenService(m_pTokenHandle);
		m_pTokenHandle = NULL;
	}
	if (m_chBuffer != NULL)
		free(m_chBuffer);
	if (m_reqHttpRequest != NULL)
		delete m_reqHttpRequest;
}

int sdk_req::Init(int service = USC_TTS_SER)
{
	if (USC_TTS_SER == service)
	{
		m_nServiceType = V_SERVICE_TTS;
	}
	else if (USC_ASR_SER == service)
	{
		m_nServiceType = V_SERVICE_ASR;
	}
	else if (USC_VPR_SER == service)
	{
		m_nServiceType = V_SERVICE_VPR;
	}
	else if (USC_GDR_SER == service)
	{
		m_nServiceType = V_SERVICE_SPR;
	}
	else
		return SET_SERVICE_ERROR;

	m_strAudioVersion = V_SDK_VERSION;
	m_strCryptVersion = V_CRYPT_VERSION;
	m_bIsInit = true;
	return USC_OK;
}

bool sdk_req::IsInit()
{
	return m_bIsInit;
}

bool sdk_req::IsCancel()
{
	return m_bIsCancel;
}

bool sdk_req::IsLogin()
{
	return m_bIsLogIn;
}

bool sdk_req::IsBeginGetText()
{
	return m_bIsBeginGetText;
}

void sdk_req::SetAppKey(string appKey)
{
	m_strAppKey = appKey;
}

bool sdk_req::SetAddress(char* ip, char* port)
{
	m_strLogioUri = U_HTTP_LOGIO_URL_LAST;
	m_strReqUri = U_HTTP_REQ_URL_LAST;

	m_host = ip;
	m_port = port;

	return true;
}

void sdk_req::ResetNetworkStatus()
{
	m_strSessionRequestID = "";
	m_bIsGetResult = false;
	m_bIsGetTmpResult = false;
	m_nBufferLength = 0;
	m_nProbeNum4TTS = -1;
	m_strBuffer = "";
	m_strTmpBuffer = "";
	m_strLastpBuffer = "";
	// 记录步骤
	m_nLastError = 0;
	m_nReqStep = 0;
	m_nRespStep = 0;
	// 设置加密信息
	m_crypt.clean();
	m_crypt.SetCryptVersion(atoi(m_strCryptVersion.c_str()));
	m_crypt.SetUseMd5(m_bIsNeedMd5);
}

void sdk_req::ResetIsLogin()
{
	m_bIsLogIn = false;
}

string sdk_req::JointParamater(std::map<string, string>* mapParamater)
{
	if (NULL == mapParamater)
		return "";
	string str_ret = "";
	std::map<string, string>::iterator map_it;
	for (map_it = mapParamater->begin(); map_it != mapParamater->end(); ++map_it)
	{
		str_ret = str_ret + map_it->first + "=" + map_it->second + ";";
	}
	return str_ret;
}

int sdk_req::SetSerParamater(const char * key, const char * value)
{
	if (key == NULL || strlen(key) > I_LIMITED_SIMPLE || strlen(key) < 1)
		return OPTION_PARAM_ERROR;
	if (value == NULL || strlen(value) > I_LIMITED_SIMPLE || strlen(value) < 1)
		return OPTION_PARAM_ERROR;
	LOGMYI((char*)key, (char*)value, 3);
	if (m_mapSerParam.find(key) != m_mapSerParam.end())
	{
		m_mapSerParam[key] = value;
	}
	else
	{
		m_mapSerParam.insert(std::make_pair(key, value));
	}
	return true;
}

int sdk_req::SetTraParamater(const char * key, const char * value)
{
	return 0;
}

void sdk_req::SetSleepTime(int timeMS)
{
	m_nSleepTime = timeMS;
}

int sdk_req::SetServiceStatus(int status)
{
	m_nServiceType = status;
	return 0;
}

string sdk_req::SetSdkParam(int flag, int step)
{
	string ret("");

	if (step >= 0)
	{
		ret += H_REQ_NUM_CMD;
		char tmp[10] = { 0 };
		sprintf(tmp, "%d", step);
		ret += tmp;
	}
	ret += H_AUDIO_VERSION;
	ret += V_SDK_VERSION;
	ret += H_SERVICE_TYPE;
	char tmp[100] = { 0 };
	sprintf(tmp, "%d", m_nServiceType);
	if (flag == 0)
		ret += tmp;
	else if (flag == 1)
		ret += V_LOGIN;
	else
		ret += V_LOGOUT;
	ret += H_SERVICE_REQ_CMD + m_strServiceReqCmd;
	ret += H_CRYPT_VERSION;
	ret += V_CRYPT_VERSION;
	ret += H_SESSION_REQUEST_ID;
	ret += P_OP_PAREN + m_strSessionRequestID + P_CL_PAREN;
	return ret;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~以下为接口函数~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int	sdk_req::Login()
{
	m_log.Write("login", "begin");
	LOGMYI("SDK", "login", 1);
	LOGMYI("SDK", (char*)GITTAG, 3);
	if (!IsInit())
		return NO_INIT_ERROR;

	m_bIsLogIn = false;
	m_bIsGetResult = false;
	m_bIsGetTmpResult = false;

	m_nStartTimes = 0;

	int ret = USC_OK;
	m_strServiceReqCmd = V_LOGINOUT;

	if (0 != m_pTokenHandle)
	{
		releaseTokenService(m_pTokenHandle);
		m_pTokenHandle = 0;
	}
	//sdk_req request;
	//ret = request.HttpInit(m_strLogioUri.c_str());
	JDpipeLined::HttpClient request(m_host, m_port);

	ret = request.InitAndConnect();
	if (ret == JDpipeLined::NORMAL)
	{
		HttpRequestInfo loginRequest;
		loginRequest.url = m_strLogioUri;

		sdk_crypt sdkCrypt;
		sdkCrypt.SetCryptVersion(atoi(m_strCryptVersion.c_str()));
		sdkCrypt.SetHttpHeader(H_SDKPARAM, SetSdkParam(1));
		sdkCrypt.SetHttpHeader(H_APPKEY, m_strAppKey);
		sdkCrypt.SetHttpBody(NULL, 0);
		sdkCrypt.PrepareHttpRequest(&loginRequest, -1);

		//send request
		int httpRespGotten = 0;
		ret = request.SendHttpRequest(&loginRequest, httpRespGotten, false);

		//ret = request.HttpProcess();
		if (ret == JDpipeLined::NORMAL)
		{
			//recv response
			std::queue<HttpResponseInfo>* respQueue = request.GetHttpResponse(ret, m_log);

			if (ret == JDpipeLined::NORMAL && respQueue->size() > 0)
			{
				HttpResponseInfo rpInfo = respQueue->front();
				if (rpInfo.statusCode != 200)
				{
					ret = TransError2User(rpInfo.statusCode);
					goto LOGIN_ERROR;
				}
				respQueue->pop();
				m_strToken = UrlDecodeUtf8(sdkCrypt.DeCrypt((char*)rpInfo.getHeader(H_TOKEN).c_str()));
				if (m_strToken.compare("") == 0)
				{
					ret = HTTP_REQ_ERROR;
					goto LOGIN_ERROR;
				}
				createTokenService(&m_pTokenHandle, m_strToken.c_str(), m_strAppKey.c_str());
				char tmpNum[500] = { 0 };
				sprintf(tmpNum, "%u", getCurrentTimeS());
				m_strTickTime = tmpNum;
				// return to android/iOS sdk
				sprintf(tmpNum, "%s\t%s", m_strToken.c_str(), m_strTickTime.c_str());
				m_strRetToken = tmpNum;
				m_bIsLogIn = true;
			}
		}
	}
	ret = TranslateErrorCode(ret);
LOGIN_ERROR:
	if (ret < 0)
		m_log.Write("login", "end", ret, m_nLastError);
	request.EndAndHandle();
	m_nLastError = request.GetLastError();
	return ret;
}

int sdk_req::StartRequest(char* reqBody, int reqBodyLen)
{
	m_nLastError = 0;
	m_bIsCancel = false;
	m_log.Reset();
	m_log.Write("request service", "begin");
	LOGMYI("SDK", "start", 1);
	if (!IsInit())
		return NO_INIT_ERROR;
	if (!IsLogin() || m_pTokenHandle == 0)
	{
		if (m_strToken.length() >= TOKEN_LEN_MIN)
		{
			createTokenService(&m_pTokenHandle, m_strToken.c_str(), m_strAppKey.c_str(), atoi(m_strTickTime.c_str()));
			if (m_pTokenHandle == 0)
				return NO_LOGIN_ERROR;
			else
				m_bIsLogIn = true;
		}
		else
			return NO_LOGIN_ERROR;
	}

	m_nStartTimes++;

	int ret = USC_OK;
	// 每次start 的时候，重置网络状态。
	ResetNetworkStatus();
	m_bIsBeginGetText = false;
	m_bIsGetStartResp = false;
	m_strServiceReqCmd = V_SERVICE_REQUEST;
	unsigned int len = 0;
	// genNewToken 会申请内存空间，记得释放。    
	const char* token = genNewToken(m_pTokenHandle, &len);
	if (token == NULL)
	{
		return GET_TOKEN_ERROR;
	}
	string m_strToken = token;
	free((void*)token);

	if (m_reqHttpRequest != NULL)
	{
		delete m_reqHttpRequest;
		m_reqHttpRequest = NULL;
	}
	m_reqHttpRequest = new JDpipeLined::HttpClient(m_host, m_port);
	m_reqHttpRequest->SetTimeoutLimit(m_nHandshakeTimeOut, m_nAckTimeOut, m_nOneRespTimeOut, m_nEndTimeOut, m_nSleepTime);
	ret = m_reqHttpRequest->InitAndConnect();

	if (ret == JDpipeLined::NORMAL)
	{
		HttpRequestInfo rSRequest;
		rSRequest.url = m_strReqUri;

		// create session id
		m_strSessionRequestID = CreateSessionID(m_reqHttpRequest->GetFd(), m_strToken.c_str(), m_strIMEI.c_str(), m_strAppKey.c_str());
		m_log.Write("session id", m_strSessionRequestID.c_str());

		m_crypt.SetSecret(m_strSecret.c_str());
		m_crypt.SetHttpHeader(H_SDKPARAM, SetSdkParam(0, m_nReqStep));
		m_crypt.SetHttpHeader(H_APPKEY, m_strAppKey);
		m_crypt.SetHttpHeader(H_USER_ID, m_strUserID);
		m_crypt.SetHttpHeader(H_IMEI, m_strIMEI);
		m_crypt.SetHttpHeader(H_UDID, m_strUDID);
		m_crypt.SetHttpHeader(H_TOKEN, UrlEncodeUtf8(m_strToken));
		m_crypt.SetHttpHeader(H_SERVICE_PARAM, UrlEncodeUtf8(JointParamater(&m_mapSerParam)));
		m_crypt.SetHttpHeader(H_CLIENT_INFO, UrlEncodeUtf8(m_strClientInfo));

		if (m_strTrafficParameter.length() > 5)
		{
			m_crypt.SetHttpHeader(H_NLU_PARAMETER, UrlEncodeUtf8(m_strTrafficParameter));
		}
		if (reqBody != NULL && reqBodyLen > 0)
		{
			m_crypt.SetHttpBody(reqBody, reqBodyLen);
		}

		//send request
		int httpRespGotten = 0;
		if (0 != (ret = m_crypt.PrepareHttpRequest(&rSRequest, m_nReqStep)))
			goto START_ERROR;
		m_crypt.ClearRequest();

		char msg[128] = { 0 };
		sprintf(msg, "Session id : %s", m_strSessionRequestID.c_str());
		LOGMYI("SDK", msg, 1);

		ret = m_reqHttpRequest->SendHttpRequest(&rSRequest, httpRespGotten);

		if (ret == 0)
		{
			m_bIsBeginGetText = true;
			if (httpRespGotten > 0)
			{
				std::queue<HttpResponseInfo>* respQueue = m_reqHttpRequest->GetHttpResponseQueue();
				HttpResponseInfo rpInfo = respQueue->front();
				if (rpInfo.statusCode != 200)
				{
					ret = TransError2User(rpInfo.statusCode);
					goto START_ERROR;
				}
				respQueue->pop();
				string sta = m_crypt.DeCrypt((char*)rpInfo.getHeader(H_REQUEST_STATE_ADD).c_str());
				if (sta.length() > 1)
				{
					string tmpNum = "1";
					tmpNum.assign(sta.c_str(), sta.find("=") + 1, sta.length() - sta.find("="));
					if (tmpNum != "")
						m_nProbeNum4TTS = atoi(tmpNum.c_str());
					else
					{
						ret = GET_INFO_ERROR;
						goto START_ERROR;
					}
				}
				m_nRespStep++;
			}
			return ret;
		}
	}
	ret = TranslateErrorCode(ret);
START_ERROR:
	char msg[128] = { 0 };
	sprintf(msg, "start error, ret : %d", ret);
	LOGMYE("SDK", msg, 1);
	m_reqHttpRequest->EndAndHandle();
	m_nLastError = m_reqHttpRequest->GetLastError();
	m_log.Write("request service", "end", ret, m_nLastError);
	delete m_reqHttpRequest;
	m_reqHttpRequest = NULL;
	return ret;
}

int sdk_req::PutRequest(char* reqBody, int reqBodyLen, bool is_last)
{
	if (m_nReqStep == 0)
		m_log.Write("probe", "begin");
	LOGMYI("SDK", "probe", 1);
	if (IsCancel())
		return USC_CANCEL;
	if (!IsInit() || !IsBeginGetText())
		return NO_INIT_ERROR;
	if (!IsLogin() || m_reqHttpRequest == NULL)
		return NO_LOGIN_ERROR;
	if (m_bIsGetResult)
	{
		m_strBuffer = "";
		m_nBufferLength = 0;
		m_bIsGetResult = false;
	}

	int ret = USC_OK;
	m_nReqStep++;
	// 请求复用前，需要把 resp 和 conn 复位
	int returnStat = GET_INFO_ERROR;
	HttpRequestInfo rSRequest;
	rSRequest.url = m_strReqUri;
	m_strServiceReqCmd = V_SERVICE_PROBE;
	m_crypt.SetHttpHeader(H_SDKPARAM, SetSdkParam(0, m_nReqStep));
	m_strTrafficParameter = "";

	string			requestStatus;
	int				tmpLen;
	// asr 添加
	if (reqBody != NULL && reqBodyLen > 0)
	{
		m_crypt.SetHttpBody(reqBody, reqBodyLen);
	}
	//send request
	int httpRespGotten = 0;

	if (0 != (ret = m_crypt.PrepareHttpRequest(&rSRequest, m_nReqStep))){
		//goto GET_RESULT_END;
		m_reqHttpRequest->EndAndHandle();
		m_nLastError = m_reqHttpRequest->GetLastError();
		delete m_reqHttpRequest;
		m_reqHttpRequest = NULL;
		m_log.Write("probe", "end", ret, m_nLastError);
		return ret;
	}
	m_crypt.ClearRequest();

	ret = m_reqHttpRequest->SendHttpRequest(&rSRequest, httpRespGotten, !is_last);

	//sleepMS(m_nSleepTime);
	m_nProbeNum4TTS--;
	if (ret == JDpipeLined::IS_CANCEL || IsCancel()){
		goto GET_RESULT_END;
	}
	else if (ret != JDpipeLined::NORMAL){
		ret = TranslateErrorCode(ret);;
		goto GET_RESULT_END;
	}

	//recv response
	if (httpRespGotten == 1)
	{
		HttpResponseInfo rpInfo;
		int stat = m_reqHttpRequest->GetFirstHttpResponse(rpInfo);
		if (stat == 0)
		{
			if (rpInfo.statusCode != 200)
			{
				ret = TransError2User(rpInfo.statusCode);
				goto GET_RESULT_END;
			}
			requestStatus = m_crypt.DeCrypt((char*)rpInfo.getHeader(H_REQUEST_STATE).c_str());

			tmpLen = atoi(rpInfo.getHeader(H_CONTENT_LENGTH).c_str());
			if (tmpLen > 0)
			{
				GetHttpResult((void*)&rpInfo, tmpLen);
			}
			m_nRespStep++;

			//printf("m_nRespStep : %d\n", m_nRespStep);			
			// 如果接受完毕
			if (requestStatus.compare(V_REQUEST_STATE_END) == 0 || requestStatus.compare(V_REQUEST_STATE_RELEASE) == 0)
				returnStat = USC_AUDIO_DATA_RECV_DONE;
			else if (requestStatus.compare(V_REQUEST_STATE_DOING) == 0)
				returnStat = USC_RECEIVING_AUDIO_DATA;
			else
				returnStat = GET_INFO_ERROR;
		}
	}
	else{
		returnStat = USC_RECEIVING_AUDIO_DATA;
	}

	if (returnStat == USC_RECEIVING_AUDIO_DATA && m_nProbeNum4TTS <= 0 && m_nServiceType == V_SERVICE_TTS)
		returnStat = USC_AUDIO_DATA_RECV_DONE;

	//m_log.Write("probe", "end", ret, m_nLastError);
	return returnStat;

GET_RESULT_END:
	char msg[128] = { 0 };
	sprintf(msg, "probe error, ret : %d", ret);
	LOGMYE("SDK", msg, 1);
	m_reqHttpRequest->EndAndHandle();
	m_nLastError = m_reqHttpRequest->GetLastError();
	m_log.Write("probe", "end", ret, m_nLastError);
	delete m_reqHttpRequest;
	m_reqHttpRequest = NULL;
	return ret;
}

int sdk_req::HaveResult()
{
	if (!m_bIsGetResult && m_nBufferLength > 0)
	{
		return m_nBufferLength;
	}
	return 0;
}

int sdk_req::have_tmpResult()
{
	if (!m_bIsGetTmpResult && m_strTmpBuffer.compare("") != 0)
		return m_strTmpBuffer.length();
	return 0;
}

const char* sdk_req::GetResult(int * errorRet, int * retLength)
{
	if (!m_bIsGetResult)
	{
		char msg[128] = { 0 };
		sprintf(msg, "GetResult : %d", m_nBufferLength);
		LOGMYI("SDK", msg, 1);
		*errorRet = USC_OK;
		*retLength = m_nBufferLength;
		m_bIsGetResult = true;
		if (m_nServiceType == V_SERVICE_TTS)
			return m_chBuffer;
		else
			return m_strBuffer.c_str();
	}
	else
	{
		LOGMYE("SDK", "GetResult : no result!", 1);
		return "";
	}
}

int sdk_req::CancelRequest()
{
	LOGI("UNISDK", "Cancel");
	m_bIsCancel = true;
	if (m_reqHttpRequest != NULL)
		m_reqHttpRequest->SetCancel(true);
	return USC_OK;
}

int sdk_req::EndRequest(char* buf, int* bytesRead)
{
	m_log.Write("normal end", "begin");
	LOGMYI("SDK", "stop", 1);
	if (IsCancel())
		return USC_CANCEL;
	if (!IsInit() || !IsBeginGetText())
		return NO_INIT_ERROR;
	if (!IsLogin())
		return NO_LOGIN_ERROR;
	if (m_bIsGetResult)
	{
		m_strBuffer = "";
		m_nBufferLength = 0;
		m_bIsGetResult = false;
	}
	int ret = USC_OK;
	int tmpLen = 0;
	m_nReqStep++;

	HttpRequestInfo rSRequest;
	rSRequest.url = m_strReqUri;
	m_strServiceReqCmd = V_SERVICE_NORMALEND;

	m_crypt.SetHttpHeader(H_SDKPARAM, SetSdkParam(0, m_nReqStep));
	if (m_strTrafficParameter.length() > 0)
	{
		m_crypt.SetHttpHeader(H_NLU_PARAMETER, UrlEncodeUtf8(m_strTrafficParameter));
	}
	//send request
	int httpRespGotten = 0;
	//if (NULL != m_chBuffer)
	//{
	//	free(m_chBuffer);
	//	m_chBuffer = NULL;
	//}
	m_crypt.PrepareHttpRequest(&rSRequest, m_nReqStep);
	m_crypt.ClearRequest();
	ret = m_reqHttpRequest->SendHttpRequest(&rSRequest, httpRespGotten);

	if (ret == JDpipeLined::NORMAL)
	{
		int mallocLen = 0;
		//recv response
		std::queue<HttpResponseInfo>* respQueue = m_reqHttpRequest->GetHttpResponse(ret, m_log, 10000, &mallocLen);

		//m_chBuffer = (char*)malloc(mallocLen+1);
		//printf("malloc len : %d\n", mallocLen+1);
		int offset = 0;
		int tmpFlag = 0;

		while (ret == JDpipeLined::NORMAL && respQueue->size()>0)
		{
			if (IsCancel()){
				ret = USC_CANCEL;
				break;
			}
			//printf("%d\n", tmpFlag++);
			HttpResponseInfo rpInfo = respQueue->front();
			if (rpInfo.statusCode != 200)
			{
				ret = TransError2User(rpInfo.statusCode);
				break;
			}
			respQueue->pop();
			if ((tmpLen = atoi(rpInfo.getHeader(H_CONTENT_LENGTH).c_str())) > 0)
			{
				GetHttpResult((void*)&rpInfo, tmpLen);
			}
			m_nRespStep++;
		}
	}
	if (ret != USC_OK)
	{
		ret = TranslateErrorCode(ret);
		char tmp[128] = { 0 };
		sprintf(tmp, "EndRequest error, ret = %d", ret);
		LOGMYE("SDK", tmp, 1);
	}
	m_bIsBeginGetText = false;
	m_reqHttpRequest->EndAndHandle();
	m_nLastError = m_reqHttpRequest->GetLastError();
	if (ret < 0)
		m_log.Write("normal end", "end", ret, m_nLastError);
	delete m_reqHttpRequest;
	m_reqHttpRequest = NULL;
	return ret;
}

int sdk_req::TtsEndRequest()
{
	m_log.Write("normal end", "begin");
	LOGMYI("SDK", "stop", 1);
	if (IsCancel())
		return USC_CANCEL;
	if (!IsInit() || !IsBeginGetText())
		return NO_INIT_ERROR;
	if (!IsLogin() || m_reqHttpRequest == NULL)
		return NO_LOGIN_ERROR;
	if (m_bIsGetResult)
	{
		m_strBuffer = "";
		m_nBufferLength = 0;
		m_bIsGetResult = false;
	}
	int ret = USC_OK;
	int tmpLen = 0;
	m_nReqStep++;
	//recv response
	string			requestStatus;

	HttpRequestInfo rSRequest;
	rSRequest.url = m_strReqUri;
	m_strServiceReqCmd = V_SERVICE_NORMALEND;
	m_crypt.SetHttpHeader(H_SDKPARAM, SetSdkParam(0, m_nReqStep));

	//send request
	int httpRespGotten = 0;
	if (NULL != m_chBuffer)
	{
		free(m_chBuffer);
		m_chBuffer = NULL;
	}

	m_crypt.PrepareHttpRequest(&rSRequest, m_nReqStep);
	m_crypt.ClearRequest();

	ret = m_reqHttpRequest->SendHttpRequest(&rSRequest, httpRespGotten);

	if (ret != JDpipeLined::NORMAL){
		ret = SEND_REQUEST_ERROR;
		goto TTS_END_ERROR;
	}
	else if (ret == JDpipeLined::IS_CANCEL)	{
		goto TTS_END_ERROR;
	}

	if (httpRespGotten == 1){

		HttpResponseInfo rpInfo;
		int stat = m_reqHttpRequest->GetFirstHttpResponse(rpInfo);
		if (stat == 0)
		{
			if (rpInfo.statusCode != 200)
			{
				ret = TransError2User(rpInfo.statusCode);
				goto TTS_END_ERROR;
			}
			requestStatus = m_crypt.DeCrypt((char*)rpInfo.getHeader(H_REQUEST_STATE).c_str());

			tmpLen = atoi(rpInfo.getHeader(H_CONTENT_LENGTH).c_str());
			if (tmpLen > 0)
			{
				GetHttpResult((void*)&rpInfo, tmpLen);
			}
			m_nRespStep++;
		}
	}
	int returnStat;
	// 如果接受完毕
	if (requestStatus.compare(V_REQUEST_STATE_END) == 0)
		returnStat = USC_AUDIO_DATA_RECV_DONE;
	else
		returnStat = USC_RECEIVING_AUDIO_DATA;

	//m_log.Write("normal end", "end", ret, m_nLastError);
	return returnStat;

TTS_END_ERROR:

	char msg[128] = { 0 };
	sprintf(msg, "stop error, ret : %d", ret);
	LOGMYE("SDK", msg, 1);
	m_reqHttpRequest->EndAndHandle();
	m_nLastError = m_reqHttpRequest->GetLastError();
	m_log.Write("normal end", "end", ret, m_nLastError);
	delete m_reqHttpRequest;
	m_reqHttpRequest = NULL;
	return ret;
}

int sdk_req::CloseFd()
{
	if (!IsLogin() || m_reqHttpRequest == NULL)
		return NO_LOGIN_ERROR;
	m_reqHttpRequest->EndAndHandle();
	return 0;
}

int sdk_req::RecvOneResponse()
{
	//m_log.Write("RecvOneResponse", "begin");
	if (IsCancel())
		return USC_CANCEL;
	if (!IsInit() || !IsBeginGetText())
		return NO_INIT_ERROR;
	if (!IsLogin() || m_reqHttpRequest == NULL)
		return NO_LOGIN_ERROR;
	if (m_bIsGetResult)
	{
		m_strBuffer = "";
		m_nBufferLength = 0;
		m_bIsGetResult = false;
	}
	int stat = GET_RESPONSE_ERROR;
	int ret = 0;
	m_reqHttpRequest->GetHttpResponse(ret, m_log, 1);
	HttpResponseInfo rpInfo;
	int tmpLen;
	string requestStatus;
	if (ret == JDpipeLined::IS_CANCEL)
		goto RECV_ONE_RESP_END;
	else if (ret < 0)
		ret = TranslateErrorCode(ret);

	if (ret < 0)
		goto RECV_ONE_RESP_END;

	stat = m_reqHttpRequest->GetFirstHttpResponse(rpInfo);
	if (stat == 0)
	{
		if (rpInfo.statusCode != 200)
		{
			ret = TransError2User(rpInfo.statusCode);
			goto RECV_ONE_RESP_END;
		}
		string sta = m_crypt.DeCrypt((char*)rpInfo.getHeader(H_REQUEST_STATE_ADD).c_str());
		if (sta.length() > 1)
		{
			string tmpNum = "1";
			tmpNum.assign(sta.c_str(), sta.find("=") + 1, sta.length() - sta.find("="));
			if (tmpNum != "")
				m_nProbeNum4TTS = atoi(tmpNum.c_str());
			else
			{
				ret = GET_INFO_ERROR;
				goto RECV_ONE_RESP_END;
			}
		}
		requestStatus = m_crypt.DeCrypt((char*)rpInfo.getHeader(H_REQUEST_STATE).c_str());
		tmpLen = atoi(rpInfo.getHeader(H_CONTENT_LENGTH).c_str());
		if (tmpLen > 0)
		{
			GetHttpResult((void*)&rpInfo, tmpLen);
		}
		m_nRespStep++;
		if (m_nRespStep == m_nReqStep + 1)
			ret = USC_AUDIO_DATA_RECV_DONE;
		else if (m_nRespStep == m_nReqStep)
			ret = USC_AUDIO_DATA_RECV_DONE;
		else
			ret = USC_RECEIVING_AUDIO_DATA;
		return ret;
	}
	else
	{
		ret = GET_NO_RESPONSE_ERROR;
	}

RECV_ONE_RESP_END:

	char msg[128] = { 0 };
	sprintf(msg, "recv one resp, error, ret : %d", ret);
	LOGMYE("SDK", msg, 1);
	m_reqHttpRequest->EndAndHandle();
	m_nLastError = m_reqHttpRequest->GetLastError();
	if (ret < 0)
		m_log.Write("RecvOneResponse", "end", ret, m_nLastError);
	delete m_reqHttpRequest;
	m_reqHttpRequest = NULL;
	return ret;
}

int sdk_req::SetOptionValue(int id, const char* value)
{
	int ret = 0;
	int tmp;
	if (value == NULL || strlen(value) > I_LIMITED_HARD)
		return OPTION_PARAM_ERROR;
	switch (id)
	{
	case OPT_COLLECTED_INFO:
		m_strClientInfo = value;
		break;
	case OPT_IMEI_ID:
		m_strIMEI = value;
		break;
	case OPT_UDID_ID:
		m_strUDID = value;
		break;
	case OPT_APP_KEY:
		m_strAppKey = value;
		break;
	case OPT_USER_ID:
		m_strUserID = value;
		break;
	case OPT_DEVICE_OS:
		m_nDeviceOS = atoi(value);
		break;
	case OPT_NETWORK_TYPE:
		m_nNetworkType = atoi(value);
		break;
	case OPT_PACKAGE_NAME:
		m_strPackageName = value;
		break;
	case ASR_TRAFFIC_PARAMETER:
		m_strTrafficParameter = value;
		break;
	case OPT_REQUEST_SLEEP_TIME:
		m_nSleepTime = atoi(value);
		break;
	case OPT_USE_MD5:
		if (strcmp(value, "true") == 0 || strcmp(value, "TRUE") == 0)
			m_bIsNeedMd5 = true;
		else
			m_bIsNeedMd5 = false;
		break;
	case OPT_SECRET:
		m_strSecret = value;
		break;
	case OPT_CRYPT_VERSION:
		m_strCryptVersion = value;
		break;
	case OPT_SET_TOKEN:
		m_strRetToken = value;
		tmp = m_strRetToken.find("\t", 0);
		if (tmp < 0)
			return OPTION_PARAM_ERROR;
		m_strToken = m_strRetToken.substr(0, tmp);
		m_strTickTime = m_strRetToken.substr(tmp + 1, m_strRetToken.length() - tmp);
		break;
	default:
		ret = OPTION_ID_ERROR;
		break;
	}
	return ret;
}

const char* sdk_req::GetOptionValue(int id)
{
	switch (id)
	{
	case USC_GET_TMP_RESULT_PARAMETER:
		if (have_tmpResult())
		{
			m_bIsGetTmpResult = true;
			return m_strTmpBuffer.c_str();
		}
		break;
	case USC_GET_SESSION_ID:
	case USC_GET_SESSION_ID_2:
		return m_strSessionRequestID.c_str();
		break;
	case USC_GET_LAST_RESULT:
		return m_strLastpBuffer.c_str();
		break;
	case USC_GET_ERROR_LOG:
		return m_log.GetLog();
		break;
	case OPT_USER_ID:
		return m_strUserID.c_str();
		break;
	case OPT_APP_KEY:
		return m_strAppKey.c_str();
		break;
	case OPT_IMEI_ID:
		return m_strIMEI.c_str();
		break;
	case OPT_UDID_ID:
		return m_strUDID.c_str();
		break;
	case OPT_SECRET:
		return m_strSecret.c_str();
		break;
	case OPT_CRYPT_VERSION:
		return m_strCryptVersion.c_str();
		break;
	case USC_GET_TOKEN:
		return m_strRetToken.c_str();
		break;
	}
	return "";
}

void sdk_req::ClearOptionValue(int id)
{
	return;
}

int sdk_req::GetLastError()
{
	return m_nLastError;
}

std::string sdk_req::CreateSessionID(int fd_, const char* token, const char* imei, const char* appkey)
{
	std::string tmp_str = "";
	tmp_str = tmp_str + token + imei + appkey;
	char tmp_c[20] = { 0 };
	sprintf(tmp_c, "%d", fd_);
	tmp_str += tmp_c;
	sprintf(tmp_c, "%u", getCurrentTimeM());
	tmp_str += tmp_c;
	sprintf(tmp_c, "%d", m_nStartTimes);
	tmp_str += tmp_c;
	tmp_str += GetUuid();
	char md5ans[100] = { 0 };
	GetMd5((char*)tmp_str.c_str(), md5ans);
	return md5ans;
}

void sdk_req::GetHttpResult(void* resp, int tmpLen)
{
	HttpResponseInfo* rpInfo = (HttpResponseInfo*)resp;
	if (m_nServiceType & V_SERVICE_TTS)
	{
		if (m_chBuffer != NULL)
			free(m_chBuffer);
		m_nBufferLength += tmpLen;
		m_chBuffer = (char*)malloc(rpInfo->getBodyLen());
		memcpy(m_chBuffer, rpInfo->getBodyPtr(), rpInfo->getBodyLen());
		LOGMYI("GET-TTS-BUFFER", m_chBuffer, 1);
	}
	else
	{
		char* retType = m_crypt.DeCrypt((char*)(rpInfo->getHeader(H_CONTENT_TYPE).c_str()));
		if (retType != NULL && strstr(retType, "-var") > 0)
		{
			m_bIsGetTmpResult = false;
			m_strTmpBuffer.assign(rpInfo->getBodyPtr(), rpInfo->getBodyLen());
			LOGMYI("GET-ASR-TMP-BUFFER", (char*)m_strTmpBuffer.c_str(), 1);
		}
		else
		{
			m_nBufferLength += tmpLen;
			string tmpStr = "";
			tmpStr.assign(rpInfo->getBodyPtr(), rpInfo->getBodyLen());
			m_strBuffer += tmpStr;
			LOGMYI("GET-ASR-BUFFER", (char*)tmpStr.c_str(), 1);
			if (m_nRespStep == m_nReqStep)
				m_strLastpBuffer = tmpStr;
		}
	}
}

int sdk_req::Logout()
{
	return 0;
}

static int TranslateErrorCode(int Error)
{
	int ret = 0;
	switch (Error)
	{
	case JDpipeLined::SOCKET_ERROR_:
		return SOCKET_ERROR;
	case JDpipeLined::CONNECT_ERROR_:
		return CONNECT_ERROR;
	case JDpipeLined::CONNECT_SELECT_ERROR_:
		return CONNECT_SELECT_ERROR;
	case JDpipeLined::CONNECT_SELECT_TIMEOUT_:
		return CONNECT_SELECT_TIMEOUT;
	case JDpipeLined::SEND_ERROR_:
		return SEND_ERROR;
	case JDpipeLined::SEND_SELECT_ERROR_:
		return SEND_SELECT_ERROR;
	case JDpipeLined::SEND_SELECT_TIMEOUT_:
		return SEND_SELECT_TIMEOUT;
	case JDpipeLined::RECV_ERROR_:
		return RECV_ERROR;
	case JDpipeLined::RECV_SELECT_ERROR_:
		return RECV_SELECT_ERROR;
	case JDpipeLined::RECV_SELECT_TIMEOUT_:
		return RECV_SELECT_TIMEOUT;
	case JDpipeLined::SET_SOCKET_OPTION_ERROR_:
		return SET_SOCKET_OPTION_ERROR;
	case JDpipeLined::SET_FCNTL_ERROR_:
		return SET_FCNTL_ERROR;
	case JDpipeLined::GET_HOST_NAME_ERROR_:
		return GET_HOST_NAME_ERROR;
	case JDpipeLined::OTHER_ERROR:
		return OTHER_ERROR;
	case JDpipeLined::TIMEOUT:
		return NETWORK_TIMEOUT_ERROR;
	default:
		return Error;
	}
	return ret;
}