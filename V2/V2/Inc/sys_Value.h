// sysValue.h

// includes
#ifndef __SYS_VALUE_H
#define __SYS_VALUE_H
#include "stdint.h"
// testing 
extern int counter1;
extern int counter2;
extern int counter3;
extern int counter4;
extern int ECGCounter;
extern int ACCECounter;
extern int RTCCounter;
extern int DatatransCounter;
extern int Mode;
 
//defines 
#define SECOND 60
 
// system status
enum SystemState
{
	IDLE = 0U,
	INIT_SET,
	ECG,
	ACCE_T,
	TRANSDATA,
	RTC_RESET
};
extern enum SystemState sysflag;
 
// system timing
extern volatile uint32_t milisecond;
extern volatile uint32_t second;

 
// data storage
extern uint8_t RTCData[SECOND][6];
extern uint16_t ECGData[SECOND][1000];
extern float ACCE_TData[SECOND][4];
 
#endif /* __SYS_VALUE_H*/

