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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_audio_class_api.h"
#include "usb_audio_class.h"

/*following line add module to available module list for dynamic device tree*/
#include "usb_audio_class_add_component.h"


#if !defined(BYTES_PER_PCM_CHANNEL)
	#error "BYTES_PER_PCM_CHANNEL should be defined in project header files"
#endif
#if !defined(NUM_OF_CHANNELS)
	#error "NUM_OF_CHANNELS should be defined in project header files"
#endif
#if !defined(SAMPLE_RATE)
	#error "SAMPLE_RATE  should be defined"
#endif


#define NORMAL_AUDIO_PACKET_SIZE  ((BYTES_PER_PCM_CHANNEL *       \
							SAMPLE_RATE * NUM_OF_CHANNELS) / 1000)// (per ms)

#define MAX_AUDIO_PACKET_SIZE (NORMAL_AUDIO_PACKET_SIZE \
							+ (NUM_OF_CHANNELS * BYTES_PER_PCM_CHANNEL))

typedef enum
{
	USB_AUDIO_CLASS_BUFF_IDLE,
	USB_AUDIO_CLASS_BUFF_IN_PROCCESS,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_FILLING,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_PROCESSING
} USB_AUDIO_CLASS_buff_state_t;

#define NUM_OF_TX_BUFFERS   5

/***************************************************/
/*!<Define Audio Class Specific Request */
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




extern void add_audio_class_device(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint16_t audio_pckt_size);


/* Record MUTE control. 0 = normal. 1 = MUTE */
static volatile uint8_t g_usbd_RecMute       = 0x01;

/* Play MUTE control. 0 = normal. 1 = MUTE */
static volatile uint8_t g_usbd_PlayMute      = 0x01;

/* Record left channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_RecVolumeL    = 0x1000;

/* Record right channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_RecVolumeR    = 0x1000;
static volatile int16_t g_usbd_RecMaxVolume  = 0x7FFF;
static volatile int16_t g_usbd_RecMinVolume  = 0x8000;
static volatile int16_t g_usbd_RecResVolume  = 0x400;

/* Play left channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_PlayVolumeL   = 0x1000;

/* Play right channel volume. Range is -32768 ~ 32767 */
static volatile int16_t g_usbd_PlayVolumeR   = 0x1000;
static volatile int16_t g_usbd_PlayMaxVolume = 0x7FFF;
static volatile int16_t g_usbd_PlayMinVolume = 0x8000;
static volatile int16_t g_usbd_PlayResVolume = 0x0400;

static uint8_t g_usbd_RecStarted = 0;

//#define DEBUG

static int dbg_force_over_or_underflow = 0;
#define SMOOTH_VALUE    10
static uint8_t skip_repeat_smooth = 0;


void new_usb_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	uint8_t	curr_buff_indx;
	uint8_t **buffers;
	uint32_t buff_size;
	uint32_t curr_pos_in_rx_buffer;
	uint32_t bytes_to_copy;
	uint8_t *curr_buff;
	int sample_to_skip_or_repeat;
	uint8_t num_of_bytes_per_sample;
	uint16_t num_of_bytes_per_sample_all_channels;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);
	num_of_bytes_per_sample = cfg_hndl->num_of_bytes_per_sample;
	sample_to_skip_or_repeat = runtime_hndl->sample_to_skip_or_repeat;
	buff_size = cfg_hndl->buff_size;

	// 2 for num of channels
	num_of_bytes_per_sample_all_channels = 2 * num_of_bytes_per_sample;

	if (buff_size < size)
	{
		CRITICAL_ERROR("support only buff_size > size \n");
	}
	if (1 == ( size & 0x01 ))
	{
		CRITICAL_ERROR("odd numbers of bytes \n");
	}

	curr_pos_in_rx_buffer = runtime_hndl->curr_pos_in_rx_buffer;
	curr_buff_indx = runtime_hndl->curr_buff_indx;
	buffers = runtime_hndl->rx_buff;
	curr_buff = buffers[curr_buff_indx];

#ifdef DEBUG
	if (dbg_force_over_or_underflow > 0)
	{
		size = size - num_of_bytes_per_sample_all_channels;
		dbg_force_over_or_underflow--;
	}
#endif

	if (( 0 < sample_to_skip_or_repeat ) &&
			(SMOOTH_VALUE == skip_repeat_smooth++) &&
			(0 == dbg_force_over_or_underflow))
	{
		skip_repeat_smooth = 0;
		size = size - num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat--;
	}
	bytes_to_copy = buff_size - curr_pos_in_rx_buffer;
	if (size < bytes_to_copy)
	{
		bytes_to_copy = size;
	}

	memcpy(&curr_buff[curr_pos_in_rx_buffer], buff, bytes_to_copy);
	curr_pos_in_rx_buffer = curr_pos_in_rx_buffer + bytes_to_copy;
	if ((( 0 > sample_to_skip_or_repeat) &&
			(SMOOTH_VALUE == skip_repeat_smooth++) &&
			(buff_size >=
				(curr_pos_in_rx_buffer + num_of_bytes_per_sample_all_channels)))
			&& (0 == dbg_force_over_or_underflow))
	{
		skip_repeat_smooth = 0;
		memcpy(&curr_buff[curr_pos_in_rx_buffer],
				&curr_buff[curr_pos_in_rx_buffer -
				           num_of_bytes_per_sample_all_channels],
				num_of_bytes_per_sample_all_channels);
		curr_pos_in_rx_buffer += num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat++;
	}
#ifdef DEBUG
	if ( (dbg_force_over_or_underflow < 0) && (buff_size >=
			(curr_pos_in_rx_buffer + num_of_bytes_per_sample_all_channels)))
	{
		memcpy(&curr_buff[curr_pos_in_rx_buffer],
				&curr_buff[curr_pos_in_rx_buffer -
				           num_of_bytes_per_sample_all_channels],
				num_of_bytes_per_sample_all_channels);
		curr_pos_in_rx_buffer += num_of_bytes_per_sample_all_channels;
		dbg_force_over_or_underflow++;
	}
#endif

	if (curr_pos_in_rx_buffer == buff_size)
	{
		uint8_t	next_buff_indx;
		uint8_t *next_buff_status;
		uint8_t *curr_buff_status;
		struct dev_desc_t *callback_dev;

		curr_pos_in_rx_buffer = 0;
		next_buff_indx = (curr_buff_indx + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
		curr_buff_status = &runtime_hndl->rx_buff_status[curr_buff_indx];
		next_buff_status = &runtime_hndl->rx_buff_status[next_buff_indx];

		*curr_buff_status =	USB_AUDIO_CLASS_BUFF_RX_RADA_READY;
		if (USB_AUDIO_CLASS_BUFF_IDLE != *next_buff_status)
		{
			//remove for debugging purposes:
			CRITICAL_ERROR("next rx buffer not ready\n");
		}
		*next_buff_status = USB_AUDIO_CLASS_BUFF_IN_PROCCESS;
		runtime_hndl->curr_buff_indx = next_buff_indx;

		callback_dev = cfg_hndl->callback_dev;
		if  (NULL != callback_dev)
		{
			DEV_CALLBACK_1_PARAMS(
					callback_dev, CALLBACK_NEW_DATA_ARRIVED, adev);
		}

		size = size - bytes_to_copy;
		if (0 != size)
		{
			buff = buff + bytes_to_copy;
			curr_buff = buffers[next_buff_indx];
			memcpy(curr_buff, buff, size);
			curr_pos_in_rx_buffer = size;
		}
	}
	runtime_hndl->curr_pos_in_rx_buffer = curr_pos_in_rx_buffer;
	runtime_hndl->sample_to_skip_or_repeat = sample_to_skip_or_repeat;

}


uint32_t volatile dbg_stall_count = 0;
size_t  volatile dbg_available_data_size = 0;
size_t  volatile dbg_buff_size = 0;
size_t  volatile dbg_copy_size = 0;
size_t  volatile dbg_cnt2 = 0;
size_t  volatile dbg_cnt3 = 0;

void new_usb_audio_requested(struct dev_desc_t *adev)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;
	size_t  curr_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
	size_t  available_data_size;
	size_t   tx_buff_size;
	size_t   buff_size;
	struct dev_desc_t *usb_hw;
	uint8_t  *tx_buff;
	uint8_t  *tx_pckt_buff;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	usb_hw = cfg_hndl->usb_hw;
	buff_size = cfg_hndl->buff_size;
	tx_buff_size = buff_size * NUM_OF_TX_BUFFERS;
	curr_write_pos_in_tx_buffer = runtime_hndl->curr_write_pos_in_tx_buffer;
	curr_read_pos_in_tx_buffer = runtime_hndl->curr_read_pos_in_tx_buffer;

	if (curr_write_pos_in_tx_buffer >= curr_read_pos_in_tx_buffer)
	{
		available_data_size =
				curr_write_pos_in_tx_buffer - curr_read_pos_in_tx_buffer;
	}
	else
	{
		available_data_size = tx_buff_size -
				(curr_read_pos_in_tx_buffer - curr_write_pos_in_tx_buffer);
	}

	dbg_available_data_size = available_data_size;
	dbg_buff_size = buff_size;
	if (available_data_size > buff_size)
	{
		struct set_data_to_in_endpoint_t  data_to_endpoint;
		size_t  data_to_copy;

		data_to_copy = NORMAL_AUDIO_PACKET_SIZE;
		if (available_data_size < (2 * buff_size)) // reduce speed(clock)
		{
			data_to_copy -= (NUM_OF_CHANNELS * BYTES_PER_PCM_CHANNEL);
			dbg_cnt2++;
		}
		if (available_data_size > (3 * buff_size)) // increase speed(clock)
		{
			data_to_copy += (NUM_OF_CHANNELS * BYTES_PER_PCM_CHANNEL);
			dbg_cnt3++;
		}
		dbg_copy_size = data_to_copy;
		tx_buff = runtime_hndl->tx_buff;
		tx_pckt_buff = runtime_hndl->tx_pckt_buff;
		data_to_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
		data_to_endpoint.data = tx_pckt_buff;
		data_to_endpoint.size = data_to_copy;
		while (data_to_copy--)
		{
			*tx_pckt_buff++ = tx_buff[curr_read_pos_in_tx_buffer++];

			if (curr_read_pos_in_tx_buffer == tx_buff_size)
			{
				curr_read_pos_in_tx_buffer = 0;
			}
		}
		DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT,
				&data_to_endpoint);
		runtime_hndl->curr_read_pos_in_tx_buffer = curr_read_pos_in_tx_buffer;
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
		dbg_stall_count++;
	}

}


static uint8_t get_full_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t *buffer_state;

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_buff_indx will not be changed
	 */
	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer ;
	buffer_state = &runtime_hndl->rx_buff_status[next_supplied_rx_buffer];
	if (USB_AUDIO_CLASS_BUFF_RX_RADA_READY == *buffer_state)
	{
		*buff = runtime_hndl->rx_buff[next_supplied_rx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state =	USB_AUDIO_CLASS_BUFF_RX_RADA_PROCESSING;
	}
	else
	{
		*buff = NULL;
		*buff_size = 0;
	}
	return 0;
}


static uint8_t release_rx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t next_supplied_rx_buffer;

	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer;
	runtime_hndl->rx_buff_status[next_supplied_rx_buffer] =
									USB_AUDIO_CLASS_BUFF_IDLE;
	next_supplied_rx_buffer =
			(next_supplied_rx_buffer + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
}


volatile uint32_t dbg_cnt1 =0;
static uint8_t get_empty_tx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *ret_buff_size)
{
	size_t  curr_write_pos_in_tx_buffer;
	size_t  next_write_pos_in_tx_buffer;
	size_t  curr_read_pos_in_tx_buffer;
	size_t   buff_size;
	size_t   tx_buff_size;
	uint8_t  there_is_space_for_write;

	curr_write_pos_in_tx_buffer = runtime_hndl->curr_write_pos_in_tx_buffer;
	curr_read_pos_in_tx_buffer = runtime_hndl->curr_read_pos_in_tx_buffer;
	buff_size = cfg_hndl->buff_size;
	tx_buff_size = buff_size * NUM_OF_TX_BUFFERS;

	next_write_pos_in_tx_buffer = curr_write_pos_in_tx_buffer + buff_size;
	if (next_write_pos_in_tx_buffer == tx_buff_size)
	{
		next_write_pos_in_tx_buffer = 0;
	}

	there_is_space_for_write = 0;
	if (curr_write_pos_in_tx_buffer >= curr_read_pos_in_tx_buffer)
	{
		// first check if there will be NO wrap in write buffer pointer
		if ((next_write_pos_in_tx_buffer >= curr_write_pos_in_tx_buffer) ||
			 (next_write_pos_in_tx_buffer < curr_read_pos_in_tx_buffer))
		{
			there_is_space_for_write = 1;
		}
	}
	else
	{
		// first check if there will BE wrap of write buffer pointer
		if ((next_write_pos_in_tx_buffer >= curr_write_pos_in_tx_buffer) &&
			 (next_write_pos_in_tx_buffer < curr_read_pos_in_tx_buffer))
		{
			there_is_space_for_write = 1;
		}

	}
	/* first check if no wrapping occurred*/
	if (there_is_space_for_write)
	{
		*buff = &runtime_hndl->tx_buff[curr_write_pos_in_tx_buffer];
		*ret_buff_size = buff_size;
		runtime_hndl->curr_write_pos_in_tx_buffer = next_write_pos_in_tx_buffer;
	}
	else
	{
			dbg_cnt1++;
		*buff = NULL;
		*ret_buff_size = 0;
		return 1;
	}
	return 0;
}


static uint8_t release_tx_buffer(struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	if (0 == g_usbd_RecStarted)
	{
		struct set_data_to_in_endpoint_t  data_to_endpoint;

		data_to_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
		data_to_endpoint.data = NULL;
		data_to_endpoint.size = 0;

		DEV_IOCTL_1_PARAMS(cfg_hndl->usb_hw,
				IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &data_to_endpoint);
		g_usbd_RecStarted = 1;
	}
	return 0;
}


static uint8_t get_mute(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMute;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMute;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 1;
	return 0;
}


static uint8_t get_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		/* Left or right channel */
		if(request[2] == 1)
		{
			data = (uint8_t*)&g_usbd_RecVolumeL;
		}
		else
		{
			data = (uint8_t*)&g_usbd_RecVolumeR;
		}
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		/* Left or right channel */
		if(request[2] == 1)
		{
			data = (uint8_t*)&g_usbd_PlayVolumeL;
		}
		else
		{
			data = (uint8_t*)&g_usbd_PlayVolumeR;
		}
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_min_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMinVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMinVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_max_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMaxVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMaxVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


static uint8_t get_res_volume(
	struct set_request_in_buffer_t *set_request_in_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecResVolume;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayResVolume;
	}
	else
	{
		return 1;
	}
	set_request_in_buffer->data = data;
	set_request_in_buffer->size = 2;
	return 0;
}


/* uac_class_in_request()
 *
 */
static void uac_class_in_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;
	uint8_t ret;

	// Device to host
	switch(request[1])
	{
	case UAC_GET_CUR:
		switch(request[3])
		{
		case MUTE_CONTROL:
			ret = get_mute(&set_request_in_buffer, request);
			break;
		case VOLUME_CONTROL:
			ret = get_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_MIN:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_min_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_MAX:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_max_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	case UAC_GET_RES:
		switch(request[3])
		{
		case VOLUME_CONTROL:
			ret = get_res_volume(&set_request_in_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	default:
		ret = 1;
		break;
	}

	if (0 == ret)
	{
		DEV_IOCTL_1_PARAMS(usb_hw,
				IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER, &set_request_in_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}




static uint8_t set_mute(
	struct set_request_out_buffer_t *set_request_out_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_RecMute;
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		data = (uint8_t*)&g_usbd_PlayMute;
	}
	else
	{
		return 1;
	}
	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 1;
	return 0;
}


static uint8_t set_volume(
	struct set_request_out_buffer_t *set_request_out_buffer, uint8_t *request)
{
	uint8_t *data;

	if (REC_FEATURE_UNITID == request[5])
	{
		if (request[2] == 1)
		{
			data = (uint8_t *)&g_usbd_RecVolumeL;
		}
		else
		{
			data = (uint8_t *)&g_usbd_RecVolumeR;
		}
	}
	else if (PLAY_FEATURE_UNITID == request[5])
	{
		if (request[2] == 1)
		{
			data = (uint8_t *)&g_usbd_PlayVolumeL;
		}
		else
		{
			data = (uint8_t *)&g_usbd_PlayVolumeR;
		}
	}
	else
	{
		return 1;
	}

	set_request_out_buffer->data = data;
	set_request_out_buffer->size = 2;
	return 0;
}


/*  uac_class_out_request()
 *
 */
static void uac_class_out_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;
	uint8_t ret;

    switch(request[1])
	{
	case UAC_SET_CUR:
		switch(request[3])
		{
		case MUTE_CONTROL:
			ret = set_mute(&set_request_out_buffer, request);
			break;
		case VOLUME_CONTROL:
			ret = set_volume(&set_request_out_buffer, request);
			break;
		default:
			ret = 1;
			break;
		}
		break;
	default:
		ret = 1;
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


void uac_class_request(struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80)    /* request data transfer direction */
	{// from device to host
		uac_class_in_request(usb_hw, request);
	}
	else
	{// from host to device
		uac_class_out_request(usb_hw, request);
	}
}


static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint8_t *new_buff;
	uint32_t buff_size;
	uint32_t i;
	uint16_t cnt = 0;



	add_audio_class_device(adev, cfg_hndl, runtime_hndl, MAX_AUDIO_PACKET_SIZE);

	buff_size = cfg_hndl->buff_size;
	for (i = 0; i < USB_AUDIO_CLASS_NUM_OF_BUFFERS; i++)
	{
		new_buff = (uint8_t*)malloc(buff_size);
		if (NULL == new_buff)
		{
			CRITICAL_ERROR("no memory");
		}
		runtime_hndl->rx_buff[i] = new_buff;
		runtime_hndl->rx_buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
	}

	new_buff = (uint8_t*)malloc(buff_size * NUM_OF_TX_BUFFERS);
	if (NULL == new_buff)
	{
		CRITICAL_ERROR("no memory");
	}
	for (i = 0; i < (buff_size * NUM_OF_TX_BUFFERS); i = i + 4)
	{
		*((uint16_t*)&new_buff[i]) =  cnt;
		*((uint16_t*)&new_buff[i + 2]) =  cnt++;
	}
	runtime_hndl->tx_buff = new_buff;
	new_buff = (uint8_t*)malloc(MAX_AUDIO_PACKET_SIZE);
	if (NULL == new_buff)
	{
		CRITICAL_ERROR("no memory");
	}
	runtime_hndl->tx_pckt_buff = new_buff;

	runtime_hndl->curr_write_pos_in_tx_buffer = 0;
	runtime_hndl->curr_write_pos_in_tx_buffer = 0;
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_audio_class_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		start_audio_class(adev, cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_GET_RX_BUFF :
		get_full_rx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;

	case USB_AUDIO_CLASS_IOCTL_RELEASE_RX_BUFF :
		release_rx_buffer(cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_GET_EMPTY_TX_BUFF :
		return get_empty_tx_buffer(cfg_hndl,
				runtime_hndl,  aIoctl_param1, aIoctl_param2);
		break;

	case USB_AUDIO_CLASS_IOCTL_RELEASE_TX_BUFF :
		release_tx_buffer(cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_SLOWDOWN_BY_SKIPING_SAMPLES :
		if (0 >= runtime_hndl->sample_to_skip_or_repeat)
		{
			runtime_hndl->sample_to_skip_or_repeat = *(uint32_t*)aIoctl_param1;
		}
		break;

	case USB_AUDIO_CLASS_IOCTL_ACCELERATE_BY_REPEATING_SAMPLES :
		if (0 <= runtime_hndl->sample_to_skip_or_repeat)
		{
			runtime_hndl->sample_to_skip_or_repeat =
										-(*(uint32_t*)aIoctl_param1);
		}
		break;

	case USB_AUDIO_CLASS_IOCTL_SOFT_RESET :
		runtime_hndl->sample_to_skip_or_repeat = 0;
		break;

	default :
		return 1;
	}
	return 0;
}
