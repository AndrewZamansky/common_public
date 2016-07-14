
#ifndef _button_manager_API_H_
#define _button_manager_API_H_

#include "button_manager_config.h"
#include "src/_button_manager_prerequirements_check.h" // should be after {button_manager_config.h,dev_management_api.h}

/*****************  defines  **************/


/**********  define API  types ************/




#define	BTN_REPORT_PUSH					0x01
#define	BTN_REPORT_SHORT_RELEASE		0x02
#define	BTN_REPORT_HOLD_EVERY_0s1		0x04
#define	BTN_REPORT_HOLD_EVERY_1s		0x08
#define	BTN_REPORT_HOLD_ONCE_5s			0x10
#define	BTN_REPORT_RELEASE_AFTER_5s		0x20



typedef struct
{

	uint8_t		report_config;
	void	 	*callback_private_data;
	uint8_t		*action_state;

} btn_action_t;

typedef struct
{
	uint8_t num_of_buttons;
	pdev_descriptor_t   *server_dev;
} buttons_manager_set_group_servers_devs_t;

typedef struct
{

	uint8_t num_of_actons;
	btn_action_t *btn_actions;

} buttons_manager_set_group_actions_t;

typedef enum
{
	IOCTL_BUTTON_MANAGER_SET_GROUP_SERVER_DEVS = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_BUTTON_MANAGER_SET_GROUP_IDLE_STATE,
	IOCTL_BUTTON_MANAGER_SET_GROUP_ACTIONS
}BUTTON_MANAGER_API_ioctl_t;

/**********  define API  functions  ************/

uint8_t  button_manager_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);

#include "src/button_manager_static_dev_macros.h"

#define BUTTON_MANAGER_API_CREATE_STATIC_DEV(dev,dev_name ) \
			__BUTTON_MANAGER_API_CREATE_STATIC_DEV(dev,dev_name )


#endif
