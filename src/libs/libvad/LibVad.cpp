#include "LibVad.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#ifdef _ANDROID_BUILD_
#include "../uvad/decision.h"
#endif

#define MAX(x, y) ((x) > (y)) ? (x) : (y)
#define MIN(x, y) ((x) > (y)) ? (y) : (x)
// #define ANDROID_LOG

#define VAD_BUFFER_SHORT_LEN		160	// 10ms

#ifdef ANDROID_LOG
#include <android/log.h>
#endif

namespace usc {

LibVAD::LibVAD()
{
	m_isSpeakStart = false;
	m_isInit = false;
	m_nBufferLength = 0;
	m_nProcessBytes = 0;
	m_nStartBytes = 0;
	m_nKHZ = 16;

	m_nEndSilms = VAD_DEFAULT_END_SIL*10;
	setFrontSil(VAD_DEFAULT_FRONT_SIL*10);
#ifdef _ANDROID_BUILD_
	mPitch = NULL;
#endif
}
LibVAD::~LibVAD()
{
#ifdef _ANDROID_BUILD_
	if(mPitch != NULL) {
		delPITCH_USC(mPitch);
		mPitch = NULL;
	}
#endif
}

void LibVAD::setFrontSil(int ms) {
	m_nFrontSilms = ms;
	m_nFrontSilMaxBytes = m_nKHZ * sizeof(short) * ms; // ms to bytes
#ifdef ANDROID_LOG
	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "setFrontSil m_nFrontSilms=%d m_nFrontSilMaxBytes=%d", m_nFrontSilms, m_nFrontSilMaxBytes);
#endif
	
}


int LibVAD::init( int kHZ )
{
	if(m_isInit) {
		return 0;
	}


#ifdef _ANDROID_BUILD_
	// pitch
	mPitch = initPITCH_USC(kHZ,30, 10, MAX_PITCH_DINGDONG, MIN_PITCH_DINGDONG);
	if (mPitch == NULL)
		return -1;
    resetPITCH_USC(mPitch);
#endif

	int nRet = m_vad.init(kHZ);
	if(nRet == 0) {
		m_nKHZ = kHZ;
		m_isInit = true;
		int value = 0;
		//setparas(PITCHENDNUMTH, &value);
		value = 6;
		//setparas(PITCHSTNUMTH,&value);
		setVadSilTime(VAD_DEFAULT_FRONT_SIL, VAD_DEFAULT_END_SIL);

	}
#ifdef ANDROID_LOG
	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "init nRet=%d khz=%d", nRet, kHZ);
#endif
	return nRet;
}

int LibVAD::setparas( int id, const void *value) 
{
#ifdef ANDROID_LOG
//	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "setparas_ id=%d value=%s", id, value);
#endif

	m_vad.setparas(id, value);
	return 0;
}

void LibVAD::setVadSilTime(int frontSil, int backSil)
{

//	itoa(backSil,value,10);

#ifdef ANDROID_LOG	
	char value[32];
	sprintf(value,"%d",backSil);
	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "setVadSilTime frontSil=%d backSil=%d", frontSil, backSil);
#endif
	
//	MINSIGLEN,     // min signal length for speech, 10 (int)
//	MAXSILLEN,     // max silence length, 30 (int)
//	m_nEndSilms = backSil*10; // to ms
	setparas( MAXSILLEN, &backSil);

	setFrontSil(frontSil * 10); 
}

void LibVAD::reset(){
	m_vad.reset(RESET_VEND);
	m_nProcessBytes = 0;
	m_nBufferLength = 0;
	m_nStartBytes = 0;
	m_isSpeakStart = false;
#ifdef ANDROID_LOG
	__android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "reset");
#endif

}

int LibVAD::processFrame(short* pcm, unsigned int len,int* energy) {

	m_nProcessBytes += (len*sizeof(short)); // short to bytes

	int ret = USC_VAD_HOLDING;
	int code = m_vad.process(pcm, len,energy);
	switch(code) {
	case SILENCE:    
	case ON_VOICE:   
	case DO_NOTHING: 
		{
			if( !m_isSpeakStart && m_nProcessBytes > m_nFrontSilMaxBytes ) {
				ret = USC_VAD_MAX_SIL;
				return ret;
			}
			else {
				ret = USC_VAD_HOLDING;
			}
		}
		break;
	case ST_VOICE:   
		ret = USC_VAD_FRONT_END;
		m_isSpeakStart = true;
		return ret;
	case END_VOICE:	 
		ret = USC_VAD_BACK_END;
		m_isSpeakStart = true;
		return ret;
	default: 
		ret = USC_VAD_HOLDING;
	}

	return ret;
}

/*
 *	@return continious silence frames
 */
int LibVAD::process(short* pcm, unsigned int len, int* energy)
{
	int ret = USC_VAD_HOLDING;
	short *pcmIndex = pcm;
	short frame[VAD_BUFFER_SHORT_LEN];

	if( len + m_nBufferLength < VAD_BUFFER_SHORT_LEN) {
		// Save less than one frame of data
		int all = len + m_nBufferLength;
		for( int i=m_nBufferLength; i<all; i ++) {
			mBuffer[i] = pcmIndex[0];
			pcmIndex++;
		}
		m_nBufferLength = all;
		return ret;
	}

	// read backup data
	for( int i = 0; i < m_nBufferLength; i ++) {
		frame[i] = mBuffer[i];
	}

	// full one frame data
	for( int i = m_nBufferLength; i<VAD_BUFFER_SHORT_LEN; i ++) {
		frame[i] = pcm[0];
		pcm++;
		len --;
	}
	m_nBufferLength = 0;

	// Frame alignment data processing
	ret = processFrame(frame, VAD_BUFFER_SHORT_LEN,energy);
	if(ret != 0) {
		return ret;
	}

	int nFrame = len/VAD_BUFFER_SHORT_LEN;
	m_nBufferLength  = len%VAD_BUFFER_SHORT_LEN;

	if(m_nBufferLength > 0 ) {
		// Save less than one frame of data
		pcmIndex = pcm;
		pcmIndex += nFrame * VAD_BUFFER_SHORT_LEN;
		for( int i=0; i<m_nBufferLength; i ++) {
			mBuffer[i] = pcmIndex[0];
			pcmIndex++;
		}
	}
	
	// Frame alignment data processing
	for( int i=0; i< nFrame; i++) {

		ret = processFrame(pcm, VAD_BUFFER_SHORT_LEN,energy);
		pcm += VAD_BUFFER_SHORT_LEN;

		if(ret != 0) {
			return ret;
		}
	}
		

#ifdef ANDROID_LOG
	if(ret != USC_VAD_HOLDING) {
	    __android_log_print(ANDROID_LOG_INFO, "JNI_VAD", "process ret =%d len =%d energy=%d m_nProcessBytes=%d m_isSpeakStart=%d m_nFrontSilMaxBytes=%d", ret, len, *energy,m_nProcessBytes, m_isSpeakStart,m_nFrontSilMaxBytes);
	}
#endif

	return ret;
}


int LibVAD::checkPitchOffset(short *pcm, int pcm_len)
{
#ifdef _ANDROID_BUILD_
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
#endif
	return 0;
}

}
