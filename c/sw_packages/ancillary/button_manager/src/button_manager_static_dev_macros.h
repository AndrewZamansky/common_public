
#ifndef _button_manager_static_dev_macros_h_
#define _button_manager_static_dev_macros_h_

#include "button_manager_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_button_manager_prerequirements_check.h" // should be after {button_manager_config.h,dev_management_api.h}

extern uint8_t button_manager_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,
		void* aIoctl_param1,void* aIoctl_param2);
extern uint8_t button_manager_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void* aCallback_param1, void* aCallback_param2);
#include "src/button_manager.h"


#endif
