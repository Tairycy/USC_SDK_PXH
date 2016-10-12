// sample.cpp : 定义控制台应用程序的入口点。
//

#include <string.h>
#include <stdio.h>
#include "libusc.h"
#include "windows.h"
#include<string>
#include "set_option_key.h"


int main(int argc, char* argv[])
{
	printf("!!!!!!!!!!!!!!!!!!!\n");
	//const char * host = "10.20.11.13";//"tr.hivoice.cn";
	//const short port = 19090;
	//const char * host = " 120.132.147.120";
	//const short port = 80;
	const char * host = "tr.hivoice.cn";
	const short port = 80;
	//const char * host = "10.30.0.246";
	//const short port = 8080;
	//const char * host = "10.10.11.2";
	//const short port = 19090;
	const char * pcmFileName = "test001.pcm";
	//const char * pcmFileName = "16k_long.wav";
	const char * resultFileName = "ans.txt";
	std::string result = "";
	std::string tmp = "";
	std::string token;
	char buff[640];

	//char *audio_format = "pcm16knoen";
	char *audio_format = AUDIO_FORMAT_PCM_16K;
	if(argc > 2) {
		audio_format = argv[3];
	}

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

	fprintf(stderr,pcmFileName);
	fprintf(fresult,pcmFileName);
	
	// 创建识别实例
	USC_HANDLE handle;
	int ret = usc_create_service_ext(&handle, host, port);
	//int ret = usc_create_service(&handle);
	if(ret != USC_ASR_OK) {
		fprintf(stderr,"usc_create_service_ext error %d\n", ret);
		goto close_files;
	}
	//ret = usc_set_option(handle, 208, "2");
	//ret = usc_set_option(handle, 209, "3");
	// 设置识别AppKey
	ret = usc_set_option(handle, USC_OPT_APP_KEY, "g6a6tnjq7j2kxbmlshkm3vhd576vypzdx2tt6vqk");
	//ret = usc_set_option(handle, USC_OPT_USER_ID, "yunzhisheng-audiotest");
	
	ret = usc_set_option(handle, USC_OPT_CRYPT_VERSION, "0");
	// 设置输入语音的格式
	ret = usc_set_option(handle, USC_OPT_INPUT_AUDIO_FORMAT, audio_format);
		
	ret = usc_set_option(handle, USC_OPT_ASR_ENGINE_PARAMETER, "sampleRate:16k\n");
	//ret = usc_set_option(handle, USC_OPT_ASR_ENGINE_PARAMETER, "lang=cn");

	ret = usc_set_option(handle, USC_OPT_RECOGNITION_FIELD, "general");
	// = usc_set_option(handle, 105, "true");
	//ret = usc_set_option(handle, 202, "250");
	//ret = usc_set_option(handle, USC_OPT_NLU_PARAMETER, "filterName=search;returnType=json;city=上海;");
	ret = usc_login_service(handle);
	if (ret != USC_ASR_OK)
	{
		fprintf(stderr, "usc_login_service error %d\n", ret);
		goto usc_release;
	}
	token = usc_get_option(handle, 206);
	printf("token : %s\n", token.c_str());

	//ret = usc_set_option(handle, USC_SERVICE_STATUS_SELECT, "8");
	//ret = usc_set_option_str(handle, "textformat", "json");
	//ret = usc_set_option(handle, USC_OPT_NLU_PARAMETER, "filterName=vpr,search;userName=671coder;type=matchSingle;returnType=json;city=上海;");
	//ret = usc_set_option(handle, USC_OPT_NLU_PARAMETER, "filterName=vpr,search;userName=671coder2;type=register;returnType=json;");
	
	ret = usc_start_recognizer(handle);
	if (ret != USC_ASR_OK)
	{
		fprintf(stderr,"usc_start_recognizer error %d\n", ret);
		goto usc_release;
	}
	
	int num = 0;
	
	while(true){

		int nRead = fread(buff,sizeof(char), sizeof(buff),fpcm);
		//printf("NO : %4d\tread : %d\n", num++, nRead);
		if(nRead <= 0) {
			break;
		}
		// 传入语音数据
		ret = usc_feed_buffer(handle, buff, nRead);
		//Sleep(10);
		//printf("request\n");

		if(ret == USC_RECOGNIZER_PARTIAL_RESULT ||
		   ret == USC_RECOGNIZER_SPEAK_END) {
			std::string tmp = usc_get_result(handle);
			printf("parital : %s\n", tmp.c_str());
			fprintf(fresult, "parital ans : %s\n", tmp.c_str());
			result.append(tmp);
		}
		else if( ret < 0) {

			// 网络出现错误退出
			fprintf(stderr,"usc_feed_buffer error %d\n", ret);
			goto usc_release;
		}

		const char* tmp = usc_get_option(handle, 34);
		if (tmp != NULL && strlen(tmp) > 0)
		{
			printf("tmp ans : %s\n", tmp);
			fprintf(fresult, "tmp ans : %s\n", tmp);
		}
	}
	
	// 停止语音输入
	ret = usc_stop_recognizer(handle);
	if(ret == 0) {

		// 获取剩余识别结果
		result.append(usc_get_result(handle));
		fprintf(fresult, "%s\n",result.c_str()); 
		fprintf(stderr, "%s\n",result.c_str());
	}
	else {
		// 网络出现错误退出
		fprintf(stderr,"usc_stop_recognizer error %d\n", ret);
	}

	tmp = usc_get_option(handle, 21);
	printf("\n\n%s\n\n", tmp.c_str());
usc_release:

	usc_release_service(handle);	
	
	if( ret != 0) {
		// 识别出现错误
		fprintf(fresult,"!!!!!!!!--------> Error %d <--------!!!!!!!!!!!\n", ret);
	}

close_files:

	fclose(fresult);
	fclose(fpcm);

	getchar();
	//for (int i = 0; i < 100; i++)

	return ret;
}
