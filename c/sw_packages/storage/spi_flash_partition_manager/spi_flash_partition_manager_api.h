
#ifndef _SPI_FLASH_PARTITION_MANAGER_API_H_
#define _SPI_FLASH_PARTITION_MANAGER_API_H_

#include "spi_flash_partition_manager_config.h"
#include "dev_management_api.h"

enum SPI_FLASH_PARTITION_MANAGER_API_ioctl_e {
	IOCTL_SPI_FLASH_PARTITION_MANAGER_SET_CHIP_TYPE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_SPI_FLASH_PARTITION_MANAGER_FORMAT,
	IOCTL_SPI_FLASH_PARTITION_MANAGER_FLUSH,
	IOCTL_SPI_FLASH_PARTITION_MANAGER_GET_BLOCK_COUNT,
	IOCTL_SPI_FLASH_PARTITION_MANAGER_GET_BLOCK_SIZE,
};

uint8_t  spi_flash_partition_manager_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);


#include "src/spi_flash_partition_manager_static_dev_macros.h"

#define SPI_FLASH_PARTITION_MANAGER_API_CREATE_STATIC_DEV(dev,dev_name  ,server_dev ) \
			__SPI_FLASH_PARTITION_MANAGER_API_CREATE_STATIC_DEV(dev,dev_name  ,server_dev )


#endif
