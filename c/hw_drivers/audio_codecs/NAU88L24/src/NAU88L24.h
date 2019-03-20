/*
 * file : NAU88L24.h
 *
 *
 */

#ifndef _NAU88L24_H
#define _NAU88L24_H

#include "_project.h"
#include "dev_management_api.h"


/***************   typedefs    *******************/

struct NAU88L24_config_t
{
	struct dev_desc_t *   i2s_mclk_clock;
	struct dev_desc_t *   i2s_fsclk_clock;
	struct dev_desc_t    *i2c_bus_dev;
	uint8_t    dev_addr;
	uint8_t    pcm_data_width;
	uint8_t    input_type;
	uint8_t    output_type;
	uint8_t    num_of_digital_mics;
};



#endif /* */
