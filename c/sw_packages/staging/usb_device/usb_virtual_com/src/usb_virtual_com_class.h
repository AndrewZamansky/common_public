/*
 *
 * usb_virtual_com_class.h

 *
 */

#ifndef _USB_VIRTUAL_COM_CLASS_H_
#define _USB_VIRTUAL_COM_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"


/********  types  *********************/

struct usb_virtual_com_class_cfg_t {
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint8_t endpoint_num;
} ;

#endif
