
#ifndef _DMIC_I94XXX_API_H_
#define _DMIC_I94XXX_API_H_

#include "I94100.h"
#include "dev_management_api.h"


enum  dmic_i94xxx_pinout_e {
	DMIC_I94XXX_DMIC_DATA0_A0,
	DMIC_I94XXX_DMIC_CLK0_A1,
	DMIC_I94XXX_DMIC_DATA1_A2,
	DMIC_I94XXX_DMIC_CLK1_A3,
};

enum DMIC_I94XXX_API_ioctl_e
{
	DMIC_I94XXX_ENABLE_INPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DMIC_I94XXX_DISABLE_INPUT_IOCTL
};


struct dmic_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t sample_rate;
	uint8_t num_of_channels;
	uint8_t dmic_data0_pin;
	uint8_t dmic_clk0_pin;
	uint8_t dmic_data1_pin;
	uint8_t dmic_clk1_pin;
};
SET_CONFIG_TYPE(dmic_i94xxx, struct dmic_i94xxx_cfg_t);


#endif
