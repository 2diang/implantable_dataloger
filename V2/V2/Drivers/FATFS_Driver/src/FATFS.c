//FATFS.c
/* USER CODE BEGIN PV */
#include "sys_Value.h"
#include "ff.h"
#include "FATFS.h"
#include "stdlib.h"

/* Private variables ---------------------------------------------------------*/
FATFS *fs;                 // Work area (file system object) for logical drive
FIL fil;                  // file objects
 
uint32_t byteswritten;                /* File write counts */
uint32_t bytesread;                   /* File read counts */
uint8_t wtext[] = "2222 \n3333 5555\n"; /* File write buffer */
uint8_t rtext[100];                     /* File read buffers */
char filename[] = "180103.txt";
FRESULT retSD;
/* USER CODE END PV */

/* USER CODE BEGIN 2 */
void FATFS_Test(void)
{
	fs = malloc(sizeof(FATFS));
 
    /*##-1- Register the file system object to the FatFs module ##############*/
    retSD = f_mount(fs, "", 0);
    if(retSD)
    {
        counter3 = 0;
    }
    else
        counter3 = 1;
     
    /*##-2- Create and Open new text file objects with write access ######*/
    retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(retSD)
        counter3 = 0;
    else
        counter3 = 2;
     
    /*##-3- Write data to the text files ###############################*/
    retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
    if(retSD)
        counter3 = 0;
    else
    {
        counter3 = 3;
    }
    /*##-4- Close the open text files ################################*/
    retSD = f_close(&fil);
    if(retSD)
       counter3 = 0;
    else
       counter3 = 4;
     
    /*##-5- Open the text files object with read access ##############*/
    retSD = f_open(&fil, filename, FA_READ);
    if(retSD)
        counter3 = 0;
    else
        counter3 = 5;
     
    /*##-6- Read data from the text files ##########################*/
    retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
    if(retSD)
       counter3 = 0;
    else
    {
        counter3 = 6;
    }
     
    /*##-7- Close the open text files ############################*/
    retSD = f_close(&fil);
    if(retSD)  
        counter3 = 0;
    else
        counter3 = 7;
     
    /*##-8- Compare read data with the expected data ############*/
    if(bytesread == byteswritten)
    { 
        counter3 = 666;
    }
	free(fs);
}
    
  /* USER CODE END 2 */