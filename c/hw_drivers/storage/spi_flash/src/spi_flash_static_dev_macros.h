
#ifndef _spi_flash_static_dev_macros_h_
#define _spi_flash_static_dev_macros_h_

#include "src/spi_flash.h"
uint8_t spi_flash_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "limits.h"
/*
 * 	size of flash is usualy greater than 2^16 so address must be at least uint32_t
 */
#if UINT_MAX < 0xffffffff
	uint32_t spi_flash_pwrite(const void *aHandle ,const uint8_t *apData , uint32_t aLength, uint32_t aOffset);
	uint32_t spi_flash_pread(const void *aHandle ,uint8_t *apData , uint32_t aLength, uint32_t aOffset);
#else
	size_t spi_flash_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
	size_t spi_flash_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);
#endif


#define __SPI_FLASH_API_CREATE_STATIC_DEV(dev,dev_name  ,spi_server_dev ,gpio_server_dev)\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t spi_server_dev ;						\
		extern const dev_descriptor_t gpio_server_dev ;						\
		spi_flash_Instance_t handle_of_##dev =	 {&spi_server_dev,&gpio_server_dev};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				spi_flash_ioctl,						\
				(dev_pwrite_func_t)spi_flash_pwrite,	\
				(dev_pread_func_t)spi_flash_pread,		\
				DEV_API_dummy_callback_func				\
			}

#endif
