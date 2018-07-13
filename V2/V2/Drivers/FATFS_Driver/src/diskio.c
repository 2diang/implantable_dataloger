/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */	
#include "SD.h"
#include "spi.h"
#include "stdlib.h"
#include "rtc.h"
#include "sys_Value.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//FATFS disio.c ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//��ʼ������
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	BYTE res = 0;	    

	res = SD_Init();
	if(res == SD_TYPE_ERR)//STM32 SPI��bug,��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
	{
		SPI2_LowSpeed();
		SPI2_ReadWriteByte(STUFF_BYTE);//�ṩ�����8��ʱ��
		SPI2_HighSpeed();
	}		 
	if(res == SD_TYPE_ERR)
		return  STA_NOINIT;
	else 
		return 0; //��ʼ���ɹ�
}  

//��ô���״̬
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

//������
//drv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	BYTE res = 0; 
    if (count == 0)
		return RES_PARERR; //count���ܵ���0�����򷵻ز�������		 	 

	res = SD_ReadDisk(buff,sector,count);	 
	if(res == SD_FAILURE)//STM32 SPI��bug,��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
	{
		SPI2_LowSpeed();
		SPI2_ReadWriteByte(STUFF_BYTE);//�ṩ�����8��ʱ��
		SPI2_HighSpeed();
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == SD_SUCCESS)
		return RES_OK;	 
    else 
		return RES_ERROR;	   
}

//д����
//drv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	BYTE res = 0;  
    if (count == 0)
		return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 

	res = SD_WriteDisk((BYTE*)buff,sector,count);

    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == SD_SUCCESS)
		return RES_OK;	 
    else 
		return RES_ERROR;	
}
#endif


//����������Ļ��
 //drv:���̱��0~9
 //ctrl:���ƴ���
 //*buff:����/���ջ�����ָ��
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						  			     
	switch(cmd)
	{
		case CTRL_SYNC:
			SD_CS_On();
			if(SD_WaitReady() == 0)
				res = RES_OK; 
			else 
				res = RES_ERROR;	  
			SD_CS_Off();
			break;	 
			
		case GET_SECTOR_SIZE:
			*(WORD*)buff = 512;
			res = RES_OK;
			break;	 
		
		case GET_BLOCK_SIZE:
			*(WORD*)buff = 8;
			res = RES_OK;
			break;	
		
		case GET_SECTOR_COUNT:
			*(DWORD*)buff = SD_GetSectorCount();
			res = RES_OK;
			break;
		
		default:
			res = RES_PARERR;
			break;
	}

    return res;
}
#endif
//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{
//	BYTE index = 0;
//	BYTE timeBuff[6] = {0};
//	DWORD returnTime = 66666;
//	RTC_GetTime(timeBuff);
//	for (index = 0; index < 6; index++)
//	{
//		returnTime += timeBuff[index]; 
//		returnTime <<= 4;
//	}
//	counter4 = returnTime;
//	return returnTime;
	return 0;
}			 
////��̬�����ڴ�
//void *ff_memalloc (UINT size)			
//{
//	return (void*)malloc(size);
//}
////�ͷ��ڴ�
//void ff_memfree (void* mf)		 
//{
//	free(mf);
//}
