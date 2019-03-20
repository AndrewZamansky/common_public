/*
 *
 * usb_audio_class.h

 *
 */

#ifndef _USB_AUDIO_CLASS_H_
#define _USB_AUDIO_CLASS_H_

/********  includes *********************/

#include "dev_management_api.h"

#include "_project_defines.h"


/* Define Terminal and Unit ID  */
#define PLAY_IT_ID              0x01
#define PLAY_OT_ID              0x03
#define PLAY_FEATURE_UNITID     0x06
#define REC_IT_ID               0x04
#define REC_OT_ID               0x02
#define REC_FEATURE_UNITID      0x05


//for changing sample rate(like in async mode)
#define  HOST_OUT_MAX_ADDITIONAL_SAMPLES   2

/********  types  *********************/

struct usb_audio_class_cfg_t {
	size_t   get_rx_buff_size;
	size_t   get_tx_buff_size;
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint32_t host_out_sample_rate_hz;
	uint32_t host_in_sample_rate_hz;
	uint8_t num_of_rx_bytes_per_sample;
	uint8_t num_of_tx_bytes_per_sample;
	uint8_t num_rx_channels;
	uint8_t num_tx_channels;
	uint8_t num_of_rx_buffers;
	uint8_t num_of_rx_over_under_flow_buffers_thr;
	uint8_t  playback_type;
	uint8_t  enable_recording;
} ;

struct usb_audio_class_runtime_t {
	uint8_t  curr_buff_indx;
	uint8_t  next_supplied_tx_buffer;
	uint8_t  next_supplied_rx_buffer;
	uint8_t  out_endpoint_num;
	uint8_t  in_endpoint_num;
	uint8_t  in_feedback_endpoint_num;
	uint8_t  *tx_buff;
	uint8_t  *tx_pckt_buff;
	uint8_t  **rx_buff;
	uint8_t  *rx_buff_status;
	uint16_t normal_host_in_pckt_size;
	uint32_t curr_pos_in_rx_buffer;
	uint32_t usb_feedback_sample_rate;
	size_t  curr_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
};
#endif
