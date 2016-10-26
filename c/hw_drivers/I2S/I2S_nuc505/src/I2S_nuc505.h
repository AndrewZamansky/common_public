/*
 *
 * I2S_nuc505.h
 *
 *
 *
 *
 *
 */

#ifndef _I2S_NUC505_H_
#define _I2S_NUC505_H_

/********  includes *********************/

#include "src/_I2S_nuc505_prerequirements_check.h"

/********  types  *********************/

typedef struct
{
	uint8_t   	clock_mode;
	uint8_t		num_of_bytes_in_word;
	pdev_descriptor_t   callback_dev;
	uint32_t	sample_rate;
	uint16_t 	num_of_words_in_buffer_per_chenel ;
} I2S_nuc505_instance_t;

#endif
