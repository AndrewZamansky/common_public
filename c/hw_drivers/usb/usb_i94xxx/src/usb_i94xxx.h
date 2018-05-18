/*
 *
 * usb_i94xxx.h
 *
 */

#ifndef _USB_I94XXX_H_
#define _USB_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct usb_i94xxx_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
};


#endif
