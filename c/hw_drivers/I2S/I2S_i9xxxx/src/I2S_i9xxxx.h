/*
 *
 * I2S_i9xxxx.h
 *
 *
 */

#ifndef _I2S_I9XXXX_H_
#define _I2S_I9XXXX_H_

#include "dev_management_api.h"

struct I2S_i9xxxx_runtime_t {
	uint32_t    actual_sample_rate;
	uint8_t init_done;
	struct dev_desc_t  *i9xxxx_i2s_clk_dev;
	struct dev_desc_t  *i9xxxx_I2S_MCLK_clk_dev;
	struct dev_desc_t  *i9xxxx_I2S_BCLK_clk_dev;
	struct dev_desc_t  *i9xxxx_I2S_FSCLK_clk_dev;
	i2s_interrupt_handler_t  int_handler;
};

SET_RUNTIME_DATA_TYPE(I2S_i9xxxx, struct I2S_i9xxxx_runtime_t);

#endif
