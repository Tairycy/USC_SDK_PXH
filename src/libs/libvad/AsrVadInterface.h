#ifndef __ASR_VAD_INTERFACE_H__
#define __ASR_VAD_INTERFACE_H__


#define USC_UNINSTANCE_ERROR -1
enum {
    USC_VAD_HOLDING   = 0,
    USC_VAD_BACK_END  = 1,
    USC_VAD_MAX_SIL   = 2,
    USC_VAD_FRONT_END = 3,
};





namespace usc {

class AsrVADInterface
{
public:
	virtual int init( int kHZ = 16 ) = 0;

    virtual void setVadSilTime(int frontSil, int backSil) = 0;    
	virtual int process(short* pcm, unsigned int len,int* energy) = 0;
	virtual int checkPitchOffset(short*pcm, int pcm_len) = 0;
	virtual int setparas( int id, const void *value) = 0;
	virtual void reset() = 0;
	virtual int getSpeakStart() {return 0;};
};

} // namespace usc
#endif
