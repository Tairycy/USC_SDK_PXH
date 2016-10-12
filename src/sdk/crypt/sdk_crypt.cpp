#include "sdk_crypt.h"
#include "function.h"
#include "http_def.h"
#include "errorNo.h"
#include "getTime.h"

#include <stdlib.h>

// check the header is our need to decrypt/encrypt
static int CheckHeader(char* key);

// get md5 needed malloc length
static int GetMallocLen(int secretLen, std::map<std::string, std::string>* map);

// check md5 legal success
static int SetMallocMemory(char *ch_md5, char* secret, std::map<std::string, std::string>* map);


sdk_crypt::sdk_crypt()
	:m_bIsNeedMd5(false), m_strSecret(""), m_pBody(NULL), m_nBodyLen(0)
{
}

sdk_crypt::~sdk_crypt()
{
	m_mapReqHeaders.clear();
}

char* sdk_crypt::EnCrypt(char* str)
{
	return m_crypt.encrypt(str);
}

char* sdk_crypt::DeCrypt(char* str)
{
	return m_crypt.decrypt(str);
}

int sdk_crypt::clean()
{
	m_mapReqHeaders.clear();
	m_mapRespHeaders.clear();
	m_crypt.clear();
	m_pBody = NULL;
	m_bIsNeedMd5 = false;
	m_strSecret = "";
	m_nBodyLen = 0;
	memset(m_nArrRandom, 0, sizeof(m_nArrRandom));
	return 0;
}

std::string sdk_crypt::GetMd5Str()
{
	// get the malloc len
	int malloc_len = GetMallocLen(m_strSecret.length(), &m_mapReqHeaders);
	// malloc memory
	char* ch_md5 = (char*)malloc(malloc_len + 1);
	SetMallocMemory(ch_md5, (char*)m_strSecret.c_str(), &m_mapReqHeaders);
	//printf("\nch_md5 : \n%s\n", ch_md5);
	// get md5 string
	char md5_ans[100];
	GetMd5(ch_md5, md5_ans);
	free(ch_md5);
	std::string str_md5 = md5_ans;
	return str_md5;
}

int sdk_crypt::SetUseMd5(bool is_use_md5)
{
	return m_bIsNeedMd5 = is_use_md5;
}

int	sdk_crypt::SetCryptVersion(int version)
{
	return m_crypt.setVersion(version);
}

int sdk_crypt::SetSecret(const char* secret)
{
	m_strSecret = secret;
	return 0;
}

int sdk_crypt::SetHttpHeader(const char* key, const char* value)
{
	if (NULL == key || NULL == value)
		return -1;
	m_mapReqHeaders.erase((std::string)key);
	m_mapReqHeaders.insert(std::pair<std::string, std::string>((std::string)key, (std::string)EnCrypt((char*)value)));
	return 0;
}

int sdk_crypt::SetHttpHeader(std::string key, std::string value)
{
	m_mapReqHeaders.erase((std::string)key);
	m_mapReqHeaders.insert(std::pair<std::string, std::string>(key, EnCrypt((char*)value.c_str())));
	return 0;
}

int sdk_crypt::DeleteHttpHeader(const char* key)
{
	m_mapReqHeaders.erase((std::string)key);
	return 0;
}

int sdk_crypt::SetHttpBody(char* body, int bodyLen)
{
	m_pBody = body;
	m_nBodyLen = bodyLen;
	return 0;
}

int	sdk_crypt::ClearRequest()
{
	m_mapReqHeaders.clear();
	m_pBody = NULL;
	m_nBodyLen = 0;
	return 0;
}

int sdk_crypt::PrepareHttpRequest(HttpRequestInfo* hri, int flag)
{
	int ret = 0;
	if (NULL == hri)
		return -1;
	if (m_bIsNeedMd5 && flag >= 0)
	{
		m_nArrRandom[flag] = abs(GetRandomNum())%1000000;
		char tmpR[10];
		sprintf(tmpR, "%d", m_nArrRandom[flag]);
		m_mapReqHeaders.erase((std::string)H_RANDOM_NUM);
		m_mapReqHeaders.insert(std::pair<std::string, std::string>((std::string)H_RANDOM_NUM, (std::string)EnCrypt(tmpR)));
		m_mapReqHeaders.erase((std::string)H_MD5);
		m_mapReqHeaders.insert(std::pair<std::string, std::string>((std::string)H_MD5, GetMd5Str()));
		if (m_strSecret.length() <= 0)
			return NO_SECRET_ERROR;
	}
	char crypt_version[100];
	sprintf(crypt_version, "%d", m_crypt.getVersion());
	m_mapReqHeaders.erase((std::string)H_CRYPT_VERSION);
	m_mapReqHeaders.insert(std::pair<std::string, std::string>((std::string)H_CRYPT_VERSION, (std::string)crypt_version));
	hri->headers = m_mapReqHeaders;
	hri->appendBody(m_pBody, m_nBodyLen);
	//for (std::map<std::string, std::string>::iterator it = m_mapReqHeaders.begin();
	//	it != m_mapReqHeaders.end(); it++)
	//{
	//	ghttp_set_header(m_pRequest, (char*)it->first.c_str(), (char*)it->second.c_str());
	//	ret++;
	//}
	return ret;
}

int	sdk_crypt::CheckHttpResponse(HttpResponseInfo* hrpi, int flag)
{
	int ret = 0;
	if (NULL == hrpi)
		return -1;
	m_mapRespHeaders.clear();
	//m_mapRespHeaders = hrpi->headers;
	for (std::map<std::string, std::string>::iterator it = hrpi->headers.begin();
		it != hrpi->headers.end(); it++)
	{
		m_mapRespHeaders.insert(std::pair<std::string, std::string>(it->first, it->second));
	}
	if (flag >= 0)
	{
		int random_pat = atoi(DeCrypt((char*)m_mapRespHeaders[H_RANDOM_NUM].c_str()));
		if (random_pat != m_nArrRandom[flag])
		{
			return CHECK_MD5_ERROR;
		}
	}
	if (m_bIsNeedMd5 && flag >= 0)
	{
		std::string md5_pat = m_mapRespHeaders[H_MD5];
		int malloc_len = GetMallocLen(m_strSecret.length(), &m_mapRespHeaders);
		char* ch_md5 = (char*)malloc(malloc_len + 1);
		SetMallocMemory(ch_md5, (char*)m_strSecret.c_str(), &m_mapRespHeaders);
		char md5_ans[100];
		GetMd5(ch_md5, md5_ans);
		free(ch_md5);
		if (strcmp(md5_ans, md5_pat.c_str()) == 0)
			return 0;
		else
			return CHECK_MD5_ERROR;
	}
	return 0;
}

static int CheckHeader(char* key)
{
	int i = 0;
	if (key == NULL)
		return -2;
	if (strlen(key) <= 3)
		return 0;
	return -1;
}


static int GetMallocLen(int secretLen, std::map<std::string, std::string>* map)
{
	if (NULL == map)
		return -1;
	int malloc_len = 0;
	malloc_len += secretLen;
	for (std::map<std::string, std::string>::iterator it = map->begin();
		it != map->end(); it++)
	{
		if (CheckHeader((char*)it->first.c_str()) == 0)
		{
			malloc_len += it->second.length();
		}
	}
	//if (body != NULL && bodyLen > 0)
	//	malloc_len += bodyLen;
	return malloc_len;
}

static int SetMallocMemory(char *ch_md5, char* secret, std::map<std::string, std::string>* map)
{
	if (NULL == ch_md5 || NULL == secret || NULL == map)
		return -1;
	int offset = 0;
	memcpy(ch_md5 + offset, (const void*)secret, strlen(secret));
	offset += strlen(secret);
	for (std::map<std::string, std::string>::iterator it = map->begin();
		it != map->end(); it++)
	{
		if (CheckHeader((char*)it->first.c_str()) == 0)
		{
			memcpy(ch_md5 + offset, (const void*)it->second.c_str(), it->second.length());
			offset += it->second.length();
			//printf("\n%s\n%s\n", it->first.c_str(), it->second.c_str());
		}
	}
	ch_md5[offset] = '\0';
	return offset;
}
