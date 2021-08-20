/*
 *
 * usb_audio_class.h

 *
 */

#ifndef _USB_AUDIO_CLASS_H_
#define _USB_AUDIO_CLASS_H_

#include "dev_management_api.h"

/* Define Terminal and Unit ID  */
#define PLAY_IT_ID              0x01
#define PLAY_OT_ID              0x03
#define PLAY_FEATURE_UNITID     0x06
#define REC_IT_ID               0x04
#define REC_OT_ID               0x02
#define REC_FEATURE_UNITID      0x05


//for changing sample rate(like in async mode)
#define  HOST_OUT_MAX_ADDITIONAL_SAMPLES   3


#define NUM_OF_PLAYBACK_CHANNELS   3 // 0 channel is master
#define NUM_OF_RECORDING_CHANNELS  3 // 0 channel is master


enum UAC_REQ_STATE {
	REQ_STATE_IDLE,
	REQ_STATE_GETTING_VOLUME,
	REQ_STATE_GETTING_MUTE,
};


struct usb_audio_class_runtime_t {
	uint8_t  curr_buff_indx;
	uint8_t  tx_buffer_was_supplied;
	uint8_t  next_supplied_rx_buffer;
	uint8_t  out_endpoint_num;
	uint8_t  in_endpoint_num;
	uint8_t  out_interface_num;
	uint8_t  in_interface_num;
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

	// UAC requests
	uint8_t playback_volumes_changed;
	uint8_t playback_mute_changed;
	uint8_t recording_volumes_changed;
	uint8_t recording_mute_changed;

	int16_t curr_playback_volume[NUM_OF_PLAYBACK_CHANNELS];
	int16_t curr_recording_volume[NUM_OF_RECORDING_CHANNELS];
	/* Record MUTE control. 0 = normal. 1 = MUTE */
	uint8_t recording_mute;
	/* Play MUTE control. 0 = normal. 1 = MUTE */
	uint8_t playback_mute;

	uint8_t request_state;
	uint8_t app_is_getting_data;

	uint8_t host_started_playback;
	uint8_t host_started_recording;
};
SET_RUNTIME_DATA_TYPE(usb_audio_class, struct usb_audio_class_runtime_t);

#endif
