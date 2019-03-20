
#ifndef _SPI_FLASH_API_H_
#define _SPI_FLASH_API_H_


/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_SPI_FLASH_GET_SIZE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SPI_FLASH_ERRASE_ALL,
	IOCTL_SPI_FLASH_ERRASE_SECTOR,
}spi_flash_api_ioctl_t;

#endif
