
#ifndef _I2C_I94XXX_API_H_
#define _I2C_I94XXX_API_H_

#include "I94100.h"

/*****************  defines  **************/

//#define I2C_I94XXX_API_BASE_ADDRESS_I2C0	(size_t)I2C0_BASE
#define I2C_I94XXX_API_BASE_ADDRESS_I2C1	(size_t)I2C1_BASE

#define  I2C_I94XXX_API_MASTER_MODE   0
#define  I2C_I94XXX_API_SLAVE_MODE    1

typedef enum {
	I2C_I94XXX_API_I2C1_SCL_PIN_PORT_D_PIN_2,
	I2C_I94XXX_API_I2C1_SCL_PIN_PORT_D_PIN_14,
} i2c_i94xxx_SCL_pinout;

typedef enum {
	I2C_I94XXX_API_I2C1_SDA_PIN_PORT_D_PIN_1,
	I2C_I94XXX_API_I2C1_SDA_PIN_PORT_D_PIN_15,
} i2c_i94xxx_SDA_pinout;

/**********  define API  types ************/



/**********  define API  functions  ************/

#endif
