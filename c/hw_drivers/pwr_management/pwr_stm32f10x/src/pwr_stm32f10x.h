/*
 *
 * pwr_stm32f10x.h
 *
 */

#ifndef _PWR_STM32F10X_H_
#define _PWR_STM32F10X_H_

struct pwr_stm32f10x_runtime_t {
	uint8_t  init_done;
};
SET_RUNTIME_DATA_TYPE(pwr_stm32f10x, struct pwr_stm32f10x_runtime_t);


#endif
