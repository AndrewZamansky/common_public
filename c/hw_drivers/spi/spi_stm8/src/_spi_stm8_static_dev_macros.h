
#ifndef _spi_stm8_static_dev_macros_h_
#define _spi_stm8_static_dev_macros_h_

#include "src/spi_stm8.h"
uint8_t spi_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t spi_stm8_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
size_t spi_stm8_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


#define __SPI_STM8_API_CREATE_STATIC_DEV(pdev  )							\
		SPI_STM8_Instance_t handle_of_##pdev =	 {0};						\
		STATIC_DEVICE(pdev , &handle_of_##pdev , spi_stm8_ioctl , 			\
				spi_stm8_pwrite , spi_stm8_pread , DEV_API_dummy_callback_func)


#endif
