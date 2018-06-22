
#ifndef _I2S_onSPI_I94XXX_API_H_
#define _I2S_onSPI_I94XXX_API_H_

#include "src/_I2S_onSPI_i94xxx_prerequirements_check.h"
#include "spi.h"

/*****************  defines  **************/
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI1   (size_t)SPI1_BASE
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI2   (size_t)SPI2_BASE

#define I2S_onSPI_I94XXX_API_MASTER_MODE         SPI_I2SMASTER
#define I2S_onSPI_I94XXX_API_SLAVE_MODE          SPI_I2SSLAVE

#define I2S_onSPI_I94XXX_DT_DATA_WIDTH_8         SPI_I2SDATABIT_8
#define I2S_onSPI_I94XXX_DT_DATA_WIDTH_16        SPI_I2SDATABIT_16
#define I2S_onSPI_I94XXX_DT_DATA_WIDTH_24        SPI_I2SDATABIT_24
#define I2S_onSPI_I94XXX_DT_DATA_WIDTH_32        SPI_I2SDATABIT_32

#define I2S_onSPI_I94XXX_DT_AUDIO_FORMAT_MONO    SPI_I2SMONO
#define I2S_onSPI_I94XXX_DT_AUDIO_FORMAT_STEREO  SPI_I2SSTEREO

#define I2S_onSPI_I94XXX_DT_TXRX_FORMAT_I2S      SPI_I2SFORMAT_I2S
#define I2S_onSPI_I94XXX_DT_TXRX_FORMAT_MSB      SPI_I2SFORMAT_MSB
#define I2S_onSPI_I94XXX_DT_TXRX_FORMAT_PCMA     SPI_I2SFORMAT_PCMA
#define I2S_onSPI_I94XXX_DT_TXRX_FORMAT_PCMB     SPI_I2SFORMAT_PCMB

#define	SPI1_PD	1
#define	SPI1_PC	2
#define	SPI2_PA	3
#define	SPI2_PC	4

/**********  define API  types ************/

typedef enum
{
	I2S_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	I2S_DISABLE_OUTPUT_IOCTL
} I2SonSPI_API_ioctl_t;


/**********  define API  functions  ************/

#endif
