/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "spi_stm8_api.h"
#include 	"src/spi_stm8.h"

uint8_t spi_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t spi_stm8_pwrite(struct dev_desc_t *adev ,const uint8_t *apData , size_t aLength, size_t aOffset);
size_t spi_stm8_pread(struct dev_desc_t *adev ,uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME					spi_stm8
#define	MODULE_IOCTL_FUNCTION		spi_stm8_ioctl
#define	MODULE_PWRITE_FUNCTION		spi_stm8_pwrite
#define	MODULE_PREAD_FUNCTION		spi_stm8_pread
#define MODULE_CONFIG_DATA_STRUCT_TYPE		struct spi_stm8_instance_t

#ifdef DT_DEV_MODULE
	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			0						\
		}
#endif

#include "add_static_dev.h"

