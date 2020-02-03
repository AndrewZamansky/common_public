#ifndef _PIN_CONTROL_API_H_
#define _PIN_CONTROL_API_H_

#include "dt_pin_control_stm32f10x.h"


uint8_t pin_control_api_set_pin_function(uint32_t  new_pin_info);

// can be send only from module that set pin function before
uint8_t pin_control_api_clear_pin_function(uint32_t  old_pin_info);

uint8_t pin_control_api_enable_input_schmitt_trigger(uint32_t  pin_info);

#endif
