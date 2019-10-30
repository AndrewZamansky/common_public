
#ifndef _SPI_I94XXX_API_H_
#define _SPI_I94XXX_API_H_

#include "dev_management_api.h"

#define SPI_I94XXX_API_BASE_ADDRESS_SPI0    (size_t)SPI0_BASE
#define SPI_I94XXX_API_BASE_ADDRESS_SPI1    (size_t)SPI1_BASE


enum SPI_I94XXX_API_ioctl_e {
	IOCTL_SPI_API_SET_CS_HIGH = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SPI_API_SET_CS_LOW,
	IOCTL_SPI_API_SET_CLK
};

struct spi_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t   clk_freq;
	size_t     base_address;
	uint32_t    CLK_pin;
	uint32_t    SS_pin;
	uint32_t    MISO_pin;
	uint32_t    MOSI_pin;
	uint8_t    data_width;
	uint8_t    rx_fifo_threshold;
	uint8_t    tx_fifo_threshold;
};
SET_CONFIG_TYPE(spi_i94xxx, struct spi_i94xxx_cfg_t);

#endif
