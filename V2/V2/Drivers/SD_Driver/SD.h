// SD.h
#ifndef __SD_H
#define __SD_H

// includes
#include "stdint.h"
#include "main.h"
// externs 
extern uint8_t SD_Type;

// fuction prototype
uint32_t SD_GetSectorCount(void);
uint8_t SD_WriteDisk(uint8_t*buf, uint32_t sector, uint8_t cnt);
uint8_t SD_ReadDisk(uint8_t* buf, uint32_t sector, uint8_t cnt);
uint8_t SD_Init(void);
uint8_t SD_WaitReady(void);

// defines 
 
#define  SD_CS_On() HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET)
#define  SD_CS_Off() HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET)

// SD card type
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

// CMD defines
#define CMD0    0       //software reset
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
// respond defines
#define MSD_FAIL 0xff

#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF
// write read sign
#define STUFF_BYTE 0xff
#define DUMMY_CRC 0x01
#define RECV_START_TOKEN 0xfe
#define TRANS_START_TOKEN 0xfe
#define MUL_TRANS_START_TOKEN 0xfc
#define TRANS_END_TOKEN 0xfd

#define SD_FAILURE 1
#define SD_SUCCESS 0
#endif
