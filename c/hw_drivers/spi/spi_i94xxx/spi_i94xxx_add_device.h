/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "spi_i94xxx_api.h"
#include  "src/spi_i94xxx.h"


#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct spi_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct spi_i94xxx_runtime_t


#ifndef SPI_I94XXX_DT_BASE_ADDRESS
	#error "SPI_I94XXX_DT_BASE_ADDRESS should be defined"
#endif

#ifndef SPI_I94XXX_DT_CLK_FREQ
	#error "SPI_I94XXX_DT_CLK_FREQ should be defined"
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

#ifndef SPI_I94XXX_DT_DATA_WIDTH_BITS
	#error "SPI_I94XXX_DT_DATA_WIDTH_BITS should be defined"
#endif

#ifndef SPI_I94XXX_DT_RX_FIFO_THRESHOLD
	#error "SPI_I94XXX_DT_RX_FIFO_THRESHOLD should be defined"
#endif

#ifndef SPI_I94XXX_DT_TX_FIFO_THRESHOLD
	#error "SPI_I94XXX_DT_TX_FIFO_THRESHOLD should be defined"
#endif

#define STATIC_DEV_DATA_STRUCT      \
	{                               \
		POINTER_TO_SRC_CLOCK_PDEV,  \
		SPI_I94XXX_DT_CLK_FREQ,     \
		SPI_I94XXX_DT_BASE_ADDRESS, \
		SPI_I94XXX_DT_CLK_PIN,      \
		SPI_I94XXX_DT_SS_PIN,       \
		SPI_I94XXX_DT_MISO_PIN,     \
		SPI_I94XXX_DT_MOSI_PIN,     \
		SPI_I94XXX_DT_DATA_WIDTH_BITS,    \
		SPI_I94XXX_DT_RX_FIFO_THRESHOLD,     \
		SPI_I94XXX_DT_TX_FIFO_THRESHOLD    \
	}

#include "add_static_dev.h"

#undef  SPI_I94XXX_DT_CLK_RATE
#undef  SPI_I94XXX_DT_BASE_ADDRESS
#undef  SPI_I94XXX_DT_SRC_CLOCK_PDEV
#undef  POINTER_TO_SRC_CLOCK_PDEV
#undef  SPI_I94XXX_DT_CLK_PIN
#undef  SPI_I94XXX_DT_SS_PIN
#undef  SPI_I94XXX_DT_MISO_PIN
#undef  SPI_I94XXX_DT_MOSI_PIN
#undef  SPI_I94XXX_DT_DATA_WIDTH_BITS
#undef  SPI_I94XXX_DT_RX_FIFO_THRESHOLD
#undef  SPI_I94XXX_DT_TX_FIFO_THRESHOLD
