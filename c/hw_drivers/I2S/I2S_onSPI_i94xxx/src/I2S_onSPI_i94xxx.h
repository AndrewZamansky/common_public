/*
 *
 * I2S_onSPI_i94xxx.h
 *
 *
 */

#ifndef _I2S_onSPI_I94XXX_H_
#define _I2S_onSPI_I94XXX_H_

#include "dev_management_api.h"

struct I2S_onSPI_i94xxx_runtime_t {
	uint8_t                         init_done;
	i2s_on_spi_interrupt_handler_t  int_handler;
	uint8_t                         *in_buff;
	uint32_t                        overflow_counter;
};
SET_RUNTIME_DATA_TYPE(I2S_onSPI_i94xxx, struct I2S_onSPI_i94xxx_runtime_t);

#endif
