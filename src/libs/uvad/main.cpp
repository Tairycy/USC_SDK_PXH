#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AsrVad.h"



int main(int argc , char** argv) {
	AsrVAD* pEngine = NULL;
    int kHZ;
	short buff[480];
	char wav[1024];
	FILE* fvoice = NULL;
	FILE* flist = NULL;
	int nFrmLen = 220;
	int n,silF = 300,silB = 100;
	int ret;
	pEngine = new AsrVAD;
    kHZ = atoi(argv[2]);
	ret = pEngine->init( kHZ );
	if( ret!=0 ){
		printf( "cannot launch VAD\n" );
		return -1;
	}
    pEngine->setVadSilTime( silF,silB );
		
	flist = fopen( argv[1],"rt" );
	if( flist==NULL ){
		fprintf( stderr,"cannot open file %s\n",argv[1] );
		return -1;
	}

	while( fgets( wav,1024,flist ) ){
		FILE* fvoice = NULL;
		char* pos = strtok( wav," \t\r\n" );
        int Energy;
		if( pos==NULL )
			continue;
		fvoice = fopen(pos,"rb");
		if( fvoice==NULL ){
			fprintf( stderr,"cannot open file %s\n",wav );
			continue;
		}
		fseek(fvoice, 44, SEEK_SET);
		//int flush = 0;
		pEngine->reset();
		printf( "process %s ...\n",wav );
		while( (n = fread(buff, sizeof(short), nFrmLen, fvoice)) != 0 ){
			ret = pEngine->process(buff, n,&Energy);
			if(ret == 1) {
				break;
			}else if( ret==2 ){
				printf("max sil %d reached\n",silF);
				break;
			}
		}
		fclose(fvoice);
//		if( ret == NONSPEECH )
		printf("%f %f\n", pEngine->start * 0.01, pEngine->stop * 0.01);
		printf( "done!\n" );


	}
	if( pEngine )
		delete pEngine;
	fclose( flist );
	return 0;
}
