//sys_Value.c
// define all the system variables when the system runs
// includes 
 #include "sys_Value.h"
 #include "stdlib.h"
 // testing 
 int counter1 = 0;
 int counter2 = 0;
 int counter3 = 0;
 int counter4 = 666;
 int ECGCounter = 0;
 int ACCECounter = 0;
 int RTCCounter = 0;
 int DatatransCounter = 0;
 int Mode = 666;
 
 // system status
 enum SystemState sysflag = IDLE;

 // system timing is in the h file
 volatile uint32_t milisecond = 0;
 volatile uint32_t second = 0;
 // data storage
 uint8_t RTCData[SECOND][6] = {0};
 uint16_t ECGData[SECOND][1000] = {0};
 float ACCE_TData[SECOND][4] = {0};
 
// void FloatToByte(float floatData, uint8_t* StringData)
// {
//	 

// }