// UscTtsDemo_singal.cpp : Defines the entry point for the console application.
// ���в������򽫴˹��̵�Դ�ļ��е���һ��cpp�Ƴ����ɡ�

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include <windows.h>
#include "tts_sdk.h"

//#pragma comment(lib , "../tts_api/Release/libusc_tts.lib")
 
using namespace std;
// ����ת������
int text_to_speech(TTSHANDLE handle, const char* src_text ,const char* des_path);

// pcm��ʽ��Ƶת��Ϊwav��ʽ
/*
����
pcm_name:ԭ�����ļ�
wav_name:��ת���������ļ�
dw_size:��������,Ĭ��16
dw_samples_per_sec:������,Ĭ��16000
ui_bits_per_sample:���ݸ�ʽλ��,Ĭ��16
*/
int pcm_to_wav(const char* pcm_name, const char* wav_name, const int dw_size,
        const int dw_samples_per_sec, const int ui_bits_per_sample);

int fun(const char* token);

int main(int argc, char* argv[])
{
	//fun("EHuubLb6JNW9oCyK03fcX0pL2qagP+tgDugjnGoJEc7aWge0c5WSNFMluXXqhroAVA8tS08nCqaMbfSfo+f0MP8wvA34NCYQ6OY541pb7cUks7JYHUCOhRsORrWBPY2b");
	int start1 = GetTickCount();

	// ����ļ���
	const char* pcm_filename = "text_to_speech_test.pcm";
	const char* wav_filename = "text_to_speech_test.wav";
	const char* imei = "671";

	int ret = 0;

	const char* test_file = "utf8.txt";

	ifstream input_fd(test_file);

	// �����ϳ�ʵ��
	TTSHANDLE handle = 0;

	//ret = usc_tts_create_service(&handle);

	//ret = usc_tts_create_service_ext(&handle, "10.10.11.2", "19090");
	ret = usc_tts_create_service_ext(&handle, "10.30.0.246", "8080");
	//ret = usc_tts_create_service_ext(&handle, "10.30.2.232", "8080");
	if ( ret != USC_SUCCESS )
	{
		printf("Login failed , Error code %d.\n",ret);
		return -1;
	}
	// ����appKey
	// const char * secret = "70b43842c4fabd5715fe5bad60a6e9b5";
	ret = usc_tts_set_option(handle, "appkey", "g6a6tnjq7j2kxbmlshkm3vhd576vypzdx2tt6vqk");
	ret = usc_tts_set_option(handle, "secret", "70b43842c4fabd5715fe5bad60a6e9b5");
	
	ret = usc_tts_set_option(handle, "audioFormat", "audio/x-wav");
	ret = usc_tts_set_option(handle, "audioCodec", "opus");
	ret = usc_tts_set_option(handle, "logLevel", "3");
	ret = usc_tts_set_option(handle, "logSwitch", "3");
	//ret = usc_tts_set_option(handle, "logOutput", "log.txt");

	//usc_tts_set_option(handle, "audioFormat", "audio/mpeg");
	//usc_tts_set_option(handle, "audioCodec", "raw");

	//ret = usc_tts_set_option(handle, "pit", "10");
	//ret = usc_tts_set_option(handle, "vol", "100");
	//ret = usc_tts_set_option(handle, "spd", "0");
	usc_tts_set_option(handle, "slp", "20");
	usc_tts_set_option(handle, "cryptVersion", "0");
	//usc_tts_set_option(handle, "md5", "true");
	//usc_tts_set_option(handle, "vcn", "");
	//usc_tts_set_option(handle, "vcn", "xiaoli");

	puts("login");
	// ���ӷ����������ϳ�
	ret = usc_tts_start_synthesis(handle);
	if (ret != USC_SUCCESS)
	{
		printf("usc_tts_start_synthesis: UscTts begin session failed Error code %d.\n", ret);
		usc_tts_release_service(handle);
		getchar();
		return -1;
	}
	printf("token : %s\n", usc_tts_get_option(handle, 206, NULL));

	usc_tts_set_option(handle, "ttsid14", "yunzhisheng-audiotest");

	string line;// = "Just for smart life.";
	int flag = 0;
	//while (getline(input_fd, line))
	while (getline(input_fd, line))
	//line = "Just for smart life!";
	//for (; flag < 10000; flag ++)
	{
		usc_tts_set_option(handle, "smt", "0");
		usc_tts_set_option(handle, "emt", "0");
		usc_tts_set_option(handle, "vcn", "xiaoli");
		char pcm_name[1000];
		sprintf(pcm_name, "%s%05d.pcm", pcm_filename, flag);
		//ret = text_to_speech(handle, line.c_str(), pcm_filename);
		ret = text_to_speech(handle, line.c_str(), pcm_name);
		if (ret != USC_SUCCESS)
		{
			printf("text_to_speech: failed , Error code %d.\n", ret);
		}
		flag++;
	}

	input_fd.close();

	// �ͷźϳɽӿ�ʵ��
	usc_tts_release_service(handle);

	// ������Ϊ��pcm�ļ�����Ϊwav��ʽ,�粻��Ҫ����ע�͵�
	/*
	ret = pcm_to_wav(pcm_filename, wav_filename, 16, 16000, 16);
	
	if (ret > 0)
	{
		printf("pcm_to_wav: success!\n");
	}
	else
	{
		printf("pcm_to_wav: error!\terror code : %d\n", ret);
	}
	*/
	printf("over\n");
	int end1 = GetTickCount();
	printf("times : %d\n", end1 - start1);
	getchar();
	return 0;
}

int text_to_speech(TTSHANDLE handle, const char* src_text ,const char* des_path)
{
	int ret = -1;
	unsigned int text_len = 0;
	unsigned int audio_len = 0;
	int synth_status = RECEIVING_AUDIO_DATA;
	ofstream output_fd;

	printf("begin to synth...\n");

	if (NULL == src_text || NULL == des_path)
	{
		printf("params is null!\n");
		return ret;
	}

	text_len = (unsigned int)strlen(src_text);

	output_fd.open(des_path, ios::binary);
	if (!output_fd.is_open())
	{
		printf("open file %s error\n",des_path);
		return ret;
	}

	puts("start");
	// �����ı�
	ret = usc_tts_text_put(handle, src_text, text_len);
	if ( ret != USC_SUCCESS )
	{
		printf("usc_tts_text_put: UscTts put text failed Error code %d.\n",ret);
		//usc_tts_stop_synthesis(handle);
		output_fd.close();
		return ret;
	}
	int errorCode = 0;
	printf("des : %s\tusc_tts_get_option : %s\n", des_path, usc_tts_get_option(handle, 26, &errorCode));

	int num_test = 0;
	while (1) 
	{
		num_test++;
		//if (num_test == 10)
		//	usc_tts_cancel(handle);

		audio_len = 0;
		// ��ȡ�ϳɽ��
		//Sleep(100);
		const void *data = usc_tts_get_result(handle, &audio_len, &synth_status, &ret);
		if (NULL != data)
		{
			output_fd.write((const char *)data, audio_len);
		}
		if (synth_status == AUDIO_DATA_RECV_DONE || ret != 0)
		{
			if (ret != 0)
			{
				printf("usc_tts_get_result error code =%d\n",ret);
				output_fd.close();
				return ret;
			}
			break;
		}
	}// �ϳ�״̬synth_statusȡֵ�ɲο������ĵ�

	puts("stop/logout");
	// ֹͣ��������
	ret = usc_tts_stop_synthesis(handle);
	if ( ret != USC_SUCCESS )
	{
		// ���ִ����˳�
		printf("usc_tts_stop_synthesis: UscTts end failed Error code %d.\n",ret);
	}
	/*
	else
	{
		const void *data = usc_tts_get_result(handle, &audio_len, &synth_status, &ret, true);
		if (NULL != data)
		{
			printf("last len : %d\n", audio_len);
			output_fd.write((const char *)data, audio_len);
		}		
	}
	*/
	output_fd.close();
	return ret;
}


int fun(const char* token)
{
	int start1 = GetTickCount();

	// ����ļ���
	const char* pcm_filename = "text_to_speech_test.pcm";
	const char* wav_filename = "text_to_speech_test.wav";
	const char* imei = "671";

	int ret = 0;

	const char* test_file = "utf8.txt";

	ifstream input_fd(test_file);

	// �����ϳ�ʵ��
	TTSHANDLE handle = 0;

	//ret = usc_tts_create_service(&handle);

	//ret = usc_tts_create_service_ext(&handle, "10.10.11.2", "19090");
	ret = usc_tts_create_service_ext(&handle, "10.30.0.246", "8080");
	//ret = usc_tts_create_service_ext(&handle, "10.30.2.232", "8080");
	if (ret != USC_SUCCESS)
	{
		printf("Login failed , Error code %d.\n", ret);
		return -1;
	}
	// ����appKey
	// const char * secret = "70b43842c4fabd5715fe5bad60a6e9b5";
	ret = usc_tts_set_option(handle, "appkey", "g6a6tnjq7j2kxbmlshkm3vhd576vypzdx2tt6vqk");
	ret = usc_tts_set_option(handle, "secret", "70b43842c4fabd5715fe5bad60a6e9b5");

	ret = usc_tts_set_option(handle, "audioFormat", "audio/x-wav");
	ret = usc_tts_set_option(handle, "audioCodec", "opus");
	//ret = usc_tts_set_option(handle, "logLevel", "2");
	//ret = usc_tts_set_option(handle, "logSwitch", "2");
	//ret = usc_tts_set_option(handle, "logOutput", "log.txt");

	//usc_tts_set_option(handle, "audioFormat", "audio/mpeg");
	//usc_tts_set_option(handle, "audioCodec", "raw");

	//ret = usc_tts_set_option(handle, "pit", "10");
	//ret = usc_tts_set_option(handle, "vol", "100");
	//ret = usc_tts_set_option(handle, "spd", "0");
	//usc_tts_set_option(handle, "slp", "50");
	//usc_tts_set_option(handle, "cryptVersion", "0");
	//usc_tts_set_option(handle, "md5", "true");
	//usc_tts_set_option(handle, "vcn", "");
	usc_tts_set_option(handle, "vcn", "xiaoli");
	usc_tts_set_option(handle, "token", token);

	puts("login");
	// ���ӷ����������ϳ�
	/*
	ret = usc_tts_start_synthesis(handle);
	if (ret != USC_SUCCESS)
	{
		printf("usc_tts_start_synthesis: UscTts begin session failed Error code %d.\n", ret);
		usc_tts_release_service(handle);
		getchar();
		return -1;
	}
	*/

	string line;// = "Just for smart life.";
	int flag = 0;
	//while (getline(input_fd, line))
	while (getline(input_fd, line))
		//line = "Just for smart life!";
		//for (; flag < 10000; flag ++)
	{
		char pcm_name[1000];
		sprintf(pcm_name, "%s%05d.pcm", pcm_filename, flag);
		//ret = text_to_speech(handle, line.c_str(), pcm_filename);
		ret = text_to_speech(handle, line.c_str(), pcm_filename);
		if (ret != USC_SUCCESS)
		{
			printf("text_to_speech: failed , Error code %d.\n", ret);
		}
	}

	input_fd.close();

	// �ͷźϳɽӿ�ʵ��
	usc_tts_release_service(handle);

	// ������Ϊ��pcm�ļ�����Ϊwav��ʽ,�粻��Ҫ����ע�͵�

	ret = pcm_to_wav(pcm_filename, wav_filename, 16, 16000, 16);

	if (ret > 0)
	{
		printf("pcm_to_wav: success!\n");
	}
	else
	{
		printf("pcm_to_wav: error!\terror code : %d\n", ret);
	}

	printf("over\n");
	int end1 = GetTickCount();
	printf("times : %d\n", end1 - start1);
	getchar();
	return 0;
}

#define     UNIT        4
#define     WORD        short
#define     DWORD       int
// wav�����ļ�ͷ��
struct RIFF_HEADER
{
    char szRiffID[4];  // 'R','I','F','F'
    DWORD dwRiffSize;
    char szRiffFormat[4]; // 'W','A','V','E'
};

struct WAVE_FORMAT
{
    WORD wFormatTag;
    WORD wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD wBlockAlign;
    WORD wBitsPerSample;
};

struct FMT_BLOCK
{
    char  szFmtID[4]; // 'f','m','t',' '
    DWORD  dwFmtSize;
    WAVE_FORMAT wavFormat;
};

struct DATA_BLOCK
{
    char szDataID[4]; // 'd','a','t','a'
    DWORD dwDataSize;
};

union DWORD_CHAR
{
    int  nValue;
    char charBuf[4];
};
// д���ļ�ͷ��
int writeWaveHead(FILE *fp, const int dw_size, const int dw_samples_per_sec,
                    const int ui_bits_per_sample);
// �����ļ�����ƴ��
int writeWaveBody(FILE *fp,long filelength);

bool writeFile2Int(FILE *fp,int nWhere,int nValue)
{
    if(fp==NULL)
    {
        return false;
    }
    fseek(fp,nWhere,SEEK_SET);
    DWORD_CHAR dc;
    dc.nValue=nValue;
    fwrite(dc.charBuf,1,4,fp);      
    return true;
}

int writeWaveHead(FILE *fp, const int dw_size, const int dw_samples_per_sec,
                    const int ui_bits_per_sample)
{
    if (NULL == fp)
    {
        return -1;
    }
    if (0 > dw_size || 0 > dw_samples_per_sec || 0 > ui_bits_per_sample)
    {
        return -2;
    }
    //дWAV�ļ�ͷ
    RIFF_HEADER rh;
    memset(&rh,0,sizeof(rh));
    strncpy(rh.szRiffFormat,"WAVE",4);
    strncpy(rh.szRiffID,"RIFF",4);

    fwrite(&rh,1,sizeof(rh),fp);

    FMT_BLOCK fb;
    strncpy(fb.szFmtID,"fmt ",4);
    fb.dwFmtSize = dw_size;
    fb.wavFormat.wFormatTag = 0x0001;
    fb.wavFormat.wChannels = 1;
    fb.wavFormat.wBitsPerSample = ui_bits_per_sample;       
    fb.wavFormat.dwSamplesPerSec = dw_samples_per_sec;       
    fb.wavFormat.wBlockAlign = fb.wavFormat.wChannels*fb.wavFormat.wBitsPerSample/8;   //4;
    fb.wavFormat.dwAvgBytesPerSec = fb.wavFormat.dwSamplesPerSec * fb.wavFormat.wBlockAlign;
    fwrite(&fb,1,sizeof(fb),fp);
    char buf[]={"data0000"};
    fwrite(buf,1,sizeof(buf),fp);
    return 1;
}

int writeWaveBody(FILE *fp, long filelength)
{
    if (NULL == fp)
    {
        return -1;
    }
    if (0 > filelength)
    {
        return -2;
    }
    //����WAV�ļ�dwRiffSize�ֶ��е�ֵ
    int nWhere = 4;
    writeFile2Int(fp,nWhere, filelength - 8);

    //����WAV�ļ�DataChunk��Size�ֶε�ֵ
    nWhere=sizeof(RIFF_HEADER)+sizeof(FMT_BLOCK)+4;
    writeFile2Int(fp,nWhere,filelength - (sizeof(RIFF_HEADER)+sizeof(FMT_BLOCK)+8) );
    return 1;
}

int pcm_to_wav(const char* pcm_name, const char* wav_name, const int dw_size = 16,
        const int dw_samples_per_sec = 16000, const int ui_bits_per_sample = 16)
{
    if (NULL == pcm_name || NULL == wav_name || 0 > dw_size ||
        0 > dw_samples_per_sec || 0 > ui_bits_per_sample)
    {
        // ����Ĳ���
        return -1;
    }

    FILE *fpS;
    FILE *fpD;
    fpS = fopen(pcm_name, "rb");
    fpD = fopen(wav_name, "wb+");

    if(fpS==NULL || fpD==NULL)
    {
    	// �ļ��򿪴���
        return -2;
    }
    fseek(fpS, 0, SEEK_END);
    long filelength = ftell(fpS);

    int ret = 0;
    ret = writeWaveHead(fpD, dw_size, dw_samples_per_sec, ui_bits_per_sample);
    if (ret < 0)
    {
    	// �ļ�ͷ��д�����
        return -3;
    }
    ret = writeWaveBody(fpD,filelength);
    if (ret < 0)
    {
    	// ��������ƴ�Ӵ���
        return -4;
    }

    fseek(fpS,44,SEEK_SET);
    
    char buf[UNIT];
    while(UNIT==fread(buf,1,UNIT,fpS))
    {
        fwrite(buf,1,UNIT,fpD);
    }
    fclose(fpS);
    fclose(fpD);

    return 1;
}
