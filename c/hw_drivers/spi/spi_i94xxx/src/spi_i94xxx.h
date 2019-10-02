/*
 *
 * spi_i94xxx.h
 *
 *
 */

#ifndef _SPI_I94XXX_H_
#define _SPI_I94XXX_H_

/********  includes *********************/

#include "os_wrapper.h"

/********  types  *********************/



struct spi_i94xxx_runtime_t {
	uint8_t  *rx_buff;
	uint32_t bytes_left_to_read;
	uint8_t  const *tx_buff;
	uint32_t bytes_left_to_transmit;
	os_queue_t xTX_WaitQueue;
	os_queue_t xRX_WaitQueue;
	os_mutex_t  spi_mutex;
};


#endif
