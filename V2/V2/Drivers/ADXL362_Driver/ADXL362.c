//ADXL362.c
//The driver of ADXL362 (accelerometer + thermometer)
/* includes */
#include "spi.h"
#include "ADXL362.h"
/* private  defines */
#define TR // uncomment to use TransmitReceive, otherwise using Tansmit and Receive
/* private function Prototypes */
uint8_t ADXL362_Init(void);
uint8_t ADXL362_ReadReg(uint8_t regAddress, uint8_t *regValue, uint16_t size);
uint8_t ADXL362_WriteReg(uint8_t regAddress, uint8_t Value);

/* private fuctions */

uint8_t ADXL362_Init(void)
{
	uint8_t returnValue = 0;
	
	//ADXL362_WriteReg(ADXL362_REG_SOFT_RESET, 0x52); //'R'
	//HAL_Delay(1); // wait 1ms
	
	uint8_t partIdBuffer = 5;
	ADXL362_ReadReg(ADXL362_REG_PARTID, &partIdBuffer, 1);	
	if (partIdBuffer != ADXL362_PART_ID)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	else
	{
		returnValue = 1;
	}
	// adjust measure range
	ADXL362_WriteReg(ADXL362_REG_FILTER_CTL, 0x13); // range : 0x83 = +-8g, 0x53 = +-4g, 0x13 = +-2g
	ADXL362_WriteReg(ADXL362_REG_POWER_CTL, 0x22); // 0x02 measure mode

	return returnValue;
}
uint8_t ADXL362_Stop(void)
{
	// set ADXL362 to stop mode
	ADXL362_WriteReg(ADXL362_REG_POWER_CTL, 0x00); // 0x00 stop mode
	ADXL362_Off(); // CS = 1;
	return 1;
}
#ifdef TR
uint8_t ADXL362_ReadReg(uint8_t regAddress, uint8_t *regValue, uint16_t size)
{
	ADXL362_On();
	uint8_t returnValue = 0;
	uint8_t index = 0;
	uint8_t rxBuffer[10] = {0};
	uint8_t txBuffer[10] = {0};// head = command(W/R) + reg address
	txBuffer[0] = ADXL362_READ_REG;
	txBuffer[1] = regAddress;
	if (HAL_SPI_TransmitReceive(&hspi1, txBuffer, rxBuffer, 2 + size, 10) != HAL_OK) // send command 
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	else
	{
			returnValue = 1;
	}
	
	for(index = 0; index < size; index++)// when the optimization level = 3, the assignment cannot be done 
	{
		regValue[index] = rxBuffer[index + 2];
	}
	ADXL362_Off();
	return returnValue;
}
#else
uint8_t ADXL362_ReadReg(uint8_t regAddress, uint8_t *regValue, uint16_t size)
{
	ADXL362_On();
	uint8_t returnValue = 0;
	uint8_t HeadBuffer[2] = {ADXL362_READ_REG, regAddress}; // head = command(W/R) + reg address
	if (HAL_SPI_Transmit(&hspi1, HeadBuffer, 2, 10) != HAL_OK) // send command 
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	else
	{
		if(HAL_SPI_Receive(&hspi1, regValue, size, 10) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		else
			returnValue = 1;
	}

	ADXL362_Off();
	return returnValue;
}
#endif
uint8_t ADXL362_WriteReg(uint8_t regAddress, uint8_t Value)
{
	ADXL362_On();
	uint8_t returnValue = 0;
	uint8_t txBuffer[] = {ADXL362_WRITE_REG, regAddress, Value}; // command(W/R) + reg address + content
	if (HAL_SPI_Transmit(&hspi1, txBuffer, 3, 10) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	else
		returnValue = 1;
	ADXL362_Off();
	return returnValue;
}

void ACCE_T_GetData(float *buffer)
{
	uint8_t rxData[8]= {0};
	int16_t temp[4] = {0};
	uint8_t index = 0;
	ADXL362_ReadReg(ADXL362_REG_XDATA_L, rxData, 8); // read XYZ and T

	for(index = 0; index < 4; index++)
	{
		if(rxData[index * 2 + 1] >> 3 == 0 ) // positive number
		{
			rxData[index * 2 + 1] &= 0x07; // MSB & 00000111 = clear MSB[11:15];
			temp[index] = ((uint16_t)rxData[index * 2 + 1] << 8) + rxData[index * 2];
		}
		else
		{
			rxData[index * 2 + 1] &= 0x07; // MSB & 00000111 = clear MSB[11:15];
			temp[index] = -1 *(((uint16_t)rxData[index * 2 + 1] << 8) + rxData[index * 2]);
		}
	}
	buffer[0] = temp[0] * 0.977 ; //*4; // 3.908mg/unit +-8g
	buffer[1] = temp[1] * 0.977 ;//*4;
	buffer[2] = temp[2] * 0.977 ;//*4;
	buffer[3] = temp[3] * 0.065 - 4; // 0.065 C/unit
}
