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
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	struct dev_desc_t *   src_clock;
	size_t   base_address;
	uint32_t baud_rate;
	uint32_t pinout;
	uint32_t slave_address;
} ;

#define		I2C_I94XXX_RCV_DATA_SIZE_BUFFER	64
struct i2c_i94xxx_runtime_t {
	uint16_t curr_data_pos;
	uint8_t	rcv_data[I2C_I94XXX_RCV_DATA_SIZE_BUFFER];
};
#endif
