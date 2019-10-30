
#ifndef _USB_Virtual_COM_API_H_
#define _USB_Virtual_COM_API_H_

#include "dev_management_api.h"
#include "uart_api.h"

enum USB_VIRTUAL_COM_API_ioctl_e {
	IOCTL_USB_VIRTUAL_COM_START = IOCTL_LAST_COMMON_UART_IOCTL + 1,
};

struct usb_virtual_com_class_cfg_t {
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_tx_dev;
	struct dev_desc_t *callback_rx_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint16_t  max_data_packet_size;
};
SET_CONFIG_TYPE(usb_virtual_com_class, struct usb_virtual_com_class_cfg_t);


#endif
