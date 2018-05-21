/*
 *
 * usb_audio_class.h

 *
 */

#ifndef _USB_AUDIO_CLASS_H_
#define _USB_AUDIO_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"

#define USB_AUDIO_CLASS_NUM_OF_BUFFERS   2

/********  types  *********************/

struct usb_audio_class_cfg_t {
	uint32_t   buff_size;
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint8_t endpoint_num;
	uint8_t num_of_bytes_per_sample;
} ;

struct usb_audio_class_runtime_t {
	uint8_t  curr_buff_indx;
	uint8_t  next_supplied_tx_buffer;
	uint8_t  next_supplied_rx_buffer;
	uint8_t  *buff[USB_AUDIO_CLASS_NUM_OF_BUFFERS];
	uint8_t  buff_status[USB_AUDIO_CLASS_NUM_OF_BUFFERS];
	uint32_t curr_pos_in_buffer;
	int sample_to_skip_or_repeat;
};
#endif
