#ifndef _NAU88L24_API_H
#define _NAU88L24_API_H

#include "dev_management_api.h"

#define  NAU88L24_Input_type_none   1
#define  NAU88L24_Input_type_stereo_aux  2
#define  NAU88L24_Input_type_MicM   3
#define  NAU88L24_Input_type_MicP   4
#define  NAU88L24_Input_type_DMIC   5


enum NAU88L24_Output_type {
	NAU88L24_Output_type_none,
	NAU88L24_Output_type_headset
};

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
SET_CONFIG_TYPE(NAU88L24, struct NAU88L24_config_t);

#endif
