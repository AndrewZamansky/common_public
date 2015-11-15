
#ifndef _SPI_FLASH_API_H_
#define _SPI_FLASH_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

//typedef void*  SPI_FLASH_API_Handle_t;
//
//typedef enum
//{
//	SPI_FLASH_API_Chip_STM32F10x=0,
//} SPI_FLASH_API_Chip_t;
//
//typedef struct {
//	SPI_FLASH_API_Chip_t chip;
//	void *spi_bus_handle;
//} SPI_FLASH_API_Init_params_t;
//
//
///**********  define API  functions  ************/
//uint8_t  SPI_FLASH_API_Init(const SPI_FLASH_API_Init_params_t *apInit_params,SPI_FLASH_API_Handle_t *apHandle);
//
//void  SPI_FLASH_API_ReadData(const void *apHandle,uint32_t startAddr, uint8_t *apData , uint32_t length);
//void  SPI_FLASH_API_WriteData(const void *apHandle,uint32_t startAddr,const uint8_t *apData , uint32_t length);
//
//uint32_t  SPI_FLASH_API_GetBlockCount(SPI_FLASH_API_Handle_t apHandle);
//uint32_t  SPI_FLASH_API_GetBlockSize(SPI_FLASH_API_Handle_t apHandle);
//
//uint8_t  SPI_FLASH_API_Flush(const void *apHandle);
//uint8_t  SPI_FLASH_API_Format(SPI_FLASH_API_Handle_t apHandle);

typedef enum
{
	IOCTL_SPI_FLASH_SET_CHIP_TYPE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SPI_FLASH_FORMAT,
	IOCTL_SPI_FLASH_FLUSH,
	IOCTL_SPI_FLASH_GET_BLOCK_COUNT,
	IOCTL_SPI_FLASH_GET_BLOCK_SIZE,
}SPI_FLASH_API_ioctl_t;

uint8_t  SPI_FLASH_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#else
#pragma message( "warning : this header file had already been included" )
#endif
