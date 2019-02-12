/*
 *
 * usb_audio_class.h

 *
 */

#ifndef _USB_AUDIO_CLASS_H_
#define _USB_AUDIO_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"

/* Define Terminal and Unit ID  */
#define PLAY_IT_ID              0x01
#define PLAY_OT_ID              0x03
#define PLAY_FEATURE_UNITID     0x06
#define REC_IT_ID               0x04
#define REC_OT_ID               0x02
#define REC_FEATURE_UNITID      0x05

#define USB_AUDIO_CLASS_NUM_OF_BUFFERS   5

/********  types  *********************/

struct usb_audio_class_cfg_t {
	uint32_t   buff_size;
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint8_t num_of_bytes_per_sample;
	uint8_t  playback_type;
} ;

struct usb_audio_class_runtime_t {
	uint8_t  curr_buff_indx;
	uint8_t  num_of_data_ready_buffers;
	uint8_t  next_supplied_tx_buffer;
	uint8_t  next_supplied_rx_buffer;
	uint8_t  out_endpoint_num;
	uint8_t  in_endpoint_num;
	uint8_t  in_feedback_endpoint_num;
	uint8_t  *tx_buff;
	uint8_t  *tx_pckt_buff;
	uint8_t  *rx_buff[USB_AUDIO_CLASS_NUM_OF_BUFFERS];
	uint8_t  rx_buff_status[USB_AUDIO_CLASS_NUM_OF_BUFFERS];
	uint32_t curr_pos_in_rx_buffer;
	size_t  curr_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
};
#endif
