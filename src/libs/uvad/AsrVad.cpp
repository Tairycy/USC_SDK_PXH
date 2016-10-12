#include "AsrVad.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
//#include <cstdlib>
#define MAX(x, y) ((x) > (y)) ? (x) : (y)
#define MIN(x, y) ((x) > (y)) ? (y) : (x)

namespace usc {


int AsrVAD::init( int kHZ )
{
	m_pVAD = initVAD_USC( kHZ );
	if( m_pVAD==NULL )
		return -1;

	mPitch = initPITCH_USC(kHZ,30, 10, MAX_PITCH_DINGDONG, MIN_PITCH_DINGDONG);
	if (mPitch == NULL)
		return -1;
    resetPITCH_USC(mPitch);


	mFrmNum = 0;
	mLeftLen = 0;
	start = -1;
	stop = 0;
	return 0;
}

void AsrVAD::setVadSilTime(int frontSil, int backSil)
{
    if (m_pVAD) {
        setParam_USC(m_pVAD, frontSil, backSil);
    }
}

void AsrVAD::reset(){
	resetVAD_USC(m_pVAD);
	mLeftLen = 0;
	start = -1;
	stop = 0;
	mFrmNum = 0;
}
/*
 *	@return continious silence frames
 */
int AsrVAD::process(short* pcm, unsigned int len,int* energy)
{
	short* curPos = pcm;
	static int frmLen = SM_FRE * FRM_MS;
	int samIn = len;
	int samPush =  frmLen - mLeftLen;
	int ret;
	int type = 0;
	if( samPush> samIn ){
		memcpy( mLeftPcm+mLeftLen,curPos,sizeof(short)*samIn );
		mLeftLen += len;
		return type;
	}

	memcpy( mLeftPcm+mLeftLen,curPos,sizeof(short)*samPush );
	mLeftLen = 0;
	curPos += samPush;
	samIn -= samPush;

	while( true ){
		mFrmNum += 3;
		ret = push2VAD_USC( m_pVAD,mLeftPcm,frmLen,energy );
		if( start==-1 && ret == SPEECH ){
			type = 3;
			start = m_pVAD->front;
		}else if( ret==NONSPEECH ){
			type = 1;
			stop = m_pVAD->end;
			start = m_pVAD->front;
			break;
		}else if( ret==MAX_SIL ){
			type = 2;
			break;
		}
		if( samIn<frmLen )
			break;
		memcpy( mLeftPcm,curPos,sizeof(short)*frmLen );
		curPos += frmLen;
		samIn -= frmLen;
	}

	if( samIn>0 && type == 0 ){
		memcpy( mLeftPcm,curPos,sizeof(short)*samIn );
		mLeftLen = samIn;
	}


	return type;

}


int AsrVAD::checkPitchOffset(short *pcm, int pcm_len)
{
    short *targetPcmBuffer = (short *)pcm;
    int targetPcmSamples = pcm_len;
    
    int targetPitchNumber = 0; int EnNumber = 0;
    int *targetPitches = (int *)malloc(targetPcmSamples * sizeof(int));
	Int64 *HiEnergy = (Int64 *)malloc(targetPcmSamples * sizeof(Int64));
	Int64 *FuEnergy = (Int64 *)malloc(targetPcmSamples * sizeof(Int64));
	feature* pfeature = new feature[targetPcmSamples];
	dingEn* pdingen = new dingEn[targetPcmSamples];

	int isCheckDing = 1;
	int ave = 0;

    push2PITCH_full(mPitch, targetPcmBuffer, targetPcmSamples, 0, pfeature, &targetPitchNumber,&ave,pdingen,&EnNumber,isCheckDing);

//	printf("pitch value:\n");
	for (int i=0;i<targetPitchNumber;i++){
		targetPitches[i] = pfeature[i].pitch>>6;
//		printf("%d:%d ",i,targetPitches[i]);
	}
	printf("\n");
    
	for (int i=0;i<EnNumber;i++){
		HiEnergy[i] = pdingen[i].high;
		FuEnergy[i] = pdingen[i].full;
	}

	int BeepEndIndex = -1;float ratio = 0.0f;
	int pitchbegin = 0;int pitchend = -1;
	int firstPassMinPitchIndex = -1;
	if(FindVaildPitchSeg(targetPitches,targetPitchNumber,&pitchbegin,&pitchend) > 0){
		if(pitchend - pitchbegin > PATTERN_PITCH_COUNT){

			int firstPassCompareCount = targetPitchNumber - (PATTERN_PITCH_COUNT - 1);
			unsigned long firstPassMinSum = firstPassThreshold;

			int minsum = 1000000;
			for (int i = 0; i < firstPassCompareCount; i++) {
				unsigned long sum = 0;
				for (int j = 0; j < PATTERN_PITCH_COUNT; j++) {
					sum += abs(targetPitches[i + j] - firstPassPitch[j]);
				}
				sum/=PATTERN_PITCH_COUNT;
				if(sum < minsum){
					minsum = sum;
				}

				if (sum < firstPassMinSum) {
					firstPassMinSum = sum;
					firstPassMinPitchIndex = i;
				}
			}   

			if (firstPassMinPitchIndex != -1) {
				BeepEndIndex = firstPassMinPitchIndex + TOTAL_PASS_PITCH_COUNT;
				BeepEndIndex = MIN(BeepEndIndex, targetPitchNumber);

				firstPassMinPitchIndex -= PATTERN_PITCH_BEGIN;
				firstPassMinPitchIndex = (firstPassMinPitchIndex > 0)? firstPassMinPitchIndex:0;

				ratio = CalcEnergyRatio(HiEnergy,FuEnergy,firstPassMinPitchIndex,BeepEndIndex);
			}

//			printf("minsum = %d,threshold = %d\n",minsum,firstPassThreshold);
		}else if(pitchend - pitchbegin > MINI_PATTERN_PITCH_COUNT){

			unsigned long sum = 0;
			for (int j = 0; j < pitchend-pitchbegin; j++) {
				sum += abs(targetPitches[pitchbegin+j] - firstPassPitch[j]);
			}
			sum /= ((pitchend-pitchbegin)+0.00000001);
			if (sum < firstPassThreshold) {
				firstPassMinPitchIndex = pitchbegin;
				BeepEndIndex = pitchend;
				ratio = CalcEnergyRatio(HiEnergy,FuEnergy,firstPassMinPitchIndex,BeepEndIndex);
			}

//			printf("sum = %lu,threshold = %d\n",sum,firstPassThreshold);
		}
	}
			
//	printf("ratio: %f, first: %d end: %d total: %d", ratio, firstPassMinPitchIndex,BeepEndIndex, targetPitchNumber);
	
	free(targetPitches);
	free(HiEnergy);
	free(FuEnergy);
	delete[] pfeature;
	delete[] pdingen;

	// return end index;
    if (BeepEndIndex == -1) {
        return 0;
	}else if(ratio < 0.4f){
		return 0;
	}else{
		return (int)(((BeepEndIndex) / (float)targetPitchNumber) * targetPcmSamples) * 2;
    }
}

}
