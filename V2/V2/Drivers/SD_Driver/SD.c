// SD.c
//includes
#include "SD.h"
#include "spi.h"
// variables
uint8_t SD_Type = 0;
// function prototype
uint8_t SD_GetCSD(uint8_t *csd_data);
uint8_t SD_GetCID(uint8_t *cid_data);
uint8_t SD_SendBlock(uint8_t* buf, uint8_t cmd);
uint8_t SD_GetResponse(uint8_t Response);
uint8_t SD_RecvData(uint8_t* buf, uint16_t len);

uint8_t SD_Select(void);
void SD_DisSelect(void);
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
/*****************************************************************
*Function: SD_Init
*Description:
*Calls:
*Called By:
*Input:
*Output:
*Return:
*Others:
*****************************************************************/
uint8_t SD_Init(void)
{
	uint8_t resp; // the respond of the SD card
    uint16_t timeOut = 20;  // wait time
    uint8_t buffer[4]; // temp buffer
	uint16_t i; // index
	
    // 1. keep Baudrate of spi less than 400Khz during the Init
 	SPI2_LowSpeed();	// spi2 speed = 250KHz
 	for(i = 0; i < 10; i++)   // send 80 bytes to wait the sufficient voltage raise
		SPI2_ReadWriteByte(STUFF_BYTE);
	
	timeOut = 20;
	// 2. send command to enter SPI mode
	do
	{
		resp = SD_SendCmd(CMD0, 0, 0x95); // IDLE state of SPI 
	}while((resp != 0x01) && timeOut--); // 
	
	if(resp == 0x01)
	{
		if(SD_SendCmd(CMD8, 0x1AA, 0x87) == 0x01) //SD V2.0
		{
			for(i = 0; i < 4; i++)
				buffer[i] = SPI2_ReadWriteByte(STUFF_BYTE);	//Get trailing return value of R7 respond

			
			if((buffer[2] == 0x01) && (buffer[3] == 0xaa))// card supports 2.7V to 3.6V
			{
				timeOut = 0xfffe;
				do
				{
					SD_SendCmd(CMD55, 0, DUMMY_CRC);	// send cmd55 before sending an ACMD
					resp = SD_SendCmd(CMD41, 0x40000000, DUMMY_CRC);// cmd41 to check if it is 2GB or higher (30: HCS = 1)
				}while((resp != 0x00) && timeOut--);
				
				if(timeOut && (SD_SendCmd(CMD58, 0, DUMMY_CRC) == 0x00))//
				{
					for(i = 0; i < 4; i++)
						buffer[i] = SPI2_ReadWriteByte(STUFF_BYTE);//read REG ORC
					
					if((buffer[0] & 0x40) == 0x40)
						SD_Type = SD_TYPE_V2HC;    // > 2GB
					else 
						SD_Type = SD_TYPE_V2;    // <=2GB 
				}
			}
		}
		else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55, 0, DUMMY_CRC);		
			resp = SD_SendCmd(CMD41, 0, DUMMY_CRC);	 
			if(resp <= 1)
			{		
				SD_Type = SD_TYPE_V1;
				
				timeOut = 0xffee;
				do  // waiting for quit the IDLE state
				{
					SD_SendCmd(CMD55, 0, DUMMY_CRC);	
					resp = SD_SendCmd(CMD41, 0, DUMMY_CRC);
				}while(resp && timeOut--);
			}
			else//MMC 
			{
				SD_Type = SD_TYPE_MMC;//MMC V3
				
				timeOut = 0XFFFE;
				do // waiting for quit the IDLE state
				{											    
					resp = SD_SendCmd(CMD1, 0, DUMMY_CRC);
				}while(resp && timeOut--);  
			}
			if((timeOut == 0) || (SD_SendCmd(CMD16, 512, DUMMY_CRC) != 0))
				SD_Type = SD_TYPE_ERR; //  error card
		}
	}
	SD_DisSelect();
	SPI2_HighSpeed();
//	if(SD_Type == SD_TYPE_MMC)
//		return 0;
//	else 
//		if(resp)
//			return resp; 	   
//	return 0xaa; // other error
	return SD_Type;
	
}

uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t resp;	
	uint8_t timeOut = 0; 
	SD_DisSelect(); // make sure the CS = 1 before starting a new data transmition
	if(SD_Select() == SD_FAILURE)
		return MSD_FAIL;  
	// then send the command
    SPI2_ReadWriteByte(cmd | 0x40);// first byte : 01cmd
    SPI2_ReadWriteByte(arg >> 24); // arguement 
    SPI2_ReadWriteByte(arg >> 16);
    SPI2_ReadWriteByte(arg >> 8);
    SPI2_ReadWriteByte(arg);	  
    SPI2_ReadWriteByte(crc);       // CRC
	
	if(cmd == CMD12)
		SPI2_ReadWriteByte(STUFF_BYTE); //add an another stuff byte when stop reading
    // waiting for the response
	timeOut = 0x1f;
	do
	{
		resp = SPI2_ReadWriteByte(STUFF_BYTE);
	}while(((resp & 0x80) != 0) && timeOut--);	 // valid respond (0x80 = 1<<7)or timeout

    return resp;
}


void SD_DisSelect(void)
{
	SD_CS_Off(); // unsclect the last CS seletion 
 	SPI2_ReadWriteByte(STUFF_BYTE); //  extra 8 bytes clock
}

uint8_t SD_Select(void)
{
	SD_CS_On();
	if(SD_WaitReady() == SD_SUCCESS) // MOSI ready  
		return SD_SUCCESS;
	else
		SD_DisSelect();
	return SD_FAILURE;
}

uint8_t SD_WaitReady(void) // MOSI = 1 ready
{
	uint32_t t = 0;
	do
	{
		if(SPI2_ReadWriteByte(STUFF_BYTE) == 0xff) // the MISO is ready
			return SD_SUCCESS;
		t++;		  	
	}while(t < 0X00ffffff); // wait 
	return SD_FAILURE;
}

//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_ReadDisk(uint8_t* buf, uint32_t sector, uint8_t cnt)
{
	uint8_t resp;
	if(SD_Type != SD_TYPE_V2HC)
		sector <<= 9; // change 512byte address to 1byte address 2^9 = 512
	if(cnt == 1)// read one sector
	{
		resp = SD_SendCmd(CMD17, sector, DUMMY_CRC);//read CMD
		if(resp == 0)// CMD done
		{
			resp = SD_RecvData(buf, 512);//receive 512 bytes
		}
	}
	else // multiple sectors
	{
		resp = SD_SendCmd(CMD18, sector, DUMMY_CRC);//series read
		do
		{
			resp = SD_RecvData(buf, 512);//����512���ֽ�	 
			buf += 512;  
		}while(--cnt && resp == 0); 	
		SD_SendCmd(CMD12, 0, DUMMY_CRC);	//stop read 
	}   
	SD_DisSelect();
	if(resp == 0) // resp = 0 SD_SUCCESS , others SD_FAILURE
		return SD_SUCCESS;
	else
		return SD_FAILURE;
	
}

//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;	
uint8_t SD_RecvData(uint8_t* buf, uint16_t len)
{			  	  
	if(SD_GetResponse(RECV_START_TOKEN) == MSD_RESPONSE_FAILURE) 
		return SD_FAILURE; // 
    while(len--)//��ʼ��������
    {
        *buf = SPI2_ReadWriteByte(STUFF_BYTE);
        buf++;
    }
    //dummy CRC
    SPI2_ReadWriteByte(STUFF_BYTE);
    SPI2_ReadWriteByte(STUFF_BYTE);									  					    
   return SD_SUCCESS;//��ȡ�ɹ�
}

//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t timeOut = 0xFFFF;  
	while ((SPI2_ReadWriteByte(STUFF_BYTE)!= Response)&& (timeOut != 0))
		timeOut--;//�ȴ��õ�׼ȷ�Ļ�Ӧ  	  
	if (timeOut == 0)
		return MSD_RESPONSE_FAILURE;
	else 
		return MSD_RESPONSE_NO_ERROR;
}

//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_WriteDisk(uint8_t*buf, uint32_t sector, uint8_t cnt)
{
	uint8_t r1;
	if(SD_Type != SD_TYPE_V2HC)
		sector <<= 9;// change 512byte address to 1byte address 2^9 = 512
	if(cnt == 1) // write once
	{
		r1 = SD_SendCmd(CMD24, sector, DUMMY_CRC); //write CMD
		if(r1 == 0x00)//CMD done
		{
			r1 = SD_SendBlock(buf, TRANS_START_TOKEN);// write 512 bytes
		}
	}else
	{
		if(SD_Type != SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55, 0, DUMMY_CRC);	
			SD_SendCmd(CMD23, cnt, DUMMY_CRC); // pre-earse the blocks to be write  
		}
 		r1 = SD_SendCmd(CMD25, sector, DUMMY_CRC); // series write
		if(r1 == 0x00)
		{
			do
			{
				r1 = SD_SendBlock(buf, MUL_TRANS_START_TOKEN);// write 512 bytes
				buf += 512;  
			}while(--cnt && (r1 == SD_SUCCESS));
			r1 = SD_SendBlock(0, TRANS_END_TOKEN);// write 512 bytes
		}
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}
//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;

uint8_t SD_SendBlock(uint8_t* buf, uint8_t cmd)
{	
	uint16_t t;		  	  
	if(SD_WaitReady() == SD_FAILURE)
		return SD_FAILURE;
	SPI2_ReadWriteByte(cmd);// start read
	if(cmd != TRANS_END_TOKEN) 
	{
		for(t = 0; t < 512; t++) // send 512 bytes
			SPI2_ReadWriteByte(buf[t]);
		
	    SPI2_ReadWriteByte(STUFF_BYTE);
	    SPI2_ReadWriteByte(STUFF_BYTE);
		t = SPI2_ReadWriteByte(STUFF_BYTE);
		if((t & 0x1F) != 0x05) // the reponsed shall =  xxx00101 
			return SD_FAILURE;							  					    
	}						 									  					    
    return SD_SUCCESS;
}

//��ȡSD����CID��Ϣ��������������Ϣ
//����: uint8_t *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1������														   
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;	   
    //��CMD10�����CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����	 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}																				  
//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:uint8_t *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//		 1������														   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//����16���ֽڵ����� 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}  
//��ȡSD����������������������   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.														  
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;  
    uint8_t n;
	uint16_t csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {	
		csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
		Capacity = (uint32_t)csize << 10;//�õ�������	 		   
    }else//V1.XX�Ŀ�
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32_t)csize << (n - 9);//�õ�������   
    }
    return Capacity;
}