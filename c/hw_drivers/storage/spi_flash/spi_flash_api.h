
#ifndef _SPI_FLASH_API_H_
#define _SPI_FLASH_API_H_

#include "src/_spi_flash_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_SPI_FLASH_GET_SIZE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SPI_FLASH_ERRASE_ALL,
	IOCTL_SPI_FLASH_ERRASE_SECTOR,
}spi_flash_api_ioctl_t;

uint8_t  spi_flash_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#endif
