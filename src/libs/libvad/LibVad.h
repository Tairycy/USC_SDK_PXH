#ifndef __USC_LIB_VAD_H__
#define __USC_LIB_VAD_H__

#include "AsrVadInterface.h"
#include "AsrVad.h"
#ifdef _ANDROID_BUILD_
#include "uvad/decision.h"
#endif
// VAD_DEFAULT_FRONT_SIL_MS 10ms
#define VAD_DEFAULT_FRONT_SIL   300
#define VAD_DEFAULT_END_SIL   30

namespace usc {

class LibVAD : public AsrVADInterface
{
public:

	LibVAD();
	~LibVAD();
    
	 int init( int kHZ = 16 );
     void setVadSilTime(int frontSil, int backSil);
    
	 int process(short* pcm, unsigned int len,int* energy);
	 int checkPitchOffset(short*pcm, int pcm_len);
	
	 void reset();

	 int getSpeakStart() {return m_nStartBytes;};
	
	 int setparas( int id, const void *value);
private:
	AsrVAD m_vad;
#ifdef _ANDROID_BUILD_
	PITCH* mPitch;
#endif
	bool m_isInit;
	bool m_isSpeakStart;

	int m_nProcessBytes;
	int m_nFrontSilms; // 
	int m_nFrontSilMaxBytes; // 
	int m_nEndSilms; // 
	int m_nStartBytes;
	int m_nKHZ;
	int m_nBufferLength;
	short mBuffer[640];
private:
	void setFrontSil( int ms);
	int processFrame(short* pcm, unsigned int len,int* energy);
};

}


#endif
