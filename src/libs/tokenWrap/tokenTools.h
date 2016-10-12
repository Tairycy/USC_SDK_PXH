#ifndef __TOKEN_TOOLS_H__
#define __TOKEN_TOOLS_H__

typedef long long TNHANDLE;

#ifdef __cplusplus
extern "C"{
#endif

int createTokenService(TNHANDLE* handle, const char* token, const char* appKey, unsigned long time = 0);

// genNewToken �������ڴ�ռ䣬�ǵ��ͷš�
const char* genNewToken(TNHANDLE handle, unsigned int* len);

int releaseTokenService(TNHANDLE handle);

#ifdef __cplusplus
}
#endif

#endif
