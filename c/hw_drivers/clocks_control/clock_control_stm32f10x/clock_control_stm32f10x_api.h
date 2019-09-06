
#ifndef _CLOCK_CONTROL_STM32f10X_API_H_
#define _CLOCK_CONTROL_STM32f10X_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/
extern struct dev_desc_t  *stm32f10x_xtal_clk_dev;
extern struct dev_desc_t  *stm32f10x_hsirc_clk_dev;
extern struct dev_desc_t  *stm32f10x_sysclk_dev;
extern struct dev_desc_t  *stm32f10x_hclk_clk_dev;
extern struct dev_desc_t  *stm32f10x_systick_clk_dev;
extern struct dev_desc_t  *stm32f10x_apb2_clk_dev;
extern struct dev_desc_t  *stm32f10x_adc_clk_dev;

#endif
