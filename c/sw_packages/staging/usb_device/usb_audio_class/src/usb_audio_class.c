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

typedef enum
{
	USB_AUDIO_CLASS_BUFF_IDLE,
	USB_AUDIO_CLASS_BUFF_IN_PROCCESS,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_FILLING,
	USB_AUDIO_CLASS_BUFF_TX_DATA_IS_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_READY,
	USB_AUDIO_CLASS_BUFF_RX_RADA_PROCESSING
} USB_AUDIO_CLASS_buff_state_t;

static void new_audio_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_audio_class_cfg_t *cfg_hndl;
	struct usb_audio_class_runtime_t *runtime_hndl;

	uint8_t	curr_buff_indx;
	uint8_t	next_buff_indx;
	uint8_t *next_buff_status;
	uint8_t *curr_buff_status;
	uint8_t **buffers;
	struct dev_desc_t *callback_dev;
	uint32_t buff_size;
	uint32_t curr_pos_in_buffer;
	uint32_t bytes_to_copy;
	uint8_t *curr_buff;
	int sample_to_skip_or_repeat;
	uint8_t num_of_bytes_per_sample;
	uint16_t num_of_bytes_per_sample_all_channels;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);;
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

	curr_pos_in_buffer = runtime_hndl->curr_pos_in_buffer;
	curr_buff_indx = runtime_hndl->curr_buff_indx;
	buffers = runtime_hndl->buff;
	curr_buff = buffers[curr_buff_indx];

	if ( 0 < sample_to_skip_or_repeat )
	{
		size = size - num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat--;
	}
	bytes_to_copy = buff_size - curr_pos_in_buffer;
	if (size < bytes_to_copy)
	{
		bytes_to_copy = size;
	}

	memcpy(&curr_buff[curr_pos_in_buffer], buff, bytes_to_copy);
	curr_pos_in_buffer = curr_pos_in_buffer + bytes_to_copy;
	if (( 0 > sample_to_skip_or_repeat) && (buff_size >=
				(curr_pos_in_buffer + num_of_bytes_per_sample_all_channels)))
	{
		memcpy(&curr_buff[
				curr_pos_in_buffer - num_of_bytes_per_sample_all_channels],
				curr_buff, num_of_bytes_per_sample_all_channels);
		curr_pos_in_buffer =
				curr_pos_in_buffer + num_of_bytes_per_sample_all_channels;
		sample_to_skip_or_repeat++;
	}

	if (curr_pos_in_buffer == buff_size)
	{
		curr_pos_in_buffer = 0;
		next_buff_indx = (curr_buff_indx + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
		curr_buff_status = &runtime_hndl->buff_status[curr_buff_indx];
		next_buff_status = &runtime_hndl->buff_status[next_buff_indx];

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
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_NEW_DATA_ARRIVED);
		}

		size = size - bytes_to_copy;
		if (0 != size)
		{
			buff = buff + bytes_to_copy;
			curr_buff = buffers[next_buff_indx];
			memcpy(curr_buff, buff, size);
			curr_pos_in_buffer = size;
		}
	}
	runtime_hndl->curr_pos_in_buffer = curr_pos_in_buffer;
	sample_to_skip_or_repeat = runtime_hndl->sample_to_skip_or_repeat;

}


static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	uint32_t buff_size;
	uint8_t i;

	buff_size = cfg_hndl->buff_size;

	for (i = 0; i < USB_AUDIO_CLASS_NUM_OF_BUFFERS; i++)
	{
		runtime_hndl->buff[i] = (uint8_t*)malloc(buff_size);
		runtime_hndl->buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
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
	buffer_state = &runtime_hndl->buff_status[next_supplied_rx_buffer];
	if (USB_AUDIO_CLASS_BUFF_RX_RADA_READY == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_rx_buffer];
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
	runtime_hndl->buff_status[next_supplied_rx_buffer] =
									USB_AUDIO_CLASS_BUFF_IDLE;
	next_supplied_rx_buffer =
			(next_supplied_rx_buffer + 1) % USB_AUDIO_CLASS_NUM_OF_BUFFERS;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
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
	struct dev_desc_t *usb_hw;
	struct set_out_endpoint_callback_t set_out_endpoint_callback;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);
	usb_hw = cfg_hndl->usb_hw;

	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		set_out_endpoint_callback.callback_dev = adev;
		set_out_endpoint_callback.func = new_audio_received;
		set_out_endpoint_callback.endpoint_num = cfg_hndl->endpoint_num;
		DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_OUT_ENDPOINT_CALLBACK,
				&set_out_endpoint_callback);
		start_audio_class(adev, cfg_hndl, runtime_hndl);

		break;

	case USB_AUDIO_CLASS_IOCTL_GET_RX_BUFF :
		get_full_rx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;

	case USB_AUDIO_CLASS_IOCTL_RELEASE_RX_BUFF :
		release_rx_buffer(cfg_hndl, runtime_hndl);
		break;

	case USB_AUDIO_CLASS_IOCTL_SLOWDOWN_BY_SKIPING_SAMPLES :
		runtime_hndl->sample_to_skip_or_repeat = *(uint32_t*)aIoctl_param1;
		break;

	case USB_AUDIO_CLASS_IOCTL_ACCELERATE_BY_REPEATING_SAMPLES :
		runtime_hndl->sample_to_skip_or_repeat = -(*(uint32_t*)aIoctl_param1);
		break;

	case USB_AUDIO_CLASS_IOCTL_SOFT_RESET :
		runtime_hndl->sample_to_skip_or_repeat = 0;
		break;

	default :
		return 1;
	}
	return 0;
}
