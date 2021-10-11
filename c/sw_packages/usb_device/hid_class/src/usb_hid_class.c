/******************************************************************************
 * @file     usb_hid_class.c
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
#include "usb_hid_class_api.h"
#include "usb_hid_class.h"
#include "usb_device_descriptors_api.h"


#define TO_BE_SET_AUTOMATICALLY   0
#define AUTO_SET_PCKT_SIZE_MSB   0x00
#define AUTO_SET_PCKT_SIZE_LSB   0x00

#define B3VAL(x) (x&0xFF), ((x >>8) & 0xFF), ((x >>16) & 0xFF)

#define HID_SET_REPORT       0x09
#define HID_SET_IDLE         0x0A
#define HID_GET_REPORT       0x01
#define HID_SET_PROTOCOL     0x0B
#define INTERFACE_LEN 		 0x09

#define REQUEST_FIELD_POS  1
#define REPORT_ID_POS      2
#define REQUEST_LENGTH_LOW_POS  6
#define REQUEST_LENGTH_HIGH_POS 7
#define GET_DATA_LENGTH(request) (request[REQUEST_LENGTH_LOW_POS] +\
									(request[REQUEST_LENGTH_HIGH_POS] << 8))

static uint8_t const interface_association_descriptor[]=
{
	/*  Interface Association Descriptor:  */
	0x08,
	0x0B ,  //    bDescriptorType
	TO_BE_SET_AUTOMATICALLY, // bFirstInterface
	0x01    , // bInterfaceCount
	0x03    , // bFunctionClass
	0x00    , // bFunctionSubClass
	0x00    , // bFunctionProtocol
	0x02    ,//  iFunction
};


static const uint8_t hid_interface[] =
{
	/* interface descriptor for HID */
	INTERFACE_LEN,  /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
	TO_BE_SET_AUTOMATICALLY,   /* bInterfaceNumber */
	0x00,           /* bAlternateSetting */
	0x02,           /* bNumEndpoints */
	0x03,           /* bInterfaceClass */
	0x00,           /* bInterfaceSubClass */
	0x00,           /* bInterfaceProtocol */
	0x00,           /* iInterface */

	/* HID Descriptor */
	0x09,        /* Size of this descriptor in UINT8s. */
	USB_HID_DESCRIPTOR_TYPE,       /* HID descriptor type. */
	0x10, 0x01,     /* HID Class Spec. release number. */
	0x00,           /* H/W target country. */
	0x01,           /* Number of HID class descriptors to follow. */
	0x22,          /* Descriptor type: HID report */
	/* Total length of report descriptor. */
	TO_BE_SET_AUTOMATICALLY,
	TO_BE_SET_AUTOMATICALLY,

	/* EP Descriptor: interrupt in. */
	0x07,   /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,  /* bDescriptorType */
	(TO_BE_SET_AUTOMATICALLY | 0x08), /* bEndpointAddress : Input */
	0x03,         /* bmAttributes : interrupt*/
	/* wMaxPacketSize */
	AUTO_SET_PCKT_SIZE_LSB,             /* wMaxPacketSize: */
	AUTO_SET_PCKT_SIZE_MSB,
	10,        /* bInterval */

	/* EP Descriptor: interrupt out. */
	0x07,   /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,  /* bDescriptorType */
	TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress */
	0x03,         /* bmAttributes : interrupt */
	/* wMaxPacketSize */
	AUTO_SET_PCKT_SIZE_LSB,             /* wMaxPacketSize: */
	AUTO_SET_PCKT_SIZE_MSB,
	10     /* bInterval */
};


static void new_data_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct dev_desc_t * callback_rx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_hid_class, adev);

	callback_rx_dev = cfg_hndl->callback_rx_dev;
	if (NULL == callback_rx_dev)
	{
		return;
	}
	DEV_CALLBACK( callback_rx_dev,
			CALLBACK_DATA_RECEIVED, (void*)buff, (void*)(size_t)size);
}



static void end_of_transmit_callback(struct dev_desc_t *adev)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct usb_hid_class_runtime_t  *runtime_hndl;
	struct dev_desc_t * callback_tx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_hid_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, adev);


	callback_tx_dev = cfg_hndl->callback_tx_dev;
	if (NULL != callback_tx_dev)
	{
		DEV_CALLBACK(callback_tx_dev,
							CALLBACK_TX_DONE, (void*)(runtime_hndl->sentLen));
	}

}


/**
 * usb_hid_pwrite()
 *
 * return:
 */
static size_t usb_hid_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct dev_desc_t *usb_hw;
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct set_data_to_in_endpoint_t set_data_to_in_endpoint;
	struct usb_hid_class_runtime_t  *runtime_hndl;
	size_t sentLen;
	uint16_t max_host_out_data_packet_size;


	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_hid_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, adev);

	usb_hw = cfg_hndl->usb_hw;
	max_host_out_data_packet_size = cfg_hndl->max_host_out_data_packet_size;

	if (aLength > max_host_out_data_packet_size)
	{
		sentLen = max_host_out_data_packet_size;
	}
	else
	{
		sentLen = aLength;
	}

	runtime_hndl->sentLen = sentLen;

	set_data_to_in_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
	set_data_to_in_endpoint.data = apData;
	set_data_to_in_endpoint.size = sentLen;
	DEV_IOCTL(usb_hw,
		IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &set_data_to_in_endpoint);

	return sentLen;
}


/* hid_class_in_request()
 *
 */
static void hid_class_in_request(struct dev_desc_t *usb_hw,
		struct usb_hid_class_runtime_t  *runtime_hndl, uint8_t *request)
{
	hid_in_report_over_control_pipe_callback_t callback_func;
	struct set_request_in_buffer_t set_request_in_buffer;
	uint16_t  data_size;
	uint16_t  requested_data_size;
	uint8_t ret;

	// Device to host
	ret = 0;
	switch(request[REQUEST_FIELD_POS])
	{
	case HID_GET_REPORT:
		callback_func =  runtime_hndl->hid_in_report_over_control_pipe_callback;
		if (NULL != callback_func)
		{
			callback_func(request[REPORT_ID_POS],
						&set_request_in_buffer.data, &data_size);
		}
		requested_data_size = GET_DATA_LENGTH(request);
		if (data_size > requested_data_size)
		{
			data_size = requested_data_size;
		}
		set_request_in_buffer.size = data_size;
		break;
	default:
		ret = 1;
		break;
	}

	if (0 == ret)
	{
		DEV_IOCTL(usb_hw,
				IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER, &set_request_in_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_STALL_CONTROL_ENDPOINT);
	}
}


/*  hid_class_out_request()
 *
 */
static void hid_class_out_request( struct dev_desc_t *usb_hw,
		struct usb_hid_class_runtime_t *runtime_hndl, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;
	uint8_t ret;
	uint16_t  data_size;

	ret = 0;
    switch(request[REQUEST_FIELD_POS])
	{
    case HID_SET_REPORT:
		set_request_out_buffer.data = runtime_hndl->report_out_buf;
		data_size =  GET_DATA_LENGTH(request);
		if (data_size > runtime_hndl->report_out_max_buf_size)
		{
			data_size = runtime_hndl->report_out_max_buf_size;
		}
		runtime_hndl->report_out_received_buf_size = data_size;
		set_request_out_buffer.size = data_size;
		break;
	case HID_SET_IDLE:
		set_request_out_buffer.data = NULL;
		set_request_out_buffer.size = 0;
		break;
	case HID_SET_PROTOCOL:
	default:
		ret = 1;
		break;
	}

    if (0 == ret)
    {
    	DEV_IOCTL(usb_hw,
    		IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER, &set_request_out_buffer);
    }
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_STALL_CONTROL_ENDPOINT);
	}
}


static void hid_class_request(struct dev_desc_t *callback_dev,
							uint8_t callback_type, uint8_t *request)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;
	struct usb_hid_class_runtime_t *runtime_hndl;
	hid_out_report_over_control_pipe_callback_t callback_func;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_hid_class, callback_dev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, callback_dev);
	usb_hw = cfg_hndl->usb_hw;
	switch (callback_type)
	{
	case INTERFACE_CALLBACK_TYPE_DATA_OUT_FINISHED:
		callback_func = runtime_hndl->hid_out_report_over_control_pipe_callback;
		if (NULL != callback_func)
		{
			callback_func(runtime_hndl->report_out_received_buf_size);
		}
		break;
	case INTERFACE_CALLBACK_TYPE_REQUEST:
		if(request[0] & 0x80)    /* request data transfer direction */
		{// from device to host
			hid_class_in_request(usb_hw, runtime_hndl, request);
		}
		else
		{// from host to device
			hid_class_out_request(usb_hw, runtime_hndl, request);
		}
		break;
	case INTERFACE_CALLBACK_TYPE_STANDARD_SET_INTERFACE:
	default:
		return;
	}
}


static void configure_endpoints(struct dev_desc_t *adev,
		struct usb_hid_class_cfg_t *cfg_hndl, uint8_t *interface)
{
	struct usb_hid_class_runtime_t *runtime_hndl;
	struct dev_desc_t *usb_hw;
	struct set_endpoints_t set_endpoints;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[2];
	uint8_t   endpoints_num_arr[2];
	usb_dev_out_endpoint_callback_func_t   out_func_arr[2];
	usb_dev_endpoint_request_callback_func_t  endpoint_request_callback_func[2];
	uint8_t    endpoints_type_arr[2];
	uint16_t   max_pckt_sizes[2];
	uint8_t in_endpoint_num;
	uint8_t out_endpoint_num;
	uint16_t max_host_in_data_packet_size;
	uint16_t max_host_out_data_packet_size;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, adev);
	usb_hw = cfg_hndl->usb_hw;
	max_host_in_data_packet_size = cfg_hndl->max_host_in_data_packet_size;
	max_host_out_data_packet_size = cfg_hndl->max_host_out_data_packet_size;


	set_endpoints.num_of_endpoints = 2;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	endpoint_request_callback_func[0] = NULL;
	endpoint_request_callback_func[1] = NULL;
	out_func_arr[0] = NULL;
	out_func_arr[1] = new_data_received;
	in_func_arr[0] = end_of_transmit_callback;
	in_func_arr[1] = NULL;
	set_endpoints.out_func_arr = out_func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.endpoint_request_callback_func =
							endpoint_request_callback_func;
	set_endpoints.callback_dev = adev;
	max_pckt_sizes[0] = max_host_in_data_packet_size;
	max_pckt_sizes[1] = max_host_out_data_packet_size;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	endpoints_type_arr[0] = USB_DEVICE_API_EP_TYPE_INTERRUPT_IN;
	endpoints_type_arr[1] = USB_DEVICE_API_EP_TYPE_INTERRUPT_OUT;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);

	in_endpoint_num = endpoints_num_arr[0];
	out_endpoint_num = endpoints_num_arr[1];
	interface[18 + 2] = 0x80 | in_endpoint_num;// 0x80 for IN endpoint
	interface[18 + 4] = max_host_in_data_packet_size & 0xFF;
	interface[18 + 5] = (max_host_in_data_packet_size >> 8) & 0xFF;

	interface[25 + 2] = out_endpoint_num;
	interface[25 + 4] = max_host_out_data_packet_size & 0xFF;
	interface[25 + 5] = (max_host_out_data_packet_size >> 8) & 0xFF;
	runtime_hndl->in_endpoint_num = endpoints_num_arr[0];

}


static void update_configuration_desc(struct dev_desc_t *adev,
		struct usb_hid_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces)
{
	struct dev_desc_t *usb_descriptors_dev;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;
	uint8_t *iad;
	uint8_t *interface;
	uint8_t *hid_descriptor;
	struct usb_descriptors_hid_descriptor_t  *p_hid_descriptor;
	struct usb_hid_class_runtime_t *runtime_hndl;
	uint16_t report_desc_size;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, adev);
	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	iad = (uint8_t*)os_safe_malloc(sizeof(interface_association_descriptor)) ;
	errors_api_check_if_malloc_succeed(iad);
	memcpy(iad, interface_association_descriptor,
						sizeof(interface_association_descriptor));
	iad[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	DEV_IOCTL(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR, iad);
	os_safe_free(iad);


	interface = (uint8_t*)os_safe_malloc(sizeof(hid_interface)) ;
	errors_api_check_if_malloc_succeed(interface);
	memcpy(interface, hid_interface, sizeof(hid_interface));
	interface[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	hid_descriptor = &interface[INTERFACE_LEN];
	report_desc_size = runtime_hndl->report_desc_size;
	hid_descriptor[7] = report_desc_size & 0xFF;
	hid_descriptor[8] = (report_desc_size >> 8) & 0xFF;
	configure_endpoints(adev, cfg_hndl, interface);

	usb_desc_add_interface.interface_desc = interface;
	usb_desc_add_interface.interface_desc_size = sizeof(hid_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	usb_desc_add_interface.alt_interface_desc_size = 0;
	usb_desc_add_interface.is_hid_interface = 1;
	p_hid_descriptor = &usb_desc_add_interface.hid_descriptor;
	p_hid_descriptor->hid_desc_position_in_config_desc = INTERFACE_LEN;
	p_hid_descriptor->report_descriptor = runtime_hndl->report_desc;
	p_hid_descriptor->size = report_desc_size;
	DEV_IOCTL(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
	os_safe_free(interface);
}


static void start_hid_class(struct dev_desc_t *adev,
		struct usb_hid_class_cfg_t *cfg_hndl,
		struct usb_hid_class_runtime_t *runtime_hndl)
{
	struct register_interfaces_t register_interfaces;
	struct register_interface_t register_interface[1];
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;

	if (1 == runtime_hndl->device_started) return;
	if (0 == runtime_hndl->hid_report_was_set)
	{
		CRITICAL_ERROR("HID report should be set before startin HID class");
	}

	usb_descriptors_alloc_interfaces.num_of_interfaces = 1;
	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	DEV_IOCTL(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);

	update_configuration_desc(
			adev, cfg_hndl, &usb_descriptors_alloc_interfaces);

	register_interface[0].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	register_interface[0].interface_func = hid_class_request;

	register_interfaces.num_of_interfaces = 1;
	register_interfaces.register_interface_arr = register_interface;
	register_interfaces.callback_dev = adev;
	usb_hw = cfg_hndl->usb_hw;
	DEV_IOCTL(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);
	runtime_hndl->device_started = 1;
}


static uint8_t set_hid_report(struct usb_hid_class_runtime_t *runtime_hndl,
		struct usb_hid_set_report_descriptor_t *set_report_desc)
{
	runtime_hndl->report_desc = set_report_desc->report_desc;
	runtime_hndl->report_desc_size = set_report_desc->report_desc_size;
	runtime_hndl->report_out_buf = set_report_desc->report_out_buf;
	runtime_hndl->report_out_max_buf_size =
			set_report_desc->report_out_buf_size;
	runtime_hndl->hid_out_report_over_control_pipe_callback =
				set_report_desc->hid_out_report_over_control_pipe_callback;
	runtime_hndl->hid_in_report_over_control_pipe_callback =
				set_report_desc->hid_in_report_over_control_pipe_callback;
	runtime_hndl->hid_report_was_set = 1;
	return 0;
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t usb_hid_class_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct usb_hid_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_hid_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_hid_class, adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		start_hid_class(adev, cfg_hndl, runtime_hndl);
		break;
	case IOCTL_USB_HID_GET_IN_ENDPOINT_SIZE:
		if (0 == runtime_hndl->device_started)
		{
			CRITICAL_ERROR("device not started yet");
		}
		*(uint16_t *)aIoctl_param1 = cfg_hndl->max_host_in_data_packet_size;
		break;
	case IOCTL_USB_HID_SET_REPORT_DESCRIPTOR:
		return set_hid_report(runtime_hndl, aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  usb_hid_class
#define MODULE_IOCTL_FUNCTION        usb_hid_class_ioctl
#define MODULE_PWRITE_FUNCTION       usb_hid_pwrite
#include "add_module.h"
