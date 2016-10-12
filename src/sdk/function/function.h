#ifndef __FUNCIONT_H__
#define __FUNCIONT_H__

#include <string>

int GetRandomNum();
int TransError2User(int errorCode);
void usc_sleep(int milliseconds);
std::string GetSystemTimeS_Y_M_D();
std::string GetSystemTimeS();
std::string GetSystemTimeTick();
std::string GetUuid();
// get md5 number
int GetMd5(char* str, char* decrypt_ans);
int GetBitOfInt(unsigned int value, int bit);



// device OS
typedef enum {
	DEVICE_OS_Android = 0,
	DEVICE_OS_iOS = 1,
	DEVICE_OS_WP8 = 2,
}DEVICE_OS;

// network type
typedef enum {
	NETWORK_TYPE_NONE = 0,
	NETWORK_TYPE_WIFI = 1,
	NETWORK_TYPE_3G = 2,
	NETWORK_TYPE_2G = 3,
	// iOS无法区分2G还是3G
	NETWORK_TYPE_WWAN = 4,
}NETWORK_TYPE;



#endif