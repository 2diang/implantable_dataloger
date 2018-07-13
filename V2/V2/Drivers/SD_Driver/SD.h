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
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
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
