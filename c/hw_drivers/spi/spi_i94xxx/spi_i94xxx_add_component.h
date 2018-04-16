/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "spi_i94xxx_api.h"
#include  "src/spi_i94xxx.h"


#define	MODULE_NAME                     spi_i94xxx
#define	MODULE_IOCTL_FUNCTION           spi_i94xxx_ioctl
#define MODULE_CALLBACK_FUNCTION        spi_i94xxx_callback
#define	MODULE_PWRITE_FUNCTION          spi_i94xxx_pwrite
#define	MODULE_PREAD_FUNCTION           spi_i94xxx_pread
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct spi_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct spi_i94xxx_runtime_t


#ifdef DT_DEV_MODULE

	#ifndef SPI_I94XXX_DT_BASE_ADDRESS
		#error "SPI_I94XXX_DT_BASE_ADDRESS should be defined"
	#endif

	#ifndef SPI_I94XXX_DT_SRC_CLOCK_PDEV
		#error "SPI_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif
	#define POINTER_TO_SRC_CLOCK_PDEV     \
			P_TO_STATIC_DEVICE_INST(SPI_I94XXX_DT_SRC_CLOCK_PDEV)

	#ifndef SPI_I94XXX_DT_CLK_PIN
		#error "SPI_I94XXX_DT_CLK_PIN should be defined"
	#endif

	#ifndef SPI_I94XXX_DT_SS_PIN
		#error "SPI_I94XXX_DT_SS_PIN should be defined"
	#endif

	#ifndef SPI_I94XXX_DT_MISO_PIN
		#error "SPI_I94XXX_DT_MISO_PIN should be defined"
	#endif

	#ifndef SPI_I94XXX_DT_MOSI_PIN
		#error "SPI_I94XXX_DT_MOSI_PIN should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT      \
		{                               \
			POINTER_TO_SRC_CLOCK_PDEV,  \
			SPI_I94XXX_DT_BASE_ADDRESS, \
			SPI_I94XXX_DT_CLK_PIN,      \
			SPI_I94XXX_DT_SS_PIN,       \
			SPI_I94XXX_DT_MISO_PIN,     \
			SPI_I94XXX_DT_MOSI_PIN,     \
		}
#endif

#include "add_component.h"

#undef  SPI_I94XXX_DT_BASE_ADDRESS
#undef  SPI_I94XXX_DT_SRC_CLOCK_PDEV
#undef  POINTER_TO_SRC_CLOCK_PDEV
#undef  SPI_I94XXX_DT_CLK_PIN
#undef  SPI_I94XXX_DT_SS_PIN
#undef  SPI_I94XXX_DT_MISO_PIN
#undef  SPI_I94XXX_DT_MOSI_PIN
