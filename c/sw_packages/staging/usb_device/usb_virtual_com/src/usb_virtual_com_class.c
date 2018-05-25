/*
 *
 * usb_virtual_com_stm32f103x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include "usb_virtual_com_class_api.h"
#include "usb_virtual_com_class.h"
#include "uart_api.h"
#include "usb_device_api.h"
#include "usb_device_descriptors_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "usb_virtual_com_class_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/



typedef struct
{
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
}LINE_CODING;


/********  externals *********************/


/********  local defs *********************/




#define USB_MAX_DATA_SIZE   64
static uint8_t USB_Rx_Buffer[USB_MAX_DATA_SIZE];

#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_DESC_INT_SIZE  0x08

#define TO_BE_SET_AUTOMATICALLY   0
#define AUTO_SET_PCKT_SIZE_MSB   0x00
#define AUTO_SET_PCKT_SIZE_LSB   0x00

static uint8_t interface_association_descriptor[]=
{
	/*  Interface Association Descriptor:  */
	0x08,
	0x0B ,  //    bDescriptorType
	TO_BE_SET_AUTOMATICALLY, // bFirstInterface
	0x02    , // bInterfaceCount
	0x02    , // bFunctionClass
	0x00    , // bFunctionSubClass
	0x00    , // bFunctionProtocol
	0x02    ,//  iFunction
};

static uint8_t cdc_interface[]=
{
	/*Interface Descriptor*/
	0x09,   /* bLength: Interface Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	TO_BE_SET_AUTOMATICALLY,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x01,   /* bNumEndpoints: One endpoints used */
	0x02,   /* bInterfaceClass: Communication Interface Class */
	0x02,   /* bInterfaceSubClass: Abstract Control Model */
	0x01,   /* bInterfaceProtocol: Common AT commands */
	0x00,   /* iInterface: */
	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,
	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	TO_BE_SET_AUTOMATICALLY,   /* bDataInterface: 1 */
	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
//	0x00,   /* bmCapabilities */
	0x02,   /* bmCapabilities */
	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	TO_BE_SET_AUTOMATICALLY,   /* bMasterInterface: Communication class interface */
	TO_BE_SET_AUTOMATICALLY,   /* bSlaveInterface0: Data Class Interface */
	/*Endpoint 2 Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (IN) */
	0x03,   /* bmAttributes: Interrupt */
	USB_DESC_INT_SIZE,      /* wMaxPacketSize: */
	0x00,
	0xFF,   /* bInterval: */
};


static uint8_t vcom_interface[]=
{
	    /*Data class interface descriptor*/
	    0x09,   /* bLength: Endpoint Descriptor size */
	    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
	    TO_BE_SET_AUTOMATICALLY,   /* bInterfaceNumber: Number of Interface */
	    0x00,   /* bAlternateSetting: Alternate setting */
	    0x02,   /* bNumEndpoints: Two endpoints used */
	    0x0A,   /* bInterfaceClass: CDC */
	    0x00,   /* bInterfaceSubClass: */
	    0x00,   /* bInterfaceProtocol: */
	    0x00,   /* iInterface: */
	    /*Endpoint 3 Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (OUT) */
	    0x02,   /* bmAttributes: Bulk */
	    USB_MAX_DATA_SIZE,             /* wMaxPacketSize: */
	    0x00,
	    0x00,   /* bInterval: ignore for Bulk transfer */
	    /*Endpoint 1 Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (IN) */
	    0x02,   /* bmAttributes: Bulk */
	    USB_MAX_DATA_SIZE,             /* wMaxPacketSize: */
	    0x00,
	    0x00,    /* bInterval */
};


LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
  };


static void new_data_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t  *runtime_hndl;
	struct dev_desc_t * callback_rx_dev ;
//	uint16_t i;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

//	for (i = 0; i < USB_MAX_DATA_SIZE; i++)
//	{
//		USB_Rx_Buffer[i] = *buff++;
//	}

	callback_rx_dev = cfg_hndl->callback_rx_dev;
	if (NULL != callback_rx_dev)
	{
		DEV_CALLBACK_2_PARAMS(callback_rx_dev,
						CALLBACK_DATA_RECEIVED,  buff,  size);
	}

}




static void end_of_transmit_callback(struct dev_desc_t *adev)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t  *runtime_hndl;
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
 * usb_virtual_com_pwrite()
 *
 * return:
 */
size_t usb_virtual_com_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct dev_desc_t *usb_hw;
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct set_data_to_in_endpoint_t set_data_to_in_endpoint;
	struct usb_virtual_com_class_runtime_t  *runtime_hndl;
	size_t sentLen;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	usb_hw = cfg_hndl->usb_hw;

	if (aLength > USB_MAX_DATA_SIZE)
	{
		sentLen = USB_MAX_DATA_SIZE;
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




static void start_virtual_com_class(struct dev_desc_t *adev,
		struct usb_virtual_com_class_cfg_t *cfg_hndl,
		struct usb_virtual_com_class_runtime_t *runtime_hndl)
{
	struct set_endpoints_t set_endpoints;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[3];
	uint8_t   endpoints_num_arr[3];
	usb_dev_out_endpoint_callback_func_t   func_arr[3];
	uint8_t    endpoints_type_arr[3];
	uint16_t   max_pckt_sizes[3];

	if (1 == runtime_hndl->init_done)
	{
		return;
	}
	runtime_hndl->init_done = 1;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	usb_hw = cfg_hndl->usb_hw;
	usb_descriptors_alloc_interfaces.num_of_interfaces = 2;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);

	interface_association_descriptor[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[0];

	cdc_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	cdc_interface[14 + 4] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	cdc_interface[23 + 3] =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	cdc_interface[23 + 4] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	vcom_interface[2] =
			usb_descriptors_alloc_interfaces.interfaces_num[1];


	set_endpoints.num_of_endpoints = 3;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	func_arr[0] = NULL;
	func_arr[1] = new_data_received;
	func_arr[2] = NULL;
	in_func_arr[0] = NULL;
	in_func_arr[1] = NULL;
	in_func_arr[2] = end_of_transmit_callback;
	set_endpoints.func_arr = func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.callback_dev = adev;
	max_pckt_sizes[0] = USB_DESC_INT_SIZE;
	max_pckt_sizes[1] = USB_MAX_DATA_SIZE;
	max_pckt_sizes[2] = USB_MAX_DATA_SIZE;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	endpoints_type_arr[0] = USB_DEVICE_API_EP_TYPE_INTERRUPT_IN;
	endpoints_type_arr[1] = USB_DEVICE_API_EP_TYPE_BULK_OUT;
	endpoints_type_arr[2] = USB_DEVICE_API_EP_TYPE_BULK_IN;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);


	cdc_interface[28 + 2] = 0x80 | endpoints_num_arr[0];// 0x80 for IN endpoint
	vcom_interface[9 + 2] = endpoints_num_arr[1];
	vcom_interface[16 + 2] = 0x80 | endpoints_num_arr[2];
	runtime_hndl->in_endpoint_num = endpoints_num_arr[2];

	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR,
			interface_association_descriptor);

	usb_desc_add_interface.interface_desc = cdc_interface;
	usb_desc_add_interface.interface_desc_size =
								sizeof(cdc_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);


	usb_desc_add_interface.interface_desc = vcom_interface;
	usb_desc_add_interface.interface_desc_size = sizeof(vcom_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);


//	buff_size = cfg_hndl->buff_size;
//	for (i = 0; i < USB_AUDIO_CLASS_NUM_OF_BUFFERS; i++)
//	{
//		runtime_hndl->buff[i] = (uint8_t*)malloc(buff_size);
//		runtime_hndl->buff_status[i] = USB_AUDIO_CLASS_BUFF_IDLE ;
//	}
}



/**
 * usb_virtual_com_class_ioctl()
 *
 * return:
 */
uint8_t usb_virtual_com_class_ioctl(struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		break;
	case IOCTL_USB_VIRTUAL_COM_START :
		start_virtual_com_class(adev, cfg_hndl, runtime_hndl);
		break;
	case IOCTL_UART_DISABLE_TX :
		runtime_hndl->tx_on = 0;
		break;
	default :
		return 1;
	}
	return 0;
}

