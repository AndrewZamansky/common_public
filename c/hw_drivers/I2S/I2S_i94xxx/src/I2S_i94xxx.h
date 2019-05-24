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
	struct dev_desc_t  *src_clock;
	uint32_t    clock_mode;
	uint32_t    sample_rate;
	uint32_t    i2s_format;
	uint32_t    mono_or_stereo;
	uint16_t    Mclock_factor_based_on_FSclock;
	uint8_t     tdm_ch_num;
	uint32_t     BCLK_pin;
	uint32_t     LRCLK_pin;
	uint32_t     DI_pin;
	uint32_t     DO_pin;
	uint32_t     MCLK_pin;
	uint8_t     num_of_bytes_in_word;

	// fix for uint8_t/uint16_t access if needed:
	uint8_t     do_reordering_for_16or8bit_channels;

} ;

struct I2S_i94xxx_runtime_t {
	uint32_t    actual_sample_rate;
	uint8_t init_done;
};

#endif
