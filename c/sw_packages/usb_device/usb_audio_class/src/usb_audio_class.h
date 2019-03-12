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

#if !defined(BYTES_PER_PCM_CHANNEL)
	#error "BYTES_PER_PCM_CHANNEL should be defined in project header files"
#endif
#if !defined(NUM_OF_CHANNELS)
	#error "NUM_OF_CHANNELS should be defined in project header files"
#endif

#if !defined(USB_HOST_IN_SAMPLE_RATE)
	#error "USB_HOST_IN_SAMPLE_RATE  should be defined in project header files"
#endif
#if !defined(USB_HOST_OUT_SAMPLE_RATE)
	#define USB_HOST_OUT_SAMPLE_RATE  USB_HOST_IN_SAMPLE_RATE
	#warning "USB_HOST_OUT_SAMPLE_RATE set as USB_HOST_IN_SAMPLE_RATE"
#endif

#if ((48000 != USB_HOST_IN_SAMPLE_RATE) && (16000 != USB_HOST_IN_SAMPLE_RATE))
	#error "defined sample rate is not tested or not implemented"
#endif

#if ((48000 != USB_HOST_OUT_SAMPLE_RATE) && (16000 != USB_HOST_OUT_SAMPLE_RATE))
	#error "defined sample rate is not tested or not implemented"
#endif


#define NORMAL_AUDIO_HOST_IN_PACKET_SIZE  ((BYTES_PER_PCM_CHANNEL *       \
				USB_HOST_IN_SAMPLE_RATE * NUM_OF_CHANNELS) / 1000)// (per ms)

#define MAX_AUDIO_HOST_IN_PACKET_SIZE (NORMAL_AUDIO_HOST_IN_PACKET_SIZE \
							+ (NUM_OF_CHANNELS * BYTES_PER_PCM_CHANNEL))

#define NORMAL_AUDIO_HOST_OUT_PACKET_SIZE  ((BYTES_PER_PCM_CHANNEL *       \
				USB_HOST_OUT_SAMPLE_RATE * NUM_OF_CHANNELS) / 1000)// (per ms)

#define ADDITIONAL_SAMPLES_NUM   1//for changing sample rate(like in async mode)
#define MAX_AUDIO_HOST_OUT_PACKET_SIZE (NORMAL_AUDIO_HOST_OUT_PACKET_SIZE \
	+ (2 * ADDITIONAL_SAMPLES_NUM * NUM_OF_CHANNELS * BYTES_PER_PCM_CHANNEL))

/* Define Terminal and Unit ID  */
#define PLAY_IT_ID              0x01
#define PLAY_OT_ID              0x03
#define PLAY_FEATURE_UNITID     0x06
#define REC_IT_ID               0x04
#define REC_OT_ID               0x02
#define REC_FEATURE_UNITID      0x05


/********  types  *********************/

struct usb_audio_class_cfg_t {
	uint32_t   buff_size;
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *usb_descriptors_dev;
	uint8_t num_of_bytes_per_sample;
	uint8_t num_rx_channels;
	uint8_t num_tx_channels;
	uint8_t num_of_rx_buffers;
	uint8_t num_of_rx_over_under_flow_buffers_thr;
	uint8_t  playback_type;
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
	uint32_t curr_pos_in_rx_buffer;
	size_t  curr_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
	uint32_t usb_feedback_sample_rate;
};
#endif
