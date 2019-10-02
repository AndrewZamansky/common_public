/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "spi_flash_api.h"
#include "src/spi_flash.h"

uint8_t spi_flash_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

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

#define	MODULE_NAME					spi_flash
#define	MODULE_IOCTL_FUNCTION		spi_flash_ioctl
#define	MODULE_PWRITE_FUNCTION		(dev_pwrite_func_t)spi_flash_pwrite
#define	MODULE_PREAD_FUNCTION		(dev_pread_func_t)spi_flash_pread
#define MODULE_CONFIG_DATA_STRUCT_TYPE		spi_flash_instance_t

#ifdef DT_DEV_MODULE
	#ifndef SPI_FLASH_DT_SPI_SERVER_DEV
		#error "SPI_FLASH_DT_SPI_SERVER_DEV should be defined"
	#endif

	#ifndef SPI_FLASH_DT_GPIO_SERVER_DEV
		#error "SPI_FLASH_DT_GPIO_SERVER_DEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_SPI_SERVER_DEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_GPIO_SERVER_DEV) ;
	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			P_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_SPI_SERVER_DEV),	\
			P_TO_STATIC_DEVICE_INST(SPI_FLASH_DT_GPIO_SERVER_DEV)	\
		}
#endif

#include "add_static_dev.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef SPI_FLASH_DT_SPI_SERVER_DEV
#undef SPI_FLASH_DT_GPIO_SERVER_DEV
