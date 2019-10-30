/*
 *
 * spi_i94xxx.h
 *
 *
 */

#ifndef _SPI_I94XXX_H_
#define _SPI_I94XXX_H_

#include "dev_management_api.h"
#include "os_wrapper.h"

struct spi_i94xxx_runtime_t {
	uint8_t  *rx_buff;
	uint32_t bytes_left_to_read;
	uint8_t  const *tx_buff;
	uint32_t bytes_left_to_transmit;
	os_queue_t xTX_WaitQueue;
	os_queue_t xRX_WaitQueue;
	os_mutex_t  spi_mutex;
};
SET_RUNTIME_DATA_TYPE(spi_i94xxx, struct spi_i94xxx_runtime_t);


#endif
