#ifndef _OPUS_WRAPPER_H
#define _OPUS_WRAPPER_H

#include "opus.h"
#include "Compresser.h"
#include "cache.h"
namespace uni {

#ifdef __cplusplus
extern "C" {
#endif


class Opus:public Compresser{
	public:
		static const int WB_MODE = 1;
		static const int NB_MODE = 2;
	public:
		Opus(int wb_mode=1, bool encode=true);
		~Opus();

		int encode(char* src, int srcLen, char **dst, unsigned int* dstLen);
		int encodeFrame(void);
		
        int decode(char* src, int srcLen, char** dst, unsigned int* dstLen);
        int decodeFrame(void);

		/* added by lqy for cache reset avoid the decode & encode error */
		void cacheReset();

public:
    enum errorCode
    {
        SUCCESS = -91900,
        INPUT_PARAMS_INVALID,
        INTERNAL_CALL_INVALID,
        SRC_DATA_ERROR,
        OPUS_ENCODE_ERROR,
        OPUS_DECODE_ERROR,
        JNI_INPUT_NULL_INVALID,
        JNI_INPUT_HANDLE_INVALID,
        JNI_INPUT_SRC_LENGTH_INVALID,
        JNI_INPUT_MODE_INVALID,
    };
	private:
        bool headRecved(void);
        int decHeadValue(int *errNo);
        char *getFrameBody(unsigned int *len);
        void releaseFrameBody(char *body);
        bool frameCompletely(int *errNo);
        int needFrameLen(void);
        int feedFrameData(char *input, unsigned int len, int *errNo, bool *startProcess);

	private:
        int sample_rate;
        int frame_size;
        int decHeadSize;

        long handle;

        bool encodeFlag;

        CLCache *srcCache;
        CLCache *dstCache;

};

#ifdef __cplusplus
}
#endif

}

#endif

