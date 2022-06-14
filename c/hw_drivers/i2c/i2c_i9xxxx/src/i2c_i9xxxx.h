/*
 *
 * i2c_i9xxxx.h

 *
 */

#ifndef _I2C_I9XXXX_H_
#define _I2C_I9XXXX_H_

#include "dev_management_api.h"
#include "os_wrapper.h"


#define		I2C_I9XXXX_RCV_DATA_SIZE_BUFFER	64

struct i2c_i9xxxx_runtime_t {
	uint16_t curr_data_pos;
	uint8_t  rcv_data[I2C_I9XXXX_RCV_DATA_SIZE_BUFFER];
	uint8_t  const  *tx_data;
	uint8_t  *rx_data;
	uint8_t  const *reg_addr_arr;
	uint8_t  reg_addr_arr_pos;
	uint8_t  reg_addr_size;
	size_t   tx_data_size;
	size_t   transmitted_data_size;
	size_t   size_of_data_to_receive;
	uint8_t   remote_slave_addr;
	uint8_t   device_access_tries;
	os_queue_t WaitQueue;
	os_mutex_t mutex;
	uint8_t  slave_address;
	uint8_t   i2c_error;
	uint8_t   init_done;
};
SET_RUNTIME_DATA_TYPE(i2c_i9xxxx, struct i2c_i9xxxx_runtime_t);

#endif
