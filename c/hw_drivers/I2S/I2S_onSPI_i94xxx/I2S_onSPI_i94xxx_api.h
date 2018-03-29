
#ifndef _I2S_onSPI_I94XXX_API_H_
#define _I2S_onSPI_I94XXX_API_H_

#include "src/_I2S_onSPI_i94xxx_prerequirements_check.h"
#include "spi.h"

/*****************  defines  **************/
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI1 (size_t)SPI1_BASE
#define I2S_onSPI_I94XXX_API_BASE_ADDRESS_SPI2 (size_t)SPI2_BASE

#define I2S_onSPI_I94XXX_API_MASTER_MODE   I2S_MODE_MASTER
#define I2S_onSPI_I94XXX_API_SLAVE_MODE    I2S_MODE_SLAVE

/**********  define API  types ************/

typedef enum
{
	I2S_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL+1
}I2S_API_ioctl_t;



/**********  define API  functions  ************/

#endif
