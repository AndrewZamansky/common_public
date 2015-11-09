
#ifndef _spi_flash_partition_manager_static_dev_macros_h_
#define _spi_flash_partition_manager_static_dev_macros_h_

#include "src/spi_flash_partition_manager.h"
uint8_t spi_flash_partition_manager_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t spi_flash_partition_manager_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
size_t spi_flash_partition_manager_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


#define __SPI_FLASH_PARTITION_MANAGER_API_CREATE_STATIC_DEV(dev,dev_name  ,server_dev )\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t server_dev ;						\
		spi_flash_partition_manager_Instance_t handle_of_##dev =	 {&server_dev};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				spi_flash_partition_manager_ioctl,		\
				spi_flash_partition_manager_pwrite,		\
				spi_flash_partition_manager_pread,		\
				DEV_API_dummy_callback_func				\
			}


#endif
