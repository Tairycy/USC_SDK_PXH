#ifndef __ASR_SERVICE_INCLUDED__
#define __ASR_SERVICE_INCLUDED__

#include "service.h"

class asr_service : public UniServiceInterface
{
public:
	asr_service();
	~asr_service();

	int Stop();

};


#endif