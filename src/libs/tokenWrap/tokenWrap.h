#ifndef __TOKEN_WRAP_H__
#define __TOKEN_WRAP_H__

namespace TokenWrap
{

#ifdef __cplusplus
extern "C" {
#endif	
	const char* encyptToken(const char* origToken, unsigned int inLen, const char* secret);
	const char* decyptToken(const char* token, unsigned int* inLen, const char* secret);
	unsigned long getTokenTimeStamp(const char* tokenBase64, unsigned int inLen, const char* secret);
#ifdef __cplusplus
}
#endif

}

#endif
