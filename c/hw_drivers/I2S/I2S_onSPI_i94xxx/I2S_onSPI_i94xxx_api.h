
#ifndef _I2S_onSPI_I94XXX_API_H_
#define _I2S_onSPI_I94XXX_API_H_

#include "src/_I2S_onSPI_i94xxx_prerequirements_check.h"
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
#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_PCMMSB   SPI_I2SFORMAT_PCMA
#define I2S_onSPI_I94XXX_API_TXRX_FORMAT_PCMLSB   SPI_I2SFORMAT_PCMB

//SPI1 : Port D pins
#define I2S_onSPI_I94XXX_API_BCLK_PIN_D4          1
#define I2S_onSPI_I94XXX_API_LRCLK_PIN_D5         2
#define I2S_onSPI_I94XXX_API_DI_PIN_D3            3
#define I2S_onSPI_I94XXX_API_DO_PIN_D2            4
#define I2S_onSPI_I94XXX_API_MCLK_PIN_D6          5

//SPI1 : Port C pins
#define I2S_onSPI_I94XXX_API_BCLK_PIN_C2          6
#define I2S_onSPI_I94XXX_API_LRCLK_PIN_C3         7
#define I2S_onSPI_I94XXX_API_DI_PIN_C1            8
#define I2S_onSPI_I94XXX_API_DO_PIN_C0            9
#define I2S_onSPI_I94XXX_API_MCLK_PIN_C4          10


/**********  define API  types ************/

typedef enum
{
	SPI_I2S_ENABLE_INPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	SPI_I2S_DISABLE_INPUT_IOCTL,
	SPI_I2S_ENABLE_OUTPUT_IOCTL,
	SPI_I2S_DISABLE_OUTPUT_IOCTL
} I2SonSPI_API_ioctl_t;


/**********  define API  functions  ************/

#endif
