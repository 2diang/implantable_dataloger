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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//FATFS disio.c 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	BYTE res = 0;	    

	res = SD_Init();
	if(res == SD_TYPE_ERR)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
	{
		SPI2_LowSpeed();
		SPI2_ReadWriteByte(STUFF_BYTE);//提供额外的8个时钟
		SPI2_HighSpeed();
	}		 
	if(res == SD_TYPE_ERR)
		return  STA_NOINIT;
	else 
		return 0; //初始化成功
}  

//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

//读扇区
//drv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	BYTE res = 0; 
    if (count == 0)
		return RES_PARERR; //count不能等于0，否则返回参数错误		 	 

	res = SD_ReadDisk(buff,sector,count);	 
	if(res == SD_FAILURE)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
	{
		SPI2_LowSpeed();
		SPI2_ReadWriteByte(STUFF_BYTE);//提供额外的8个时钟
		SPI2_HighSpeed();
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == SD_SUCCESS)
		return RES_OK;	 
    else 
		return RES_ERROR;	   
}

//写扇区
//drv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
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
		return RES_PARERR;//count不能等于0，否则返回参数错误		 	 

	res = SD_WriteDisk((BYTE*)buff,sector,count);

    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == SD_SUCCESS)
		return RES_OK;	 
    else 
		return RES_ERROR;	
}
#endif


//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
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
//获得时间
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
////动态分配内存
//void *ff_memalloc (UINT size)			
//{
//	return (void*)malloc(size);
//}
////释放内存
//void ff_memfree (void* mf)		 
//{
//	free(mf);
//}
