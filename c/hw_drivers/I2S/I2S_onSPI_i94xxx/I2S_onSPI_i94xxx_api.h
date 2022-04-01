
#ifndef _I2S_onSPI_I94XXX_API_H_
#define _I2S_onSPI_I94XXX_API_H_

#include "dev_management_api.h"
#include "spi.h"

/*****************  defines  **************/
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI1   (size_t)SPI1_BASE
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI2   (size_t)SPI2_BASE

#define I2S_onSPI_I94XXX_API_MASTER_MODE         SPI_I2SMASTER
#define I2S_onSPI_I94XXX_API_SLAVE_MODE          SPI_I2SSLAVE

#define I2S_onSPI_I94XXX_API_DATA_WIDTH_8         SPI_I2SDATABIT_8
#define I2S_onSPI_I94XXX_API_DATA_WIDTH_16        SPI_I2SDATABIT_16
#define I2S_onSPI_I94XXX_API_DATA_WIDTH_24        SPI_I2SDATABIT_24
#define I2S_onSPI_I94XXX_API_DATA_WIDTH_32        SPI_I2SDATABIT_32

#define I2S_onSPI_I94XXX_API_AUDIO_MONO           SPI_I2SMONO
#define I2S_onSPI_I94XXX_API_AUDIO_STEREO         SPI_I2SSTEREO

#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_I2S      SPI_I2SFORMAT_I2S
#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_MSB      SPI_I2SFORMAT_MSB
#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_PCMA     SPI_I2SFORMAT_PCMA
#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_PCMB     (3 << SPI_I2SCTL_FORMAT_Pos)

#define I2S_onSPI_I9XXXX_API_DATA_TRANSFER_TYPE_INTERRUPT_HANDLER     0
#define I2S_onSPI_I9XXXX_API_DATA_TRANSFER_TYPE_DMA                   1


enum I2SonSPI_API_ioctl_e {
	SPI_I2S_ENABLE_INPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	SPI_I2S_DISABLE_INPUT_IOCTL,
	SPI_I2S_ENABLE_OUTPUT_IOCTL,
	SPI_I2S_DISABLE_OUTPUT_IOCTL,
	SPI_I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE,
	SPI_I2S_I9XXXX_SET_INTERRUPT_HANDLER,
	SPI_I2S_I9XXXX_GET_OVERFLOW_COUNTER
};

typedef void (*i2s_on_spi_interrupt_handler_t)(
					uint8_t *in_out_data, size_t in_out_data_len);

struct I2S_onSPI_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	size_t      base_address;
	uint32_t    clk_mode;
	uint32_t    sample_rate;
	uint32_t    data_width;
	uint32_t    audio_format;
	uint32_t    txrx_format;
	uint16_t    Mclock_factor_based_on_FSclock;
	uint32_t     BCLK_pin;
	uint32_t     LRCLK_pin;
	uint32_t     DI_pin;
	uint32_t     DO_pin;
	uint32_t     MCLK_pin;

	uint8_t  data_transfer_type;

	// fix for uint8_t/uint16_t access if needed:
	uint8_t     do_reordering_for_16or8bit_channels;
};
SET_CONFIG_TYPE(I2S_onSPI_i94xxx, struct I2S_onSPI_i94xxx_cfg_t);

#endif
