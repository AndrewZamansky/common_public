/*
 *
 * i2c_i94xxx.h

 *
 */

#ifndef _I2C_I94XXX_H_
#define _I2C_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"
#include "os_wrapper.h"

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
	uint8_t  curr_state;
	uint16_t curr_data_pos;
	uint8_t  rcv_data[I2C_I94XXX_RCV_DATA_SIZE_BUFFER];
	uint8_t  const  *tx_data;
	uint8_t  *rx_data;
	uint8_t  const *reg_addr_arr;
	uint8_t  reg_addr_left_to_transmit;
	size_t   tx_data_size;
	size_t   transmitted_data_size;
	size_t   size_of_data_to_receive;
	uint8_t   remote_slave_addr;
	uint8_t   device_access_tries;
	os_queue_t WaitQueue;
	os_mutex_t mutex;
};
#endif

typedef enum
{
	I2C_I94XXX_IDLE            = 0x01,
	I2C_I94XXX_DEVICE_ACCESS   = 0x02,
	I2C_I94XXX_ADDRESS_WRITE   = 0x03,
	I2C_I94XXX_DATA_WRITE      = 0x04,
	I2C_I94XXX_DATA_READ       = 0x05
} I2C_I94XXX_STATES_t;
