
#ifndef _I2C_I94XXX_API_H_
#define _I2C_I94XXX_API_H_

#include "dev_management_api.h"
#include "I94100.h"

#define I2C_I94XXX_API_BASE_ADDRESS_I2C0  (size_t)I2C0_BASE
#define I2C_I94XXX_API_BASE_ADDRESS_I2C1  (size_t)I2C1_BASE

#define  I2C_I94XXX_API_MASTER_MODE   0
#define  I2C_I94XXX_API_SLAVE_MODE    1


struct i2c_i94xxx_cfg_t {
	struct dev_desc_t *callback_tx_dev;
	struct dev_desc_t *callback_rx_dev;
	struct dev_desc_t *src_clock;
	size_t   base_address;
	uint32_t baud_rate;
	uint8_t master_slave_mode;
	uint32_t SCL_pinout;
	uint32_t SDA_pinout;
	uint8_t slave_address;
};
SET_CONFIG_TYPE(i2c_i94xxx, struct i2c_i94xxx_cfg_t);


#endif
