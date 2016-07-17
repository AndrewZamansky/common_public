#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "spi_flash_api.h"

#undef _add_static_device_step2

#undef SPI_FLASH_DT_DEV_NAME
#undef SPI_FLASH_DT_SPI_SERVER_DEV
#undef SPI_FLASH_DT_GPIO_SERVER_DEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef SPI_FLASH_DT_DEV_NAME
#error "SPI_FLASH_DT_DEV_NAME should be defined"
#endif

#ifndef SPI_FLASH_DT_SPI_SERVER_DEV
#error "SPI_FLASH_DT_SPI_SERVER_DEV should be defined"
#endif

#ifndef SPI_FLASH_DT_GPIO_SERVER_DEV
#error "SPI_FLASH_DT_GPIO_SERVER_DEV should be defined"
#endif


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


EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_SPI_SERVER_DEV) ;
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_GPIO_SERVER_DEV) ;
spi_flash_Instance_t STATIC_DEVICE_INNER_INST(SPI_FLASH_DT_DEV_NAME) =
	{
			P_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_SPI_SERVER_DEV),
			P_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_GPIO_SERVER_DEV)
	};
CREATE_STATIC_DEVICE(SPI_FLASH_DT_DEV_NAME , &STATIC_DEVICE_INNER_INST(SPI_FLASH_DT_DEV_NAME) ,
	spi_flash_ioctl ,
	(dev_pwrite_func_t)spi_flash_pwrite ,
	(dev_pread_func_t)spi_flash_pread ,
	DEV_API_dummy_callback_func);


#undef CURRENT_DEV

#endif
