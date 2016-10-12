#ifndef __GET_TIME_H__
#define __GET_TIME_H__

#ifdef __cplusplus
extern "C"{
#endif

unsigned long getCurrentTimeM();
unsigned long getCurrentTimeS();
void sleepMS(unsigned int time);

#ifdef __cplusplus
}
#endif

#endif
