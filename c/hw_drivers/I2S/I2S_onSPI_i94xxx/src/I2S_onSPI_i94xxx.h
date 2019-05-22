/*
 *
 * I2S_onSPI_i94xxx.h
 *
 *
 */

#ifndef _I2S_onSPI_I94XXX_H_
#define _I2S_onSPI_I94XXX_H_

/********  includes *********************/
#include "dev_management_api.h" // for device manager defines and typedefs

/********  types  *********************/

struct I2S_onSPI_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	size_t      base_address;
	uint32_t    clk_mode;
	uint32_t    sample_rate;
	uint32_t    data_width;
	uint32_t    audio_format;
	uint32_t    txrx_format;
	uint16_t    Mclock_factor_based_on_FSclock;
	uint32_t     BCLK_pin;
	uint32_t     LRCLK_pin;
	uint32_t     DI_pin;
	uint32_t     DO_pin;
	uint32_t     MCLK_pin;

	// fix for uint8_t/uint16_t access if needed:
	uint8_t     do_reordering_for_16or8bit_channels;
};

struct I2S_onSPI_i94xxx_runtime_t {
	size_t   empty;
};

#endif
