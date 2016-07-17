#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "spi_stm8_api.h"

#undef _add_static_device_step2

#undef SPI_STM8_DT_DEV_NAME


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef SPI_STM8_DT_DEV_NAME
#error "SPI_STM8_DT_DEV_NAME should be defined"
#endif


#include "src/spi_stm8.h"
uint8_t spi_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t spi_stm8_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
size_t spi_stm8_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


SPI_STM8_Instance_t STATIC_DEVICE_INNER_INST(SPI_STM8_DT_DEV_NAME) =	 {0};
CREATE_STATIC_DEVICE(SPI_STM8_DT_DEV_NAME , &STATIC_DEVICE_INNER_INST(SPI_STM8_DT_DEV_NAME) ,
	spi_stm8_ioctl , spi_stm8_pwrite ,
	spi_stm8_pread , DEV_API_dummy_callback_func);



#undef CURRENT_DEV

#endif
