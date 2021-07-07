/******************************************************************************
 * @file     usb_audio_class.c
 * @brief
 *
 * @note
 *
 ******************************************************************************/

/*!<Includes */
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors_api.h"
#include "os_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_audio_class_api.h"
#include "usb_audio_class.h"



/***************************************************/
/*!<Define Audio Class Specific Request */

#define BM_REQ_TYPE_POS          0
#define B_REQUEST_POS            1
#define W_VALUE_LOW_POS          2
#define W_VALUE_HIGH_POS         3
#define W_INDEX_LOW_POS          4
#define W_INDEX_HIGH_POS         5
#define INTERFACE_ID_POS    W_INDEX_LOW_POS
#define ENDPOINT_ID_POS     W_INDEX_LOW_POS
#define FEAURE_UNIT_ID      W_INDEX_HIGH_POS
#define ENDPOINT_CONTROL_SELECTOR_POS   W_VALUE_HIGH_POS
#define INTERFACE_CONTROL_SELECTOR_POS  W_VALUE_HIGH_POS
#define FEATURE_CHANNEL_SELECTOR_POS  W_VALUE_LOW_POS
#define REQ_DIRECTION_MASK    0x80
#define REQ_TYPE_MASK         0x60
#define REQ_TYPE_CLASS        0x20
#define REQ_RECIPIENT_MASK    0x1f
#define REQ_RECIPIENT_INTERFACE    0x01
#define REQ_RECIPIENT_ENDPOINT     0x02


#define UAC_REQUEST_CODE_UNDEFINED  0x00
#define UAC_SET_CUR                 0x01
#define UAC_GET_CUR                 0x81
#define UAC_SET_MIN                 0x02
#define UAC_GET_MIN                 0x82
#define UAC_SET_MAX                 0x03
#define UAC_GET_MAX                 0x83
#define UAC_SET_RES                 0x04
#define UAC_GET_RES                 0x84
#define UAC_SET_MEM                 0x05
#define UAC_GET_MEM                 0x85
#define UAC_GET_STAT                0xFF

#define MUTE_CONTROL                0x01
#define VOLUME_CONTROL              0x02
#define SAMPLING_FREQ_CONTROL       0x01




static volatile uint32_t g_usbd_PlaySampleRate = 48000;


static volatile int16_t g_usbd_RecMaxVolume  = 0x0000; // 0db
static volatile int16_t g_usbd_RecMinVolume  = 0xc000; // -127db
static volatile int16_t g_usbd_RecResVolume  = 0x400;

static volatile int16_t g_usbd_PlayMaxVolume = 0x0000; // 0db
static volatile int16_t g_usbd_PlayMinVolume = 0xc000; // -127db
static volatile int16_t g_usbd_PlayResVolume = 0x0100;


#define MAX_DATA_TO_SEND_LEN 2
static uint8_t data_to_send[MAX_DATA_TO_SEND_LEN];


static uint8_t get_mute(struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t unit_id, uint8_t **data, uint16_t *data_len)
{
	if (REC_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&runtime_hndl->recording_mute;
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&runtime_hndl->playback_mute;
	}
	else
	{
		return 1;
	}
	*data_len = 1;
	return 0;
}


static uint8_t get_volume(struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t unit_id, uint8_t channel_id, uint8_t **data, uint16_t *data_len)
{
	if (REC_FEATURE_UNITID == unit_id)
	{
		if (NUM_OF_PLAYBACK_CHANNELS <= channel_id)
		{
			return 1;
		}
		*data = (uint8_t*)&runtime_hndl->curr_playback_volume[channel_id];
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		if (NUM_OF_RECORDING_CHANNELS <= channel_id)
		{
			return 1;
		}
		*data = (uint8_t*)&runtime_hndl->curr_recording_volume[channel_id];
	}
	else
	{
		return 1;
	}
	*data_len = 2;
	return 0;
}


static uint8_t get_min_volume(
		uint8_t unit_id, uint8_t **data, uint16_t *data_len)
{
	if (REC_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_RecMinVolume;
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_PlayMinVolume;
	}
	else
	{
		return 1;
	}
	*data_len = 2;
	return 0;
}


static uint8_t get_max_volume(
		uint8_t unit_id, uint8_t **data, uint16_t *data_len)
{
	if (REC_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_RecMaxVolume;
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_PlayMaxVolume;
	}
	else
	{
		return 1;
	}
	*data_len = 2;
	return 0;
}


static uint8_t get_res_volume(
			uint8_t unit_id, uint8_t **data, uint16_t *data_len)
{
	if (REC_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_RecResVolume;
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		*data = (uint8_t*)&g_usbd_PlayResVolume;
	}
	else
	{
		return 1;
	}
	*data_len = 2;
	return 0;
}


/* uac_class_interface_in_request()
 *
 */
static void uac_class_interface_in_request( struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;
	uint8_t ret;
	uint8_t control_selector;
	uint8_t unit_id;
	uint8_t *data;
	uint16_t data_len;

	// Device to host
	unit_id = request[FEAURE_UNIT_ID];
	control_selector = request[INTERFACE_CONTROL_SELECTOR_POS];

	ret = 1; // by default - stall
	switch(request[B_REQUEST_POS])
	{
	case UAC_GET_CUR:
		switch(control_selector)
		{
		case MUTE_CONTROL:
			ret = get_mute(runtime_hndl, unit_id, &data, &data_len);
			break;
		case VOLUME_CONTROL:
			ret = get_volume(runtime_hndl, unit_id,
					request[FEATURE_CHANNEL_SELECTOR_POS], &data, &data_len);
			break;
		default:
			break;
		}
		break;
	case UAC_GET_MIN:
		switch(control_selector)
		{
		case VOLUME_CONTROL:
			ret = get_min_volume(unit_id, &data, &data_len);
			break;
		default:
			break;
		}
		break;
	case UAC_GET_MAX:
		switch(control_selector)
		{
		case VOLUME_CONTROL:
			ret = get_max_volume(unit_id, &data, &data_len);
			break;
		default:
			break;
		}
		break;
	case UAC_GET_RES:
		switch(control_selector)
		{
		case VOLUME_CONTROL:
			ret = get_res_volume(unit_id, &data, &data_len);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (0 == ret)
	{
		memcpy(data_to_send, data, data_len);
		set_request_in_buffer.data = data_to_send;
		set_request_in_buffer.size = data_len;
		DEV_IOCTL_1_PARAMS(usb_hw,
				IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER, &set_request_in_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}




static uint8_t set_mute(struct usb_audio_class_runtime_t *runtime_hndl,
	struct set_request_out_buffer_t *set_request_out_buffer, uint8_t unit_id)
{
	uint8_t *data;

	if (1 == runtime_hndl->app_is_getting_data)
	{
		return 1;
	}

	if (REC_FEATURE_UNITID == unit_id)
	{
		data = (uint8_t*)&runtime_hndl->recording_mute;
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		data = (uint8_t*)&runtime_hndl->playback_mute;
	}
	else
	{
		return 1;
	}
	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 1;
	runtime_hndl->request_state = REQ_STATE_GETTING_MUTE;
	return 0;
}



static uint8_t set_volume(struct usb_audio_class_runtime_t *runtime_hndl,
				struct set_request_out_buffer_t *set_request_out_buffer,
				uint8_t unit_id, uint8_t channel_id)
{
	uint8_t *data;

	if (1 == runtime_hndl->app_is_getting_data)
	{
		return 1;
	}

	if (REC_FEATURE_UNITID == unit_id)
	{
		if (NUM_OF_PLAYBACK_CHANNELS <= channel_id)
		{
			return 1;
		}
		data = (uint8_t*)&runtime_hndl->curr_recording_volume[channel_id];
	}
	else if (PLAY_FEATURE_UNITID == unit_id)
	{
		if (NUM_OF_RECORDING_CHANNELS <= channel_id)
		{
			return 1;
		}
		data = (uint8_t*)&runtime_hndl->curr_playback_volume[channel_id];
	}
	else
	{
		return 1;
	}

	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 2;
	runtime_hndl->request_state = REQ_STATE_GETTING_VOLUME;
	return 0;
}


/*  uac_class_interface_out_request()
 *
 */
static void uac_class_interface_out_request(struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;
	uint8_t ret;
	uint8_t unit_id;

	unit_id = request[FEAURE_UNIT_ID];

	ret = 1; // by default - stall
	switch(request[B_REQUEST_POS])
	{
	case UAC_SET_CUR:
		switch(request[INTERFACE_CONTROL_SELECTOR_POS])
		{
		case MUTE_CONTROL:
			ret = set_mute(runtime_hndl, &set_request_out_buffer, unit_id);
			break;
		case VOLUME_CONTROL:
			ret = set_volume(runtime_hndl, &set_request_out_buffer,
					unit_id, request[FEATURE_CHANNEL_SELECTOR_POS]);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (0 == ret)
	{
		DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER, &set_request_out_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}


static void out_transfer_finished(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	struct dev_desc_t *control_callback_dev;

	control_callback_dev = cfg_hndl->control_callback_dev;
	if (NULL == control_callback_dev)
	{
		runtime_hndl->request_state = REQ_STATE_IDLE;
		return;
	}

	switch (runtime_hndl->request_state)
	{
	case REQ_STATE_GETTING_VOLUME:
		DEV_CALLBACK_0_PARAMS(
			control_callback_dev, USB_AUDIO_CLASS_CALLBACK_VOLUME_CHANGED);
		runtime_hndl->request_state = REQ_STATE_IDLE;
		break;
	case REQ_STATE_GETTING_MUTE:
		DEV_CALLBACK_0_PARAMS(
			control_callback_dev, USB_AUDIO_CLASS_CALLBACK_MUTE_CHANGED);
		runtime_hndl->request_state = REQ_STATE_IDLE;
		break;
	case REQ_STATE_IDLE:
	default:
		break;
	}
}


void uac_interface_class_request(
	struct dev_desc_t *callback_dev, uint8_t callback_type, uint8_t *request)
{
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, callback_dev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	switch (callback_type)
	{
	case INTERFACE_CALLBACK_TYPE_STANDARD_SET_INTERFACE:
		break;
	case INTERFACE_CALLBACK_TYPE_REQUEST:
		if(request[BM_REQ_TYPE_POS] & REQ_DIRECTION_MASK)
		{// from device to host
			uac_class_interface_in_request(usb_hw, runtime_hndl, request);
		}
		else
		{// from host to device
			uac_class_interface_out_request(usb_hw, runtime_hndl, request);
		}
		break;
	case INTERFACE_CALLBACK_TYPE_DATA_OUT_FINISHED:
		out_transfer_finished(cfg_hndl, runtime_hndl);
		break;
	default:
		return;
	}
}


/*  uac_class_endpoint_out_request()
 *
 */
static void uac_class_endpoint_out_request( struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;

	switch(request[B_REQUEST_POS])
	{
	case UAC_SET_CUR:
		if ((request[ENDPOINT_CONTROL_SELECTOR_POS] == SAMPLING_FREQ_CONTROL) &&
				(request[ENDPOINT_ID_POS] == runtime_hndl->out_endpoint_num))
		{
			set_request_out_buffer.data = (uint8_t*)&g_usbd_PlaySampleRate;
			set_request_out_buffer.size = 3;
			DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER,
				&set_request_out_buffer);
			return;
		}
		break;
	default:
		break;
	}
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
}



/*  uac_class_endpoint_in_request()
 *
 */
static void uac_class_endpoint_in_request( struct dev_desc_t *usb_hw,
		struct usb_audio_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;

	switch(request[B_REQUEST_POS])
	{
	case UAC_GET_CUR:
		if ((request[ENDPOINT_CONTROL_SELECTOR_POS] == SAMPLING_FREQ_CONTROL) &&
				(request[ENDPOINT_ID_POS] == runtime_hndl->out_endpoint_num))
		{
			set_request_in_buffer.data = (uint8_t*)&g_usbd_PlaySampleRate;
			set_request_in_buffer.size = 3;
			DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER,
				&set_request_in_buffer);
			return;
		}
		break;
	default:
		break;
	}
	DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
}



void uac_endpoint_class_request(
		struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_audio_class_runtime_t *runtime_hndl;
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_audio_class, callback_dev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_audio_class, callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if (request[BM_REQ_TYPE_POS] & REQ_DIRECTION_MASK) // Device to host
	{
		uac_class_endpoint_in_request(usb_hw, runtime_hndl, request);
	}
	else // Host to device
	{
		uac_class_endpoint_out_request(usb_hw, runtime_hndl, request);
	}

}
