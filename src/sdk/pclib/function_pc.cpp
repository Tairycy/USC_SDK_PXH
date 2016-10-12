#include "function_pc.h"
#include "getTime.h"
#include "Version.h"
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "string.h"

#ifdef LINUX
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h> 
#elif defined (WIN32)
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif

#ifdef WIN32
#define OS_NAME		"win"
#define DEVICE_OS  10
#elif LINUX
#define OS_NAME		"linux"
#define DEVICE_OS  11
#endif

#ifdef __x86_64__
#define CPU_BIT  "64"
#else
#define CPU_BIT  "32"
#endif


#ifdef LINUX
int get_mac(char* mac, int max_len)
{
	int fd = 0, intrface, retn = -1;
	struct ifreq buf[16];
	struct ifconf ifc;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t)buf;

		if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			while (intrface-- > 0)
			{
				//获取设备名称?
				if (!(ioctl(fd, SIOCGIFHWADDR, (char *)&buf[intrface])))
				{
					if ((unsigned)buf[intrface].ifr_hwaddr.sa_data[0] ||
						(unsigned)buf[intrface].ifr_hwaddr.sa_data[1] ||
						(unsigned char)buf[intrface].ifr_hwaddr.sa_data[2] ||
						(unsigned char)buf[intrface].ifr_hwaddr.sa_data[3] ||
						(unsigned char)buf[intrface].ifr_hwaddr.sa_data[4])
					{
						sprintf(mac, "LINUX-%02X%02X%02X%02X%02X%02X",
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[0],
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[1],
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[2],
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[3],
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[4],
							(unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]);
						close(fd);
						return 0;
					}
				}
			}
		}
		close(fd);
	}
	return retn;
}

#elif defined (WIN32)
int get_mac(char * mac, int max_len)
{
	NCB ncb;
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS   adapt;
		NAME_BUFFER   NameBuff[30];
	}ASTAT, *PASTAT;

	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{
		UCHAR   length;
		UCHAR   lana[MAX_LANA];
	}LANA_ENUM;

	LANA_ENUM lana_enum;
	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	for (int lana = 0; lana<lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
			break;
	}

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy((char*)ncb.ncb_callname, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb);

	if (uRetCode != NRC_GOODRET)
		return uRetCode;

	sprintf(mac, "WIN-%02X%02X%02X%02X%02X%02X",
		Adapter.adapt.adapter_address[0],
		Adapter.adapt.adapter_address[1],
		Adapter.adapt.adapter_address[2],
		Adapter.adapt.adapter_address[3],
		Adapter.adapt.adapter_address[4],
		Adapter.adapt.adapter_address[5]);

	return 0;
}
#endif


void InitNetwork()
{
#ifdef	WIN32
	WSAData wsaData;
	int nCode;
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0)
	{
		LOGMYE("ASR-JNI", "error initialize winsock.", 0);
		return;
	}
#endif
}

int get_client_info(char* info, int max_len, int last_error, char* service_info)
{
	memset(info, 0, max_len);
	sprintf(info, "%s%s:%d::::%s:%d:%s", OS_NAME, CPU_BIT, DEVICE_OS, USC_SDK_VERSION, last_error, service_info);
	return 0;
}
