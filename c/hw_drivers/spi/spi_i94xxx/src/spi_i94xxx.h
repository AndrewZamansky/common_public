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

struct spi_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t   clk_freq;
	size_t     base_address;
	uint32_t    CLK_pin;
	uint32_t    SS_pin;
	uint32_t    MISO_pin;
	uint32_t    MOSI_pin;
	uint8_t    data_width;
	uint8_t    rx_fifo_threshold;
	uint8_t    tx_fifo_threshold;

};


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
