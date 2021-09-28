
#ifndef _I2C_API_H_
#define _I2C_API_H_

#include "dev_management_api.h"

enum i2c_common_ioctl_e {
	IOCTL_I2C_TURN_OFF = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_I2C_SET_MASTER_CLOCK_RATE_HZ,
	IOCTL_I2C_DISABLE_TX,
	IOCTL_I2C_ENABLE_TX,
	IOCTL_I2C_SET_ISR_CALLBACK_TX_DEV,
	IOCTL_I2C_SET_ISR_CALLBACK_RX_DEV,
	IOCTL_I2C_MASTER_WRITE,
	IOCTL_I2C_MASTER_READ,
//	IOCTL_I2C_FILTER_NON_ASCII_NOISE,
	IOCTL_LAST_COMMON_I2C_IOCTL
};

enum i2c_common_errors_e {
	I2C_OK = 0,
	I2C_DATA_NAK_ERROR,
	I2C_ARBITRATION_ERROR,
	I2C_BUS_ERROR,
	I2C_DEVICE_TIMEOUT,
	I2C_BAD_REG_ADDR_SIZE,
	I2C_NOT_INITIALIZED
};


struct i2c_api_master_read_t {
	size_t   num_of_bytes_to_read;
	size_t   num_of_bytes_that_was_read;
	uint8_t  *rx_data;
	size_t   reg_addr;
	uint8_t  reg_addr_size;
	uint8_t  device_addr_7bit;
	uint8_t  i2c_error;
};


struct i2c_api_master_write_t {
	size_t   num_of_bytes_to_write;
	size_t   num_of_bytes_written;
	uint8_t  const *tx_data;
	size_t   reg_addr;
	uint8_t  reg_addr_size;
	uint8_t  device_addr_7bit;
	uint8_t  i2c_error;
};


#define I2C_API_BAUD_RATE_STR "baud_rate"

#endif
