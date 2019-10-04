/*
 *
 * usb_hid_class.h

 *
 */

#ifndef _USB_HID_CLASS_H_
#define _USB_HID_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct usb_hid_class_cfg_t {
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_tx_dev;
	struct dev_desc_t *callback_rx_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint16_t  max_host_in_data_packet_size;
	uint16_t  max_host_out_data_packet_size;
};

struct usb_hid_class_runtime_t {
	size_t  sentLen;
	uint8_t in_endpoint_num;
	uint8_t tx_on;
	uint8_t init_done;
};
#endif
