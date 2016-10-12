#ifndef __ASR_DEF_VAD_H__
#define __ASR_DEF_VAD_H__

#include "../vad/AsrVadInterface.h"

#include "decision.h"

#define ASR_VAD_TIMEOUT -7

namespace usc {

class AsrVAD : public AsrVADInterface
{
	static const int SM_FRE = 16;
	static const int FRM_MS = 30;
	static const int SFT_MS = 10;
public:

	AsrVAD()
	{
		m_pVAD = NULL;
		mPitch = NULL;
		mLeftLen = 0;
		mFrmNum = 0;
		start = 0;
		stop = 0;
	}
	~AsrVAD()
	{
		if( m_pVAD != NULL)
			delVAD_USC(m_pVAD);
		if(mPitch != NULL)
			delPITCH_USC(mPitch);

	}
    
	virtual int init( int kHZ = 16 );
    virtual void setVadSilTime(int frontSil, int backSil);
    
	virtual int process(short* pcm, unsigned int len,int* energy);
	virtual int checkPitchOffset(short*pcm, int pcm_len);

	virtual void reset();

	virtual int getSpeakStart() {return start;};
private:
	VAD* m_pVAD;
	PITCH* mPitch;
	short mLeftPcm[SM_FRE * FRM_MS];
	int mLeftLen;
	int mFrmNum;
	int start;
	int stop;

};

}


#endif
