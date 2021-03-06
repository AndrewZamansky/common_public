/*
 *
 * I2S_nuc505.h
 *
 *
 */

#ifndef _I2S_NUC505_H_
#define _I2S_NUC505_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct I2S_nuc505_cfg_t {
	//struct dev_desc_t	*src_clock;
	uint8_t   	clock_mode;
	uint8_t		num_of_bytes_in_word;
	struct dev_desc_t *   callback_dev;
	struct dev_desc_t *   timer_dev;
	uint32_t	sample_rate;
	uint16_t 	num_of_words_in_buffer_per_chenel ;
};

#endif
