#ifndef __SDK_REQ_H__
#define __SDK_REQ_H__

#include "sdk_crypt.h"
#include "JHttpClient.h"
#include "tokenTools.h"

#include <string>
#include <map>

#define TOKEN_LEN_MIN	50	// token len is 128 from service...
#define DEFAULT_WRITE_TO 8	//8 second
#define DEFUALT_READ_TO 10*1000*1000	//10 second
typedef long long TTSHANDLE;

enum{
    USC_OK = 0,
    USC_RECEIVING_AUDIO_DATA = 1,	//还有语音结果需要接收	m_requestState
	USC_AUDIO_DATA_RECV_DONE = 2,	//语音结果全部接收完毕	m_requestState
	USC_AUDIO_DATA_RECV_END = 3,	//已经end
	USC_CANCEL = 4,
    USC_SYNTH_PROCESS_ERROR = -91134,//错误
};

// m_nServiceType
enum{
    USC_ASR_SER = 3000,				// asr
    USC_TTS_SER = 3001,				// tts
    USC_VPR_SER = 3002,				// vpr
    USC_GDR_SER = 3003,				// gdr
};

class sdk_req
{
public:
    sdk_req();
	sdk_req(std::string uri);
    ~sdk_req();
    void InitParamater();						// 初始化一些属性

public:

    TNHANDLE			m_pTokenHandle;				// token
    sdk_crypt			m_crypt;					// 加密
	bool				m_bIsNeedMd5;
	std::string			m_strSecret;

    std::string			m_strBuffer;				// asr 结果
	std::string			m_strTmpBuffer;				// asr 临时结果
	std::string			m_strLastpBuffer;			// vpr 最后一次结果
	char *				m_chBuffer;					// tts 结果
	unsigned int		m_nBufferLength;
	bool				m_bIsGetResult;				// 判断是否取过数据
	bool				m_bIsGetTmpResult;			// 判断是否取过数据

	std::string			m_strLogioUri;				// 发起请求的 uri
	std::string			m_strReqUri;				// 收发数据的 uri

    /*
     *      http 请求所带的头
     */
	std::map<std::string, std::string> m_mapSerParam;
	std::string			m_strTrafficParameter;
	
	int					m_nServiceType;				// 不变
	std::string			m_strIMEI;
	std::string			m_strUDID;
	std::string			m_strAppKey;				// 不变
	std::string			m_strUserID;				// 不变
	std::string			m_strCryptVersion;			// 不变
	std::string			m_strAudioVersion;			// 不变

    // setoption
    int				m_nDeviceOS;
    int				m_nNetworkType;
    int 			m_nProbeNum4TTS;
	std::string		m_strPackageName;

	std::string		m_strSessionRequestID;		// 一次传入后保持不变
	std::string		m_strToken;
	std::string		m_strTickTime;
	std::string		m_strRetToken;

	JDpipeLined::HttpClient* m_reqHttpRequest;
	std::string		m_strServiceReqCmd;

    // client info
    std::string		m_strClientInfo;			// 客户端属性，已经拼接好的字符串

private:
    bool			m_bIsInit;					// 是否已经初始化
    bool			m_bIsLogIn;					// 判断是否进行过Login 操作
    bool			m_bIsBeginGetText;			// 是否已经开始取回数据（否则开始，调用初始化）
	bool			m_bIsCancel;
	std::string 	m_host;
	std::string 	m_port;
	int				m_nReqStep;
	int				m_nRespStep;
	int				m_bIsGetStartResp;

	int				m_nStartTimes;
	int				m_nLastError;

	sdk_Log			m_log;

private:
	// network time out limit

	int				m_nSleepTime;
	int				m_nHandshakeTimeOut;
	int				m_nAckTimeOut;
	int				m_nOneRespTimeOut;
	int				m_nEndTimeOut;

public:
	int				GetLastError();
	bool			IsInit();											// 判断是否初始化
	bool			IsCancel();											// 判断是否已经cancel
    bool			IsLogin();											// 判断是Login
    bool			IsBeginGetText();									// 判断是否进行了开始取数据的初始化
    bool			IsNetworkOK();										// 判断网络是否正常
    bool			SetAddress(char* ip, char* port);					// set ip && port
	void			SetAppKey(std::string appKey);						// 设置appkey
    int				Init(int service);									// 设置服务，暂时默认tts
    void			ResetNetworkStatus();								// 重置网络状态
    void			ResetIsLogin();										// 重置到未Login 状态
	std::string		JointParamater(std::map<std::string, std::string>* mapParamater);	// 拼接属性字符串
    int 			SetSerParamater(const char * key, const char * value);// 设置 ServiceParameter
    int				SetTraParamater(const char * key, const char * value);// 设置 NluParameter
	void			SetSleepTime(int timeMS);
	std::string		SetSdkParam(int flag = 0, int step = -1);			// 拼接P头 15.4.14 增加

	int				SetServiceStatus(int status);
    int				SetOptionValue(int id, const char* value);			// 设置属性
    const char*		GetOptionValue(int id);
    void			ClearOptionValue(int id);

    int				Login();													// 发起Login，进行用户验证，创建一个session
	int				StartRequest(char* reqBody = NULL, int reqBodyLen = 0);		// 发起服务器请求
	int				PutRequest(char* reqBody = NULL, int reqBodyLen = 0, 
											bool is_last = false);				// 发起探寻命令
    int				HaveResult();												// 查询是否有数据没取回
	//int				HaveTtsResult();
    int				have_tmpResult();											// 查询是否有临时数据没取回
    const char*		GetResult(int* errorRet, int* retLength);					// 获取探寻结果
    //const char*		GetResultTts(int * errorRet, int * retLength);				// tts
    int				CancelRequest();											// 取消请求
    int				EndRequest(char* buf = NULL, int* bytesRead = 0);				// 发起结束命令，最后一个参数是设置超时时间
    int				Logout();													// Logout，销毁session

	int				RecvOneResponse();
	int				TtsEndRequest();
	int				CloseFd();

	std::string 	CreateSessionID(int fd_, const char* token, const char* imei, const char* appkey);
	void			GetHttpResult(void* resp, int tmpLen);
};


#endif