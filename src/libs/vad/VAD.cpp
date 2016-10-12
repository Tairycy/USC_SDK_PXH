// VAD.cpp : Defines the entry point for the console application.
//


#include "AsrVad.h"
//#include "windows.h"
#define FRM_MS	25
#define SFT_MS	10
// extern DWORD clock_1;
// extern DWORD clock_2;
// extern DWORD clock_3;
// extern DWORD clock_4;

#ifndef	WIN32
#include <sys/time.h>
struct timeval tv1,tv2;
static void getCountStart()
{
	gettimeofday (&tv1 , NULL);
}
static long getCount()
{
	long interval;
	gettimeofday (&tv2 , NULL);
	interval = tv2.tv_usec - tv1.tv_usec;
	if(interval < 0)
		interval += 1000000;
	tv1 = tv2;
	return interval;
}
#else
static void getCountStart()
{
}
static long getCount()
{
	return 0;
}
#endif

#define BLOCK 1600
#define SIL_TIMEOUT	300
int main(int argc, char* argv[])
{
	printf("Hello World!\n");
	short buf[BLOCK];
	FILE* fp = fopen( argv[1],"rb" );
	if( fp==NULL ){
		printf( "%s not found\n",argv[1] );
		return 0;
	}

	char epd[256];
	strcpy( epd,argv[1] );
	printf( "%s\n",epd );
	fseek( fp,0L,SEEK_END );
	int len = (ftell(fp)-44L)/sizeof(short);
	printf("%d\n",len);
//	printf( "%d %d\n",nFrmLen,nSftLen );
//	EPD* pEPD = new EPD( nFrmLen,nSftLen,60,8 );
//	Result res;
//	Spectrum frame(nFrmLen,nSftLen);
//	short* pT = frame.audio_buffer.getDataBuf();
	fseek( fp,44L,SEEK_SET );
//	int prevType,nowType;
//	nowType = EPD::EPD_INIT;
//	int Offset = pEPD->GetOffset();
//	short* buf = (short* )calloc( nSftLen*Offset,sizeof(short) );//30
	int samNum = 0;
	int ret = 0;
	AsrVAD* server = new AsrVAD();
	server->init();
	while(true){
		ret = fread(buf,sizeof(short),BLOCK,fp);
		if( ret==0 )
			break;
		samNum += ret;
		int state = server->process( buf,ret );
//		printf( "reading %d samples:%d\n",samNum,state );
		if( state==-1 || state>SIL_TIMEOUT-45 ){
			printf( "speech end at %d:%d ",samNum,state );
			break;
		}
	};

	if( ret==0 ){
		printf( "no speech found\n" );
	}
	delete server;
//	end = GetTickCount();
//	printf( "Uts=%d\n",ee );
//	printf( "Real-Time Ratio=%fRT\n",(end-start)*1.0f*16/(FrmNum*110) );
//	printf( "Real-Time Ratio=%fRT\n",getCount()*1.0f*16/(FrmNum*110) );
// 	printf( "Spectral Energy = %f of Total Computation\n",clock_1*1.0f/(end-start) );
// 	printf( "DownSample = %f of Total Computation\n",clock_2*1.0f/(end-start) );
// 	printf( "QuantileMask = %f of Total Computation\n",clock_3*1.0f/(end-start) );
// 	printf( "EPD_Measure = %f of Total Computation\n",clock_4*1.0f/(end-start) );
 	fclose( fp );
//	fclose( fpD );
//	free(buf);
 //	delete pEPD;
	return 0;
}

