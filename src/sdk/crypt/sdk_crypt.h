#ifndef __SDK_CRYPT_H__
#define __SDK_CRYPT_H__

#include <string>
#include <map>
#include "crypt.h"
#include "HttpRequestInfo.h"

#ifdef __cplusplus
extern "C"
{
#endif

class sdk_crypt
{
public:
	sdk_crypt();
	~sdk_crypt();
	int									m_nCryptVersion;
	bool								m_bIsNeedMd5;
	std::string							m_strSecret;
	CLCrypt								m_crypt;
	std::map<std::string, std::string>	m_mapReqHeaders;
	std::map<std::string, std::string>	m_mapRespHeaders;
	char*								m_pBody;
	int									m_nBodyLen;
	int									m_nArrRandom[1000];

public:

	int				clean();
	char*			EnCrypt(char* str);
	char*			DeCrypt(char* str);
	std::string		GetMd5Str();

	int				SetUseMd5(bool is_use_md5 = true);
	int				SetCryptVersion(int version = 1);
	int				SetSecret(const char* secret);
	int				DeleteHttpHeader(const char* key);
	int				SetHttpHeader(const char* key, const char* value);
	int				SetHttpHeader(std::string key, std::string value);
	int				SetHttpBody(char* body, int bodyLen);
	int				ClearRequest();

	// flag == -1 is login & logout, flag >= 0 is request & probe & normalend
	int				PrepareHttpRequest(HttpRequestInfo* hri, int flag);
	int				CheckHttpResponse(HttpResponseInfo* hrpi, int flag);

private:

};

#ifdef __cplusplus
}
#endif

#endif//__CRYPT_H__

