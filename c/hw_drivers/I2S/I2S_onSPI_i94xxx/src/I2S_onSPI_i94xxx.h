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
	pdev_descriptor_t   src_clock;
	size_t   base_address;
	uint8_t   	clock_mode;
	uint32_t	sample_rate;
	uint8_t		num_of_bytes_in_word;
} ;

#endif
