
#ifndef _USB_I94XXX_API_H_
#define _USB_I94XXX_API_H_

#include "dev_management_api.h"

struct usb_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
};
SET_CONFIG_TYPE(usb_i94xxx, struct usb_i94xxx_cfg_t);


#endif
