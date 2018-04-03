
#ifndef _I2C_API_H_
#define _I2C_API_H_

#include "dev_management_api.h" // for device manager defines and typedefs

/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	IOCTL_I2C_TURN_OFF = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_I2C_SET_MASTER_CLOCK_RATE_HZ,
	IOCTL_I2C_DISABLE_TX,
	IOCTL_I2C_ENABLE_TX,
	IOCTL_I2C_SET_ISR_CALLBACK_TX_DEV,
	IOCTL_I2C_SET_ISR_CALLBACK_RX_DEV,
//	IOCTL_I2C_FILTER_NON_ASCII_NOISE,
	IOCTL_LAST_COMMON_I2C_IOCTL
}I2C_COMMON_API_ioctl_t;


#define I2C_API_BAUD_RATE_STR "baud_rate"

#endif
