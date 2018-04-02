
#ifndef _I2S_I94XXX_API_H_
#define _I2S_I94XXX_API_H_

#include "dev_management_api.h"
#include "i2s.h"

/*****************  defines  **************/

#define I2S_I94XXX_API_MASTER_MODE    I2S_MASTER
#define I2S_I94XXX_API_SLAVE_MODE     I2S_SLAVE

#define I2S_I94XXX_API_DI_PIN_B13    0
#define I2S_I94XXX_API_DI_PIN_D4     1

#define I2S_I94XXX_API_DO_PIN_B14    0
#define I2S_I94XXX_API_DO_PIN_D5     1

#define I2S_I94XXX_API_MCLK_PIN_B15    0
#define I2S_I94XXX_API_MCLK_PIN_D2     1
#define I2S_I94XXX_API_MCLK_PIN_NONE   2

/**********  define API  types ************/

typedef enum
{
	I2S_I94XXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	I2S_I94XXX_GET_MEASURED_SAMPLE_RATE
}I2S_I94XXX_API_ioctl_t;



/**********  define API  functions  ************/

#endif
