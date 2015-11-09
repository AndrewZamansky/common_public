/*
 *
 * file :   SPI_FLASH.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "global_typedefs.h"
#include "SPI_FLASH_api.h"
#include "SPI_FLASH.h"

/********  defines *********************/


#define SPI_FLASH_TIMEOUT  3000

#define SPI_FLASH_TASK_PRIORITY				TASK_NORMAL_PRIORITY

/* The check task uses the sprintf function so requires a little more stack. */
#define SPI_FLASH_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 150 )


/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/



/***********   local variables    **************/
static SPI_FLASH_Instance_t SPI_FLASH_InstanceParams = {0} ;






/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize 256

#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0x20//0xD8  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5


#define TOTAL_SIZE_OF_FLASH			(1024*1024) // 1M
#define SECTOR_SIZE					(512) //
#define TOTAL_NUM_OF_SECTORS		(TOTAL_SIZE_OF_FLASH / SECTOR_SIZE)

#define SECTORS_PER_TRACK			8 //
#define TOTAL_NUM_OF_CYLINDERS		(TOTAL_NUM_OF_SECTORS / SECTORS_PER_TRACK)

#define NUM_OF_RESERVED_CYLINDERS	6 // reserved cylenders for bufferring
#define REPORTED_NUM_OF_CYLINDERS	(TOTAL_NUM_OF_CYLINDERS - NUM_OF_RESERVED_CYLINDERS) // windows will not recognize less then 0x11
#define NUM_OF_HEADS				0x1 //

#define REPORTED_SECTORS_ON_STORAGE	   ((NUM_OF_HEADS * REPORTED_NUM_OF_CYLINDERS) * SECTORS_PER_TRACK)
#define REPORTED_SECTORS_ON_PARTITION   (REPORTED_SECTORS_ON_STORAGE - 1*(SECTORS_PER_TRACK)) // first block is MBR

#define uint16_low8(x)          ((x) & 0xFF)
#define uint16_high8(x)         (((x) >> 8) & 0xFF)


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(const void *apHandle)
{
  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();

  /* Send "Write Enable" instruction */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(WREN);

  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();
}


/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(const void *apHandle)
{
  uint8_t FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();

  /* Send "Read Status Register" instruction */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(RDSR);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {

    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */

    FLASH_Status =  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);

  } while((FLASH_Status & WIP_Flag) == 1); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(const void *apHandle,uint32_t SectorAddr)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable(apHandle);

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();
  /* Send Sector Erase instruction  */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(SE);
  /* Send SectorAddr high nibble address byte */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(apHandle);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(const void *apHandle)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable(apHandle);

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();
  /* Send Bulk Erase instruction  */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(BE);
  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(apHandle);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(const void *apHandle,const uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable(apHandle);

  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();

  /* Send "Write to Memory " instruction */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(WRITE);
  /* Send WriteAddr high nibble address byte to write to */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(WriteAddr & 0xFF);

  /* while there is data to be written on the FLASH */
  while(NumByteToWrite--)
  {
    /* Send the current byte */
	  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(apHandle);
}



/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
uint32_t SPI_FLASH_ReadID(const void *apHandle)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();

  /* Send "RDID " instruction */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(0x9F);

  /* Read a byte from the FLASH */
  Temp0 = ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
 Temp1 = ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp2 = ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}


/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t SPI_FLASH_ReadByte(const void *apHandle)
{

  return ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);
}



///*******************************************************************************
//* Function Name  : SPI_FLASH_SendHalfWord
//* Description    : Sends a Half Word through the SPI interface and return the
//*                  Half Word received from the SPI bus.
//* Input          : Half Word : Half Word to send.
//* Output         : None
//* Return         : The value of the received Half Word.
//*******************************************************************************/
//uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord)
//{
//  /* Loop while DR register in not emplty */
//  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);
//
//  /* Send Half Word through the SPI1 peripheral */
//  SPI_SendData(SPI1, HalfWord);
//
//  /* Wait to receive a Half Word */
//  while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);
//
//  /* Return the Half Word read from the SPI bus */
//  return SPI_ReceiveData(SPI1);
//}



/*******************************************************************************
* Function Name  : spi_flash_pread
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  spi_flash_pread(const void *apHandle ,uint8_t *apData , uint32_t length ,uint32_t startAddr)
{

  /* Select the FLASH: Chip Select low */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipSelect();

  /* Send "Read from Memory " instruction */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(READ);

  /* Send startAddr high nibble address byte to read from */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((startAddr & 0xFF0000) >> 16);
  /* Send startAddr medium nibble address byte to read from */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte((startAddr& 0xFF00) >> 8);
  /* Send startAddr low nibble address byte to read from */
  ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(startAddr & 0xFF);

  while(length--) /* while there is data to be read */
  {
	/* Read a byte from the FLASH */

	*apData = ((SPI_FLASH_Instance_t*)apHandle)->fSendByte(Dummy_Byte);
	/* Point to the next location where the byte read will be saved */
	apData++;
  }

  /* Deselect the FLASH: Chip Select high */
  ((SPI_FLASH_Instance_t*)apHandle)->fChipDeselect();

  return length;

}


/*******************************************************************************
* Function Name  : SPI_FLASH_WriteData
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  SPI_FLASH_WriteData(const void *apHandle,uint32_t startAddr,const uint8_t *apData , uint32_t length)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = startAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  length / SPI_FLASH_PageSize;
  NumOfSingle = length % SPI_FLASH_PageSize;

  if(Addr == 0) /* startAddr is SPI_FLASH_PageSize aligned  */
  {
	if(NumOfPage == 0) /* length < SPI_FLASH_PageSize */
	{
	  SPI_FLASH_PageWrite(apHandle,apData, startAddr, length);
	}
	else /* length > SPI_FLASH_PageSize */
	{
	  while(NumOfPage--)
	  {
		SPI_FLASH_PageWrite(apHandle,apData, startAddr, SPI_FLASH_PageSize);
		startAddr +=  SPI_FLASH_PageSize;
		apData += SPI_FLASH_PageSize;
	  }

	  SPI_FLASH_PageWrite(apHandle,apData, startAddr, NumOfSingle);
   }
  }
  else /* startAddr is not SPI_FLASH_PageSize aligned  */
  {
	if(NumOfPage== 0) /* length < SPI_FLASH_PageSize */
	{
	  if(NumOfSingle > count) /* (length + startAddr) > SPI_FLASH_PageSize */
	  {
		temp = NumOfSingle - count;

		SPI_FLASH_PageWrite(apHandle,apData, startAddr, count);
		startAddr +=  count;
		apData += count;

		SPI_FLASH_PageWrite(apHandle,apData, startAddr, temp);
	  }
	  else
	  {
		SPI_FLASH_PageWrite(apHandle,apData, startAddr, length);
	  }
	}
	else /* length > SPI_FLASH_PageSize */
	{
	  length -= count;
	  NumOfPage =  length / SPI_FLASH_PageSize;
	  NumOfSingle = length % SPI_FLASH_PageSize;

	  SPI_FLASH_PageWrite(apHandle,apData, startAddr, count);
	  startAddr +=  count;
	  apData += count;

	  while(NumOfPage--)
	  {
		SPI_FLASH_PageWrite(apHandle,apData, startAddr, SPI_FLASH_PageSize);
		startAddr +=  SPI_FLASH_PageSize;
		apData += SPI_FLASH_PageSize;
	  }

	  if(NumOfSingle != 0)
	  {
		SPI_FLASH_PageWrite(apHandle,apData, startAddr, NumOfSingle);
	  }
	}
  }
}

#define SPI_FLASH_SIZE   			(1024*1024)
#define SPI_FLASH_4K_BLOCKS_NUM   	(1024/4)
#define SPI_FLASH_BUFFER_ADDR   	(SPI_FLASH_SIZE - (4*1024))
#define SPI_FLASH_NON_EXISTENT_ADDR  (0xffffffff)

static uint32_t currWrite4KblockAddr = SPI_FLASH_NON_EXISTENT_ADDR;
static uint32_t dirty_512bytes_block;

#define TEMP_BUFF_SIZE 16
/*******************************************************************************
* Function Name  : SPI_FLASH_Copy_from_buffer_to_main_area
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  SPI_FLASH_Copy_from_buffer_to_main_area(const void *apHandle)
{
	uint32_t i,j;
	uint8_t buffer[TEMP_BUFF_SIZE];

	// fill bufer with current data from main storage
	for(i=0 ; 8>i ; i++)
	{
		if( 0==( dirty_512bytes_block & (1<<i)))
		{
			for(j=0 ; 512 > j; j=j+TEMP_BUFF_SIZE)
			{
				spi_flash_pread(apHandle , buffer , TEMP_BUFF_SIZE , currWrite4KblockAddr + (512 * i) + j);
				SPI_FLASH_WriteData(apHandle , SPI_FLASH_BUFFER_ADDR + (512 * i) + j , buffer , TEMP_BUFF_SIZE);
			}
		}
	}

	//erase sector in main storage area
	SPI_FLASH_SectorErase(apHandle,currWrite4KblockAddr);

	// transfer from buder to  main storage area
	for(i=0 ; 8>i ; i++)
	{
		for(j=0 ; 512 > j; j=j+TEMP_BUFF_SIZE)
		{
			spi_flash_pread(apHandle , buffer , TEMP_BUFF_SIZE, SPI_FLASH_BUFFER_ADDR + (512 * i) + j);
			SPI_FLASH_WriteData(apHandle , currWrite4KblockAddr + (512 * i) + j , buffer , TEMP_BUFF_SIZE);
		}
	}

	//erase fuffer sector
	SPI_FLASH_SectorErase(apHandle,SPI_FLASH_BUFFER_ADDR);

	currWrite4KblockAddr = SPI_FLASH_NON_EXISTENT_ADDR;
	dirty_512bytes_block=0;

}



/*******************************************************************************
* Function Name  : spi_flash_pwrite
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  spi_flash_pwrite(const void *apHandle ,const uint8_t *apData , uint32_t length ,uint32_t startAddr)
{
	uint32_t offset_in_4k_block;
	uint32_t next_dirty_512bytes_block;

	offset_in_4k_block = startAddr & 0xfff;// startAddr % 4k = startAddr % (2^12)
	startAddr = (startAddr & (~0xfff) ) ; //  startAddr in 4k blocks

	if(startAddr != currWrite4KblockAddr)
	{
		if(SPI_FLASH_NON_EXISTENT_ADDR != currWrite4KblockAddr)
		{
			SPI_FLASH_Copy_from_buffer_to_main_area(apHandle);
		}
	}

	currWrite4KblockAddr=startAddr;
	next_dirty_512bytes_block = (1<<(offset_in_4k_block >> 9)); //  offset_in_4k_block / 512

	if(next_dirty_512bytes_block & dirty_512bytes_block)
	{
		SPI_FLASH_Copy_from_buffer_to_main_area(apHandle);
	}

	dirty_512bytes_block |= next_dirty_512bytes_block;
	SPI_FLASH_WriteData(apHandle , SPI_FLASH_BUFFER_ADDR +  offset_in_4k_block , apData , 512);

	if( 0xff == dirty_512bytes_block)
	{
		SPI_FLASH_Copy_from_buffer_to_main_area(apHandle);
	}

	return length;
//
}


// (6*4k) will be saved
const uint8_t PartitionTable[16] =
{
	    0x00,// boot/non-boot signature
	    0x00, 0x01, 0x01, // CHS of first section in partition
	    0x01/*0x04*/, // FAT type
	    0x00, SECTORS_PER_TRACK , REPORTED_NUM_OF_CYLINDERS-1 ,  // CHS of last section in partition
	    SECTORS_PER_TRACK, 0x00,  0x00, 0x00, // LBA of first sector in partition
	    uint16_low8(REPORTED_SECTORS_ON_PARTITION),uint16_high8(REPORTED_SECTORS_ON_PARTITION),  0x00, 0x00,	// partition size																		 0x00, 0x00,
};

#define START_OF_PARTITION_SECTOR  0x08
#define SECTORS_PER_FAT  0x08
#define RESERVED_SECTORS  0x08
const uint8_t VBR[62] =
{
	0xEB, 0x3C, 0x90,
	'A', 'N', 'D', 'R', 'E', 'W', '_', 'Z',// system that created partition
	0x00, 0x02, // bytes per sector
	0x01, // logical sectors per claster
	uint16_low8(RESERVED_SECTORS),uint16_high8(RESERVED_SECTORS), // reserved logical sectors (we align to 4k )
	0x02,// number of fats
	0x00, 0x01, //number of root entries . 0x100=256 entries
	uint16_low8(REPORTED_SECTORS_ON_PARTITION),uint16_high8(REPORTED_SECTORS_ON_PARTITION),	// partition size																		 0x00, 0x00,
	0xF0, // partition type
	uint16_low8(SECTORS_PER_FAT),uint16_high8(SECTORS_PER_FAT), // sectrors per FAT
	uint16_low8(SECTORS_PER_TRACK),uint16_high8(SECTORS_PER_TRACK), // sector per track/header
	uint16_low8(NUM_OF_HEADS),uint16_high8(NUM_OF_HEADS), //number of heads
	0x01, 0x00, 0x00, 0x00,// hidden sectors (sectors before partition starts)

	0x00, 0x00, 0x00, 0x00, // total number of sectors
	0x80, 0x00, 0x29, 0x59, 0x25, 0xEB, 0x1B,
	'A' , 'N' , 'D' , 'R' , 'E' , 'W' , '_' , 'P' , 'R' , 'T' , ' ' ,
	'F' , 'A' , 'T' , '1' , '2' , ' ' , ' ' , ' ' ,
};

const uint8_t StartOfFatSection[3] =
{
		0xF8,0xFF,0xFF,
};

const uint8_t StartOfRootSection[32] =
{
		'A', 'N', 'D', 'R', 'E', 'W', ' ' , ' ' , ' ' , ' ' , ' ' , 0x08,  0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0x45,	0x2F, 0x45, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
};

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_Format                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  SPI_FLASH_Format(const void *apHandle)
{
	uint8_t *block_data;
	uint32_t i,j;
	block_data=(uint8_t *)malloc(0x200);// = 512

	// writing MBR
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0x1be],PartitionTable,16);
	block_data[0x1fe]=0x55;
	block_data[0x1ff]=0xaa;
	spi_flash_pwrite(apHandle, block_data , 0x200 ,0 );

	// writing VBR
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0],VBR,62);
	block_data[0x1fe]=0x55;
	block_data[0x1ff]=0xaa;
	spi_flash_pwrite(apHandle  , block_data , 0x200,512 * START_OF_PARTITION_SECTOR/*next 4k*/);

	// writing FAT
	for (j=0;j<2;j++)
	{
		memset(block_data ,0 , 0x200);
		memcpy(&block_data[0],StartOfFatSection,3);
		spi_flash_pwrite(apHandle ,  block_data , 0x200,
				512 * (START_OF_PARTITION_SECTOR + RESERVED_SECTORS +(j*SECTORS_PER_FAT)) );
		memset(block_data ,0 , 0x200);
		for(i=1;i<SECTORS_PER_FAT;i++)
		{
			spi_flash_pwrite(apHandle , block_data , 0x200,
					512 * (START_OF_PARTITION_SECTOR+ RESERVED_SECTORS + (j*SECTORS_PER_FAT) + i) );
		}
	}

	// writing root section
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0],StartOfRootSection,32);
	spi_flash_pwrite(apHandle , block_data , 0x200,
			512 * (START_OF_PARTITION_SECTOR + RESERVED_SECTORS +(2*SECTORS_PER_FAT))  );
	memset(block_data ,0 , 0x200);
	for(i=1;i<SECTORS_PER_FAT;i++)
	{
		spi_flash_pwrite(apHandle , block_data , 0x200,
				512 * (START_OF_PARTITION_SECTOR+ RESERVED_SECTORS + (2*SECTORS_PER_FAT) + i)  );
	}

	free(block_data);

	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:       SPI_FLASH_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void SPI_FLASH_Task( void *pvParameters )
{

	for( ;; )
	{
		vTaskDelay( 3000 );

		if(SPI_FLASH_NON_EXISTENT_ADDR != currWrite4KblockAddr)
		{
			SPI_FLASH_Copy_from_buffer_to_main_area(pvParameters);
		}

#if (1==INCLUDE_uxTaskGetStackHighWaterMark )
		{
			static  uint32_t stackLeft,minStackLeft=0xffffffff;

			stackLeft = uxTaskGetStackHighWaterMark( NULL );
			if(minStackLeft > stackLeft)
			{
				minStackLeft = stackLeft;
				PRINTF_DBG("%s stack left = %d\r\n" , __FUNCTION__ ,minStackLeft);
			}
		}
#endif

	}


}
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_start                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  SPI_FLASH_start()
{
	uint8_t mbr_signiture[2];
//	uint32_t id;

//	uint8_t buffer[TEMP_BUFF_SIZE*10];
	dirty_512bytes_block=0;

	switch(SPI_FLASH_InstanceParams.chip)
	{
		case SPI_FLASH_Chip_STM32F10x:
			SPI_FLASH_STM32F10x_Init(&SPI_FLASH_InstanceParams);
			break;
		default :
			return 1;
	}

//	id=SPI_FLASH_ReadID(&SPI_FLASH_InstanceParams);
//	SPI_FLASH_API_ReadData(&SPI_FLASH_InstanceParams , SPI_FLASH_BUFFER_ADDR , buffer , TEMP_BUFF_SIZE*10);
//	SPI_FLASH_SectorErase(&SPI_FLASH_InstanceParams,SPI_FLASH_BUFFER_ADDR);
//	SPI_FLASH_API_ReadData(&SPI_FLASH_InstanceParams , SPI_FLASH_BUFFER_ADDR , buffer , TEMP_BUFF_SIZE*10);

	spi_flash_pread(&SPI_FLASH_InstanceParams, mbr_signiture , 2 , 0x1fe);
	if( (0x55 != mbr_signiture[0]) ||  (0xAA != mbr_signiture[1]))
	{
		SPI_FLASH_Format(&SPI_FLASH_InstanceParams);
	}


	xTaskCreate( SPI_FLASH_Task, "SPI_FLASH_Task", SPI_FLASH_TASK_STACK_SIZE,(void*) &SPI_FLASH_InstanceParams,
			SPI_FLASH_TASK_PRIORITY , NULL );

	return 0;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_flash_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t spi_flash_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{

		case IOCTL_SPI_FLASH_SET_CHIP_TYPE :
			if (0 == memcmp((uint8_t*) aIoctl_param1 , "stm32f103" , sizeof("stm32f103") ))
			{
				SPI_FLASH_InstanceParams.chip = SPI_FLASH_Chip_STM32F10x;
			}
			break;

		case IOCTL_DEVICE_START :
			SPI_FLASH_start();
			break;

		case IOCTL_SPI_FLASH_GET_BLOCK_COUNT :
			*((uint32_t*)aIoctl_param1) = REPORTED_SECTORS_ON_STORAGE ; // block is 512bytes = 2^9 bytes
			break;

		case IOCTL_SPI_FLASH_GET_BLOCK_SIZE :
			*((uint32_t*)aIoctl_param1) = SECTOR_SIZE ;
			break;

		case IOCTL_SPI_FLASH_FORMAT :
			SPI_FLASH_Format(aHandle) ;
			break;

		case IOCTL_SPI_FLASH_FLUSH :
			if(SPI_FLASH_NON_EXISTENT_ADDR != currWrite4KblockAddr)
			{
				SPI_FLASH_Copy_from_buffer_to_main_area(aHandle);
			}
			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  SPI_FLASH_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &SPI_FLASH_InstanceParams;
	aDevDescriptor->ioctl = spi_flash_ioctl;
	aDevDescriptor->pread = spi_flash_pread;
	aDevDescriptor->pwrite = spi_flash_pwrite;

	return 0 ;

}
