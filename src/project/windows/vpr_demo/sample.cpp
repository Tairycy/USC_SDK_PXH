// sample.cpp : 定义控制台应用程序的入口点。
//

#include <string.h>
#include <stdio.h>
#include "libusc_vpr.h"
#include <string>
#include <stdlib.h>
#include <time.h>

int fun(const char* token);

int main(int argc, char* argv[])
{
	//const char * host = "10.20.11.13";
	//const short port = 19090;
	//const char * host = "10.10.11.11";
	//const short port = 10006;
	//const char * host = "10.30.0.246";
	//const short port = 8080;

	//if(argc <= 8) {
	//	printf("argc error!\tip port vp/vpr username pcmfile outputfile appkey secret\n");
	//	return -5;
	//}
	//
	//const char * host = argv[1];
	//const char * portStr = argv[2];
	//int port = atoi(portStr);
	//bool isVpr = false;
	//const char * vprStr = argv[3];
	//if (strcmp(vprStr, "vpr") == 0)
	//	isVpr = true;
	//const char* username;
	//if (strcmp(argv[4], "null") == 0)
	//	username = "";
	//else 
	//	username= argv[4];
	//const char * pcmFileName = argv[5];
	//const char * resultFileName = argv[6];
	//const char * appKey;
	//if (strcmp(argv[7], "null") == 0)
	//	appKey = "";
	//else 
	//	appKey= argv[7];
	//const char * secret = argv[8];

	//int num_pcm = 1;
	//if (argc > 9)
	//	num_pcm = atoi(argv[9]);	

	bool isVpr = false;
	//const char * host = "10.30.0.246";
	//int port = 8080;
	const char * host = "vpr.hivoice.cn";
	int port = 80;
	const char * vprStr = "vpr";
	if (strcmp(vprStr, "vpr") == 0)
		isVpr = true;
	const char* username = "joe3251245039342875192";
	const char * pcmFileName = "16k_long.wav";
	const char * resultFileName = "ans.txt";
	const char * appKey = "g6a6tnjq7j2kxbmlshkm3vhd576vypzdx2tt6vqk";
	const char * secret = "70b43842c4fabd5715fe5bad60a6e9b5";
	int num_pcm = 1;

	//printf("username : %s\nappkey : %s\n", username, appKey);

	//const char * pcmFileName = "login.pcm";
	//const char * resultFileName = "ans.txt";

	int iii = 1;
	for (iii=0; iii<num_pcm; iii++)
	{	
		std::string result = "";
		char buff[640];
			
		char *audio_format = AUDIO_FORMAT_PCM_16K;

		// 打开结果保存文件(UTF-8)
		FILE* fresult = fopen(resultFileName, "a+");
		if( fresult == NULL) {
			fprintf(stderr,"Cann't Create File %s\n", resultFileName);
			return -2;
		}
		// 打开语音文件(16K/8K 16Bit pcm)
		FILE* fpcm = fopen(pcmFileName, "rb");
		if( fpcm == NULL) {
			fprintf(stderr,"Cann't Open File %s\n", pcmFileName);
			return -3;
		}
		time_t t;
		time(&t);

		char num_flag[100];
		sprintf(num_flag, "\t%05d\t", iii);

		fprintf(stderr,pcmFileName);
	//	fprintf(stderr, num_flag);
	//	fprintf(stderr,asctime(gmtime(&t)));

	//	fprintf(fresult,asctime(gmtime(&t)));
	//	fprintf(fresult, num_flag);
		fprintf(fresult,pcmFileName);
		
		// 创建识别实例
		VPRHANDLE handle;
		int ret = usc_vpr_create_service_ext(&handle, host, port);
		//int ret = usc_vpr_create_service(&handle);
		if(ret != USC_VPR_OK) {
			fprintf(stderr,"usc_vpr_create_service_ext error %d\n", ret);
			return -4;
		}

		// 设置识别AppKey
		ret = usc_vpr_set_option(handle, USC_VPR_OPT_APP_KEY, appKey);
		ret = usc_vpr_set_option(handle, USC_VPR_OPT_USER_SECRET, secret);
		//ret = usc_vpr_set_option(handle, 203, "true");
		ret = usc_vpr_set_option(handle, 205, "0");
		
		// 设置输入语音的格式
		ret = usc_vpr_set_option(handle, USC_VPR_OPT_INPUT_AUDIO_FORMAT, audio_format);

		char UN[100];
		sprintf(UN, "%s%05d", username, iii);
		
		std::string trafficStr("filterName=vpr;userName=");
		//trafficStr += UN;
		trafficStr += username;
		if (isVpr)
			trafficStr += ";type=matchSingle;returnType=json;";
		else
			trafficStr += ";type=register;returnType=json;";

		// 设置 traffic parameter
		ret = usc_vpr_set_option(handle, USC_VPR_OPT_TRAFFIC_PARAMETER, trafficStr.c_str());

		ret = usc_vpr_start_recognizer(handle);
		if(ret != USC_VPR_OK )
		{
			fprintf(stderr,"usc_vpr_start_recognizer error %d\n", ret);
			goto usc_vpr_release;
		}
		ret = usc_vpr_set_option(handle, 203, "false");
		
		while(true){

			int nRead = fread(buff,sizeof(char), sizeof(buff),fpcm);
			if(nRead <= 0) {
				break;
			}
			// 传入语音数据
			ret = usc_vpr_feed_buffer(handle, buff, nRead);
			if( ret < 0) {
				// 网络出现错误退出
				fprintf(stderr,"usc_vpr_feed_buffer error %d\n", ret);
				goto usc_vpr_release;
			}
		}
		
		// 停止语音输入
		ret = usc_vpr_stop_recognizer(handle);
		if(ret == 0) {

			std::string resultTmp = usc_vpr_get_result(handle);
			//result.append(usc_vpr_get_result(handle));
			result.append(resultTmp);

			// 获取剩余识别结果
			fprintf(fresult, "%s\n",result.c_str()); 
			fprintf(stderr, "%s\n",result.c_str());
		}
		else {
			// 网络出现错误退出
			fprintf(stderr,"usc_vpr_stop_recognizer error %d\n", ret);
		}

usc_vpr_release:

		if (ret != 0)
		{
			fprintf(stderr, "session ID : %s\n", usc_vpr_get_option(handle, 21));
			fprintf(fresult, "session ID : %s\n", usc_vpr_get_option(handle, 21));
		}
		usc_vpr_release_service(handle);	
		
		if( ret != 0) {
			// 识别出现错误
			fprintf(fresult,"!!!!!!!!--------> Error %d <--------!!!!!!!!!!!\n", ret);
		}

		fclose(fresult);
		fclose(fpcm);
	}

	getchar();
	return 0;
}


