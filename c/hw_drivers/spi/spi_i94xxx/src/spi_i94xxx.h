/*
 *
 * spi_i94xxx.h
 *
 *
 */

#ifndef _SPI_I94XXX_H_
#define _SPI_I94XXX_H_

/********  includes *********************/


/********  types  *********************/

struct spi_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	size_t   base_address;
	uint8_t    CLK_pin;
	uint8_t    SS_pin;
	uint8_t    MISO_pin;
	uint8_t    MOSI_pin;
	uint8_t    data_width;
};


struct spi_i94xxx_runtime_t {
	uint8_t  *rx_buff;
	uint32_t curr_rx_pos;
	uint8_t  const *tx_buff;
	uint32_t bytes_left_to_transmit;
};


#endif
