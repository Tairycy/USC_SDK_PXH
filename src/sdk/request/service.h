#ifndef   __SERVICE_H__
#define   __SERVICE_H__

#include "sdk_req.h"
#include "errorNo.h"
#include "log.h"
#include <string>

class UniServiceInterface
{
public:
	//virtual int setVadSilTime(int frontSil, int backSil) = 0;
	//virtual int setVadParas(int id, const void *value) = 0;
	//virtual void clearOptionValue(int id) = 0;

	//virtual int isactive(char*pcm, int pcm_len) = 0;
	//virtual int getLastError() = 0;               //get error code when error occured

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

public:
	UniServiceInterface()
		:m_strAppkey(""), m_strSecret(""), m_strHost(""), m_strPort(""), m_nLastError(0){
		m_sdkReq = new sdk_req();
	}

	virtual ~UniServiceInterface() {
		if (m_sdkReq != NULL)
			delete m_sdkReq;
	};

public:
	sdk_req*			m_sdkReq;
	std::string			m_strAppkey;
	std::string			m_strSecret;
	std::string			m_strHost;
	std::string			m_strPort;

private:
	int					m_nLastError;
	char				m_cLastError[25];

private:
	void				AddLog(char* tag, char* msg);

public:
	void				SetLastError(int error) { m_nLastError = error; }
	virtual int			GetLastError() { return m_nLastError; }


public:

	virtual int Login();

	virtual int Start(const char * textdata = NULL, unsigned int textlen = 0);

	virtual const char* GetResult(int * errorRet, int * retLength);

	virtual int QueryResult();

	//virtual int recognizer(char*pcm, int pcm_len) = 0;//used to recognize pcm data
	virtual int Probe(char*data=NULL, int data_len=0, bool is_last=false);

	virtual int Cancel();

	virtual int Stop() = 0;

	virtual int Logout();

	virtual int SetSerParam(char* key, char* value);

	virtual int SetTraParam(char* key, char* value);

public:

	virtual void SetSleepTimeMS(int timeMS);

	int SetValueInt(int id, int value);

	int SetValueString(int id, const char* s);

	const char* GetOptionValue(int id);

	virtual void SetServer(const char* ip, const unsigned short port);

public:

	bool DealEngineParam(std::string param);

};

#endif