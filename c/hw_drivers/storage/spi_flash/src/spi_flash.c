/*
 *
 * file :   SPI_FLASH.c
 *
 *
 *
 */


#include "spi_flash_api.h"
#include "gpio_api.h"
#include "spi_flash.h"
#include "spi_flash_add_component.h"


#define INSTANCE(hndl)	((struct spi_flash_instance_t*)hndl)


#define SPI_FLASH_PageSize_in_bits 8

#define SPI_FLASH_PageSize (1 << SPI_FLASH_PageSize_in_bits)

#define SPI_FLASH_PageSize_mask (SPI_FLASH_PageSize - 1)

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



#define uint16_low8(x)          ((x) & 0xFF)
#define uint16_high8(x)         (((x) >> 8) & 0xFF)


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : _chip_select_on
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void _chip_select_on(const void *apHandle   )
{
	/* Select the FLASH: Chip Select low */
	DEV_IOCTL_0_PARAMS(
			INSTANCE(apHandle)->gpio_select_dev , IOCTL_GPIO_PIN_CLEAR );
}

/*******************************************************************************
* Function Name  : _chip_select_off
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void _chip_select_off(const void *apHandle )
{
	/* Deselect the FLASH: Chip Select high */
	DEV_IOCTL_0_PARAMS(INSTANCE(apHandle)->gpio_select_dev , IOCTL_GPIO_PIN_SET );
}

/*******************************************************************************
* Function Name  : _write_to_flash_with_cs_toggle
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void _write_to_flash_with_cs_toggle(const void *apHandle , const uint8_t* apBuffer , uint32_t length )
{
	/* Select the FLASH: Chip Select low */
	_chip_select_on(apHandle);

	DEV_WRITE(INSTANCE(apHandle)->spi_server_dev , apBuffer , length);

	/* Deselect the FLASH: Chip Select high */
	_chip_select_off(apHandle );

}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(const void *apHandle)
{
	uint8_t write_data[1];
	write_data[0]= WREN;
	_write_to_flash_with_cs_toggle(apHandle , write_data , 1);
}


/*******************************************************************************
* Function Name  : addr_to_bytesBuffer
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void addr_to_bytesBuffer( uint8_t *addr,uint32_t StartAddr)
{
	/* Send startAddr high nibble address byte to read from */
	addr[0]= (StartAddr & 0xFF0000) >> 16;
	/* Send startAddr medium nibble address byte to read from */
	addr[1]= (StartAddr& 0xFF00) >> 8;
	/* Send startAddr low nibble address byte to read from */
	addr[2]= StartAddr & 0xFF;


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
	uint8_t write_data[1];
  uint8_t FLASH_Status = 0;
	struct dev_desc_t const *spi_server_dev = INSTANCE(apHandle)->spi_server_dev;

	/* Select the FLASH: Chip Select low */
	_chip_select_on(apHandle);

  /* Send "Read Status Register" instruction */
	write_data[0]= RDSR;
	DEV_WRITE(spi_server_dev , write_data , 1);





  /* Loop as long as the memory is busy with a write cycle */
  do
  {

   /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */

	DEV_READ(spi_server_dev , &FLASH_Status , 1);

  } while((FLASH_Status & WIP_Flag) == 1); /* Write in progress */

	/* Deselect the FLASH: Chip Select high */
	_chip_select_off(apHandle );
}


/*******************************************************************************
* Function Name  : _write_to_flash_with_cs_toggle
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void _write_to_flash_with_wen_and_wait(
		const void *apHandle , const uint8_t* apBuffer , uint32_t length )
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_WriteEnable(apHandle);

	_write_to_flash_with_cs_toggle(apHandle,apBuffer,length);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_WaitForWriteEnd(apHandle );

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
	uint8_t write_data[4];
	struct dev_desc_t const *spi_server_dev = INSTANCE(apHandle)->spi_server_dev;
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable(apHandle);

	/* Select the FLASH: Chip Select low */
	_chip_select_on(apHandle);

  /* Send "Write to Memory " instruction */
	write_data[0]= WRITE;

	addr_to_bytesBuffer(&write_data[1],WriteAddr);


	DEV_WRITE(spi_server_dev , write_data , 4);

	DEV_WRITE(spi_server_dev , pBuffer , NumByteToWrite);

	/* Deselect the FLASH: Chip Select high */
	_chip_select_off(apHandle );

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd(apHandle);
}



/*****************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
******************************************************************************/
uint32_t SPI_FLASH_ReadID(const void *apHandle)
{
	uint8_t write_data[3];
	uint32_t retVal = 0;
	struct dev_desc_t const *spi_server_dev =
							INSTANCE(apHandle)->spi_server_dev;

	/* Select the FLASH: Chip Select low */
	_chip_select_on(apHandle);

	/* Send "RDID " instruction */
	write_data[0]= 0x9F;
	DEV_WRITE(spi_server_dev , write_data , 1);

	DEV_READ(spi_server_dev , write_data , 3);

	retVal |= (uint32_t)write_data[0]<<16;
	retVal |= (uint32_t)write_data[1]<<8;
	retVal |= (uint32_t)write_data[2];

	/* Deselect the FLASH: Chip Select high */
	_chip_select_off(apHandle );


	return retVal;
}




/****************************************************************************
* Function Name  : spi_flash_pread
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
******************************************************************************/
#if UINT_MAX < 0xffffffff
uint32_t  spi_flash_pread(struct dev_desc_t *adev,
		uint8_t *apData, uint32_t length ,uint32_t startAddr)
#else
size_t  spi_flash_pread( struct dev_desc_t *adev,
		uint8_t *apData , size_t length ,size_t startAddr)
#endif
{
	struct spi_flash_instance_t *config_handle;
	struct dev_desc_t * spi_server_dev;
	uint8_t write_data[4];

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	spi_server_dev = config_handle->spi_server_dev;

	/* Select the FLASH: Chip Select low */
	_chip_select_on(config_handle);

	/* Send "Read from Memory " instruction */
	write_data[0]= READ;

	addr_to_bytesBuffer(&write_data[1],startAddr);

	DEV_WRITE(spi_server_dev , write_data , 4);

	DEV_READ(spi_server_dev , apData , length);

	/* Deselect the FLASH: Chip Select high */
	_chip_select_off(config_handle );

	return length;

}



/*****************************************************************************
* Function Name  : spi_flash_pwrite
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*****************************************************************************/
/*
 * 	size of flash is usualy 
 *    greater than 2^16 so address must be at least uint32_t
 */
#if UINT_MAX < 0xffffffff
uint32_t  spi_flash_pwrite( struct dev_desc_t *adev,
         const uint8_t *apData , uint32_t length ,uint32_t startAddr)
#else
size_t  spi_flash_pwrite( struct dev_desc_t *adev,
		const uint8_t *apData , size_t length ,size_t startAddr)
#endif
{
	struct spi_flash_instance_t *config_handle;
#if UINT_MAX < 0xffffffff
	uint32_t retVal  ;
#else
	size_t retVal  ;
#endif
	uint16_t alignmentBytes;

	retVal=length;


	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	
	/* in case startAddr is not aligned SPI_FLASH_PageSize   */
	alignmentBytes = ((uint16_t)startAddr) & SPI_FLASH_PageSize_mask;
	while(length)
	{
		uint16_t NumOfBytesToWrite;
		NumOfBytesToWrite = SPI_FLASH_PageSize - alignmentBytes;

		if(length < (uint32_t)NumOfBytesToWrite )
		{
		  NumOfBytesToWrite = (uint16_t)length;
		}

	//	if(NumOfBytesToWrite>100)
	//	{
	//		  while(1);
	//	}
		SPI_FLASH_PageWrite(config_handle ,apData, startAddr, NumOfBytesToWrite);
		startAddr +=  NumOfBytesToWrite;
		apData += NumOfBytesToWrite;
		length -=NumOfBytesToWrite;
		alignmentBytes = 0;
	}

	return retVal;
}



uint8_t spi_flash_ioctl( struct dev_desc_t *adev,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct spi_flash_instance_t *config_handle;
	uint8_t write_data[4];

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
		case IOCTL_DEVICE_START :
			/* Deselect the FLASH: Chip Select high */
			_chip_select_off(config_handle );
			DEV_IOCTL_0_PARAMS(config_handle->gpio_select_dev , IOCTL_DEVICE_START );
			DEV_IOCTL_0_PARAMS(config_handle->spi_server_dev , IOCTL_DEVICE_START );
//			{ // for spi test
//				uint32_t tmp=SPI_FLASH_ReadID(aHandle);
//			}

			break;

		case IOCTL_SPI_FLASH_ERRASE_ALL :
			write_data[0]= BE;
			_write_to_flash_with_wen_and_wait(config_handle , write_data , 1);

			break;

		case IOCTL_SPI_FLASH_ERRASE_SECTOR :
			write_data[0]= SE;
			addr_to_bytesBuffer(&write_data[1],*(uint32_t*)aIoctl_param1);
			_write_to_flash_with_wen_and_wait(config_handle , write_data , 4);

			break;

		default :
			return 1;
	}
	return 0;
}
