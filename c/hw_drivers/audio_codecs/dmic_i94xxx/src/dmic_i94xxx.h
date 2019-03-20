/*
 *
 * dmic_i94xxx.h
 *
 */

#ifndef _DMIC_I94XXX_H_
#define _DMIC_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct dmic_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t sample_rate;
	uint8_t num_of_channels;
	uint8_t dmic_data0_pin;
	uint8_t dmic_clk0_pin;
	uint8_t dmic_data1_pin;
	uint8_t dmic_clk1_pin;
};


#endif
