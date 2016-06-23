
#ifndef _SPI_FLASH_API_H_
#define _SPI_FLASH_API_H_

#include "spi_flash_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_spi_flash_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_SPI_FLASH_GET_SIZE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SPI_FLASH_ERRASE_ALL,
	IOCTL_SPI_FLASH_ERRASE_SECTOR,
}spi_flash_api_ioctl_t;

uint8_t  spi_flash_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#include "src/spi_flash_static_dev_macros.h"


#define SPI_FLASH_API_CREATE_STATIC_DEV(dev,dev_name  ,spi_server_dev,gpio_server_dev ) \
			__SPI_FLASH_API_CREATE_STATIC_DEV(dev,dev_name  ,spi_server_dev ,gpio_server_dev)

#endif
