
#ifndef _spi_stm32f10x_static_dev_macros_h_
#define _spi_stm32f10x_static_dev_macros_h_

#include "src/spi_stm32f10x.h"
uint8_t spi_stm32f10x_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t spi_stm32f10x_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
size_t spi_stm32f10x_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


#define __SPI_STM32F10X_API_CREATE_STATIC_DEV(dev,dev_name  )\
		extern const dev_descriptor_t dev ;						\
		SPI_STM32F10X_Instance_t handle_of_##dev =	 {0};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				spi_stm32f10x_ioctl,					\
				spi_stm32f10x_pwrite,					\
				spi_stm32f10x_pread,					\
				DEV_API_dummy_callback_func				\
			}


#endif
