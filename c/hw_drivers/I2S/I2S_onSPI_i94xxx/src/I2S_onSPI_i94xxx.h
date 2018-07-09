/*
 *
 * I2S_onSPI_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _I2S_onSPI_I94XXX_H_
#define _I2S_onSPI_I94XXX_H_

/********  includes *********************/

#include "src/_I2S_onSPI_i94xxx_prerequirements_check.h"

/********  types  *********************/

struct I2S_onSPI_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	size_t      base_address;
	uint32_t	clk_mode;
	uint32_t	sample_rate;
	uint32_t	data_width;
	uint32_t	audio_format;
	uint32_t	txrx_format;
	uint8_t     BCLK_pin;
	uint8_t     LRCLK_pin;
	uint8_t     DI_pin;
	uint8_t     DO_pin;
	uint8_t     MCLK_pin;

};

struct I2S_onSPI_i94xxx_runtime_t {
	size_t		empty;
};

#endif
