
#ifndef _serial_number_stm8_API_H_
#define _serial_number_stm8_API_H_

#include "serial_number_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "serial_number_api.h"
#include "src/_serial_number_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_managment_api.h}

/**********  define API  types ************/
#define SERIAL_NUMBER_LEN  16


uint8_t  serial_number_stm8_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/serial_number_stm8_static_dev_macros.h"

#define SERIAL_NUMBER_STM8_API_CREATE_STATIC_DEV(dev,dev_name ) \
		__SERIAL_NUMBER_STM8_API_CREATE_STATIC_DEV(dev,dev_name )

#endif
