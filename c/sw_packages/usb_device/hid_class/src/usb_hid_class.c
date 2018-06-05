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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dev_management_api.h"

#include "usb_device_api.h"
#include "usb_hid_class_api.h"
#include "usb_hid_class.h"
#include "usb_device_descriptors_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "usb_hid_class_add_component.h"


#define TO_BE_SET_AUTOMATICALLY   0

#define B3VAL(x) (x&0xFF), ((x >>8) & 0xFF), ((x >>16) & 0xFF)

#define HID_SET_REPORT              0x09
#define HID_SET_IDLE                0x0A
#define HID_GET_REPORT              0x01
#define HID_SET_PROTOCOL            0x0B


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

#define IN_MAX_PKT_SIZE     64
#define OUT_MAX_PKT_SIZE    64

#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_HID_DESCRIPTOR_TYPE       0x21

static const uint8_t default_hid_report_desc[] =
{
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x00, // USAGE (0)
    0xA1, 0x01, // COLLECTION (Application)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0xFF, // LOGICAL_MAXIMUM (255)
    0x19, 0x01, // USAGE_MINIMUM (1)
    0x29, 0x08, // USAGE_MAXIMUM (8)
    0x95, 0x40, // REPORT_COUNT (8)
    0x75, 0x08, // REPORT_SIZE (8)
    0x81, 0x02, // INPUT (Data,Var,Abs)
    0x19, 0x01, // USAGE_MINIMUM (1)
    0x29, 0x08, // USAGE_MAXIMUM (8)
    0x91, 0x02, // OUTPUT (Data,Var,Abs)
    0xC0        // END_COLLECTION
};

#define INTERFACE_LEN  0x09

static const uint8_t hid_interface[] =
{
	/* I/F descr: HID */
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
	sizeof(default_hid_report_desc) & 0x00FF,
	(sizeof(default_hid_report_desc) & 0xFF00) >> 8,

	/* EP Descriptor: interrupt in. */
	0x07,   /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,  /* bDescriptorType */
	(TO_BE_SET_AUTOMATICALLY | 0x08), /* bEndpointAddress : Input */
	0x03,         /* bmAttributes : interrupt*/
	/* wMaxPacketSize */
	IN_MAX_PKT_SIZE & 0x00FF,
	(IN_MAX_PKT_SIZE & 0xFF00) >> 8,
	10,        /* bInterval */

	/* EP Descriptor: interrupt out. */
	0x07,   /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,  /* bDescriptorType */
	TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress */
	0x03,         /* bmAttributes : interrupt */
	/* wMaxPacketSize */
	OUT_MAX_PKT_SIZE & 0x00FF,
	(OUT_MAX_PKT_SIZE & 0xFF00) >> 8,
	10     /* bInterval */
};



#define REPORT_SIZE  8
static uint8_t report[REPORT_SIZE];

static void new_data_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct dev_desc_t * callback_rx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	callback_rx_dev = cfg_hndl->callback_rx_dev;
	if (NULL == callback_rx_dev)
	{
		return;
	}
	DEV_CALLBACK_2_PARAMS(callback_rx_dev, CALLBACK_DATA_RECEIVED, buff, size);
}



static void end_of_transmit_callback(struct dev_desc_t *adev)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct usb_hid_class_runtime_t  *runtime_hndl;
	struct dev_desc_t * callback_tx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);


	callback_tx_dev = cfg_hndl->callback_tx_dev;
	if ((NULL != callback_tx_dev) && (1 == runtime_hndl->tx_on))
	{
		DEV_CALLBACK_1_PARAMS(callback_tx_dev,
							CALLBACK_TX_DONE, (void*)(runtime_hndl->sentLen));
	}

}


/**
 * usb_hid_pwrite()
 *
 * return:
 */
size_t usb_hid_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct dev_desc_t *usb_hw;
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct set_data_to_in_endpoint_t set_data_to_in_endpoint;
	struct usb_hid_class_runtime_t  *runtime_hndl;
	size_t sentLen;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	usb_hw = cfg_hndl->usb_hw;

	if (aLength > OUT_MAX_PKT_SIZE)
	{
		sentLen = OUT_MAX_PKT_SIZE;
	}
	else
	{
		sentLen = aLength;
	}

	runtime_hndl->sentLen = sentLen;
	runtime_hndl->tx_on = 1;

	set_data_to_in_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
	set_data_to_in_endpoint.data = apData;
	set_data_to_in_endpoint.size = sentLen;
	DEV_IOCTL_1_PARAMS(usb_hw,
		IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &set_data_to_in_endpoint);

	return sentLen;
}


/* hid_class_in_request()
 *
 */
static void hid_class_in_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;
	uint8_t ret;

	// Device to host
	ret = 0;
	switch(request[1])
	{
    case HID_GET_REPORT:
		set_request_in_buffer.data = (uint8_t *)&report;
		set_request_in_buffer.size = REPORT_SIZE;
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
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_SATLL);
	}
}





/*  hid_class_out_request()
 *
 */
static void hid_class_out_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_out_buffer_t set_request_out_buffer;
	uint8_t ret;

	ret = 0;
    switch(request[1])
	{
    case HID_SET_REPORT:
		set_request_out_buffer.data = (uint8_t *)&report;
		set_request_out_buffer.size = REPORT_SIZE;
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
    	DEV_IOCTL_1_PARAMS(usb_hw,
    		IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER, &set_request_out_buffer);
    }
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_SATLL);
	}
}


static void hid_class_request(struct dev_desc_t *callback_dev, uint8_t *request)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80)    /* request data transfer direction */
	{// from device to host
		hid_class_in_request(usb_hw, request);
	}
	else
	{// from host to device
		hid_class_out_request(usb_hw, request);
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
	uint8_t    endpoints_type_arr[2];
	uint16_t   max_pckt_sizes[2];
	uint8_t in_endpoint_num;
	uint8_t out_endpoint_num;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);
	usb_hw = cfg_hndl->usb_hw;


	set_endpoints.num_of_endpoints = 2;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	out_func_arr[0] = NULL;
	out_func_arr[1] = new_data_received;
	in_func_arr[0] = end_of_transmit_callback;
	in_func_arr[1] = NULL;
	set_endpoints.out_func_arr = out_func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.callback_dev = adev;
	max_pckt_sizes[0] = IN_MAX_PKT_SIZE;
	max_pckt_sizes[1] = OUT_MAX_PKT_SIZE;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	endpoints_type_arr[0] = USB_DEVICE_API_EP_TYPE_INTERRUPT_IN;
	endpoints_type_arr[1] = USB_DEVICE_API_EP_TYPE_INTERRUPT_OUT;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);

	in_endpoint_num = endpoints_num_arr[0];
	out_endpoint_num = endpoints_num_arr[1];
	interface[18 + 2] = 0x80 | in_endpoint_num;// 0x80 for IN endpoint
	interface[25 + 2] = out_endpoint_num;
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
	struct usb_descriptors_hid_descriptor_t  *p_hid_descriptor;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	iad = (uint8_t*)malloc(sizeof(interface_association_descriptor)) ;
	memcpy(iad, interface_association_descriptor,
						sizeof(interface_association_descriptor));
	iad[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR, iad);
	free(iad);



	interface = (uint8_t*)malloc(sizeof(hid_interface)) ;
	memcpy(interface, hid_interface, sizeof(hid_interface));
	interface[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];

	configure_endpoints(adev, cfg_hndl, interface);

	usb_desc_add_interface.interface_desc = interface;
	usb_desc_add_interface.interface_desc_size = sizeof(hid_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	usb_desc_add_interface.alt_interface_desc_size = 0;
	usb_desc_add_interface.is_hid_interface = 1;
	p_hid_descriptor = &usb_desc_add_interface.hid_descriptor;
	p_hid_descriptor->hid_desc_position_in_config_desc = INTERFACE_LEN;
	p_hid_descriptor->report_descriptor = default_hid_report_desc;
	p_hid_descriptor->size = sizeof(default_hid_report_desc);
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
	free(interface);
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


	usb_descriptors_alloc_interfaces.num_of_interfaces = 1;
	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
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
	DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_hid_class_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_hid_class_cfg_t *cfg_hndl;
	struct usb_hid_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		start_hid_class(adev, cfg_hndl, runtime_hndl);
		break;
	case IOCTL_UART_DISABLE_TX :
		runtime_hndl->tx_on = 0;
		break;
	default :
		return 1;
	}
	return 0;
}
