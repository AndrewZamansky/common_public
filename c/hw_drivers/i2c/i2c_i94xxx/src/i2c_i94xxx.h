/*
 *
 * i2c_i94xxx.h

 *
 */

#ifndef _I2C_I94XXX_H_
#define _I2C_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct i2c_i94xxx_cfg_t {
	struct dev_desc_t *callback_tx_dev;
	struct dev_desc_t *callback_rx_dev;
	struct dev_desc_t *src_clock;
	size_t   base_address;
	uint32_t baud_rate;
	uint8_t master_slave_mode;
	uint8_t SCL_pinout;
	uint8_t SDA_pinout;
	uint8_t slave_address;
} ;

#define		I2C_I94XXX_RCV_DATA_SIZE_BUFFER	64

struct i2c_i94xxx_runtime_t {
	uint16_t curr_data_pos;
	uint8_t  rcv_data[I2C_I94XXX_RCV_DATA_SIZE_BUFFER];
	 uint8_t const  *tx_data;
	size_t   tx_data_size;
	size_t   transmitted_data_size;
	uint8_t   remote_slave_addr;
};
#endif
