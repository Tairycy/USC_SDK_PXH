#include "libusc_vpr.h"
#include "libusc.h"
#include "set_option_key.h"
#include <stdlib.h>
#include <string.h>
#include <string>

USC_API
int usc_vpr_create_service(USC_HANDLE* handle)
{
	return usc_create_service(handle);
}


USC_API
int usc_vpr_create_service_ext(USC_HANDLE* handle, const char* host, const unsigned short port)
{
	return usc_create_service_ext(handle, host, port);
}

USC_API
int usc_vpr_login_service(USC_HANDLE handle)
{
	return usc_login_service(handle);
}

USC_API
void usc_vpr_vad_set_timeout(USC_HANDLE handle, int frontSil,int backSil)
{
	usc_vad_set_timeout(handle, frontSil, backSil);
}


USC_API
int usc_vpr_start_recognizer(USC_HANDLE handle)
{
	int ret;
	ret = usc_set_option(handle, USC_SERVICE_STATUS_SELECT, "8");
	ret = usc_set_option(handle, OPT_COLLECTED_INFO, "vpr");
	ret = usc_set_option(handle, OPT_USE_MD5, "true");
	if (ret != 0)
		return ret;
	return usc_start_recognizer(handle);
}


USC_API
int usc_vpr_stop_recognizer(USC_HANDLE handle)
{
	return usc_stop_recognizer(handle);
}


USC_API
int usc_vpr_feed_buffer(USC_HANDLE handle, const char* buffer, int len)
{
	int ret = usc_feed_buffer(handle, buffer, len);

	if(ret == USC_VPR_RECOGNIZER_PARTIAL_RESULT ||
		ret == USC_VPR_RECOGNIZER_SPEAK_END)
	{
		std::string resultTmp = usc_get_result(handle);
	}
	return ret;
}


USC_API
const char* usc_vpr_get_result(USC_HANDLE handle)
{
	// ´¦Àí×Ö·û´®
	return usc_get_option(handle, 51);
}


USC_API
void usc_vpr_release_service(USC_HANDLE handle)
{
	return usc_release_service(handle);
}


USC_API
int usc_vpr_set_option(USC_HANDLE handle, int option_id, const char* value)
{
	return usc_set_option(handle, option_id, value);
}


USC_API
int usc_vpr_set_option_str(USC_HANDLE handle, const char* key, const char* value)
{
	return usc_set_option_str(handle, key, value);
}


USC_API
const char* usc_vpr_get_version()
{
	return usc_get_version();
}


USC_API
const char * usc_vpr_get_option(USC_HANDLE handle, int option_id)
{
	return usc_get_option(handle, option_id);
}

USC_API
void usc_vpr_clear_option(USC_HANDLE handle, int option_id)
{
	return usc_clear_option(handle, option_id);
}
