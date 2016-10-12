#ifndef __ASR_VAD_H__
#define __ASR_VAD_H__
#include "vad_paras.h"
// caution:
// the first 15ms data should throw away, because some machine always be zeros or very low energy data.

class AsrVAD
{
public:
	/************************************************************************\
	 FUNCTION:  init
	 DESCRIPTION:
	   Do initialization.
	 PARAMETERS: 
	   kHZ:         The speech samplerate/1000.[In]
	 RETURN:
	  0     : Success.
	  Others: Errors.
	 NOTE: just surport 16k now!
	\************************************************************************/
	int init( int kHZ = 16 );
    
	/************************************************************************\
	 FUNCTION:  process
	 DESCRIPTION:
	   Do voice detection.
	 PARAMETERS: 
	   pcm: Voice data.[In]
	   len: Data length.[In]
	   energy: data energy.[Out]
	 RETURN:
	  >=0 : status of vad.
	  < 0 : errors.
	 NOTE: 1. len must be 10ms(160 for 16kHz), if not this, errors, so if the data length
	          is not this, pls do a loop calling or waiting!
	       2. if the voice start status returned, the voice have persistened hundreds of ms,
		      so, the upper calling function must do cache for the voice, 0.5s maybe enough
			  for default setting.
	\************************************************************************/
	int process(const short* pcm, int len, int* energy);

	/************************************************************************\
	 FUNCTION:  getstlength
	 DESCRIPTION:
	   get the voice length last before Start voice detected.
	 PARAMETERS: 
	   No.
	 RETURN:
	  the frame number.
	  < 0 : errors.
	 NOTE: the buffer could be setted large, and use this value as the realy start point.
	       but, the length should be larger than it for thinking about error, such as, 
		   10 frame(100ms) lasted, use 200~300 ms to protect the boundry.
	\************************************************************************/
	int getstlength(){return nLen;}

	int getcilp(){return m_nClipNumOut; }

	float getsnr() {return m_fSnrOut;}

	float getvol() {return m_fSpVolOut;}

	/************************************************************************\
	 FUNCTION:  setparas
	 DESCRIPTION:
	   Set parameters of the vad engien.
	 PARAMETERS: 
	   ntype : vad parameters' type, see VAD_PARA_TYPES.[In]
	   val   : parameter's values.[In] 
	 RETURN:
	  None.
	 NOTE: 1. MINBACKENG: minimum of the back energy, it is got by calculate
	          the frame energy of the noise, the default value is got by dongle's voice,
			  if the voice energy is low, the value should be tuned down.
		   2. MINBACKENGH: it is very like above parameter, above is low TH for noise, 
		      this is for high TH, it is for voice start.
		   3. PITCHTH: pitch threshold, high for voice, low for noise,if set zero, 
		      not use pitch, if the voice's quality is not so good, especialy in low 
			  frequnce region, set it to zero, and if the quality is very good, set it
			  higher may cut many high energy noise.
		   4. PITCHSTNUMTH: how many frame contain vowle may consider voice, larger for
		      good quality voice.
		   5. PITCHENDNUMTH: how many frame not contain vowls may consider noise, larger
		      for bad quality voice.
		   6. LOWHIGHTH: high frequency energy divide low frequency energy, for judge vowel,
		      lower for stricter.
		   7. SINGLEMAX: max single point energy, very large means not used, lower for 
		      restricter for voice, it is useful for single period noise, but bad for
			  very noise data, because /u/ /n/ phone just contain 3~4 harmonic, if the 
			  noise is very high, some may lost.
		   8  NOISE2YTH: noise to signal energy ratio threshold for voice, lower for more 
		      restrict, when the noise is very high, tune it high.
		   9  NOISE2YTHVOWEL: like the NOISE2YTH, but it is for vowel speech, so, it should
		      lower, when the noise is very high, tune it high.
		   10 VOICEPROBTH: the speech prob from spectral energy, tune it down when the noise
		      energy is very high.
		   11 USEPEAK: if use freq domain for period detect, default use time domain. If the 
		      noise is very high, the time domain maybe lose, use this maybe better, this method
			  may instead the time domain in future.
		   12 PITCHLASTTH: the pitch last frame number threshold, for distinct tail and middle,
		      if 1 is set, means not use it.
	\************************************************************************/
	void setparas(int ntype, const void* val);

	/************************************************************************\
	 FUNCTION:  reset
	 DESCRIPTION:
	   Do reset.
	 PARAMETERS: 
	   None.
	 RETURN:
	  None.
	 NOTE: when get the end status of the voice, call this for reset.
	\************************************************************************/
	void reset(int ntype = RESET_FEND);

	// constructor & destructor
	AsrVAD() {
		psDataBuf = 0;
		pvhd = 0;
	}
	~AsrVAD();

private:
	int  GetEng(const short* psDataIn, int len, int* energy, int* engstatus);
	void idhdbypd(const short* psDataIn, int len);
	void idtlbypd(const short* psDataIn, int len);
	void idhdbypk();
	void idtlbypk();
//	void idhdbycpt(const short* psDataIn, int len);
//	void idtlbycpt(const short* psDataIn, int len);
	int  vadlogic(int nStatus, const short* psDataIn, int len);
	void* pvhd;
	int nStatus;
	int nFrameLen;
	int nFrameSft;
	int nLen;
	int nInitYet;
	short* psDataBuf;
	int nGetStartYet;
	int m_nSilCnt;
	int m_nSigCnt;   // for zero the m_nSilCnt
	int m_nSigCntTh; 

	int nProbInc;
	short psForProbData[1024];
	float m_prob, m_probaux;
	int   nContinuePitch, nContinuePitch2;
	int   nLastPitchSign;
	float m_preprob[8]; // for 3 value max filter
	int   m_preEng[8]; // for 3 value max filter,for hold on
	// parameters
	float m_minBackEng;
	float m_pitchTh;
	int   m_nPitchNumTh;
	float m_flowhighcmpTh;
	int   m_nPitchEndNumTh;
	int   nMinSigLen;
	int   nMaxSilLen;
	float m_fnoise2yTh;
	float m_fVowelnoise2yTh;
	int   m_nusepeak;
	int   m_nstart;
	int   m_nstop;
	float m_feng;
	float m_feng2Th;
	int   m_nProbLastNum;
	int   m_nProbLastTh;
	// for fix version
	int   m_nUseFixVad;
	void* fixVad;
	int   m_nFrmCnt;
	// use continue pitch
//	void* m_pPitch;
	int  m_Fs;
	// for quality check
	int  m_nDoQualtyCheck;
	int  m_nClipNum; // clip frame number for each speech part	
	float m_fASpVol;   // average energy for each speech part
	float m_fANsVol;   // average energy for global noise
	int m_nQualityFrmNum;// for check quality frame number
	int m_nNoiseFrmNum;  // global noise frame number

	float m_fSnrOut;    // average snr for each speech part
	float m_fSpVolOut; 
	int   m_nClipNumOut;
};

#endif
