/*
 *
 * I2S_i94xxx.h
 *
 *
 */

#ifndef _I2S_I94XXX_H_
#define _I2S_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct I2S_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t   	clock_mode;
	uint32_t	sample_rate;
	uint32_t	i2s_format;
	uint8_t		num_of_bytes_in_word;
	uint8_t		BCLK_pin;
	uint8_t		LRCLK_pin;
	uint8_t		DI_pin;
	uint8_t		DO_pin;
	uint8_t		MCLK_pin;
} ;

struct I2S_i94xxx_runtime_t {
	uint32_t	actual_sample_rate;
};

#endif
