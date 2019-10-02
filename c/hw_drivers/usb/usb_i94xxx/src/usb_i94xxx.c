/*
 *
 * usb_i94xxx.c

 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "usb_i94xxx_api.h"
#include "usb_i94xxx.h"
#include "I94100.h"

#include "irq_api.h"
#include "usb_device_api.h"
#include "clock_control_i94xxx_api.h"
#include "pin_control_api.h"

#include "usbd.h"
#include "string.h"

#include "os_wrapper.h"

//#include "audio_class.h"


#if !defined(INTERRUPT_PRIORITY_FOR_USBD)
	#error "INTERRUPT_PRIORITY_FOR_USBD should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_USBD)
	#error "priority should be lower then maximal priority for os syscalls"
#endif


/********  defines *********************/
/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    64 // must be aligned by 8 bytes
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE // must be aligned by 8 bytes

#define SETUP_BUF_BASE      0
#define SETUP_BUF_LEN       8// must be aligned by 8 bytes
#define EP0_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN         (EP0_MAX_PKT_SIZE)
#define EP1_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN         (EP1_MAX_PKT_SIZE)


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/

/* ------------------------ local variables --------*/
static S_USBD_INFO_T l_gsInfo;

#define MAX_NUM_OF_ENDPOINTS    12
static usb_dev_out_endpoint_callback_func_t
				out_callback_functions[MAX_NUM_OF_ENDPOINTS];
static usb_dev_in_endpoint_callback_func_t
				in_callback_functions[MAX_NUM_OF_ENDPOINTS];
static struct dev_desc_t *endpoint_callback_devs[MAX_NUM_OF_ENDPOINTS];
static uint16_t max_pckt_sizes[MAX_NUM_OF_ENDPOINTS] = {0};
#define MAX_NUM_OF_ITERFACES    16
static struct dev_desc_t *interface_callback_devs[MAX_NUM_OF_ITERFACES];
usb_dev_interface_request_callback_func_t
				interface_callback_functions[MAX_NUM_OF_ITERFACES];
usb_dev_endpoint_request_callback_func_t
				endpoint_request_callback_functions[MAX_NUM_OF_ENDPOINTS];

static uint8_t endpoints_count = 2;
static uint32_t available_buff_pointer = EP1_BUF_BASE + EP1_BUF_LEN;

#define USB_STATE_SETTING  0
#define USB_STATE_STARTED  1
static uint8_t usb_state = USB_STATE_SETTING;
static uint8_t init_done = 0;
/*--------------------------------------------------------------------------*/

//Windows 10 Compiler failed without this variable even though called in an
// 		unused function.
uint32_t CyclesPerUs      = 0;

#define DBG_PRINTF(...)


#define GET_EP_CFG(ep)     \
	(*((__IO uint32_t *) ((uint32_t)&USBD->EP[0].CFG + (uint32_t)((ep) << 4))))


void EP_Handler(uint8_t ep_num)
{
	uint32_t ep_cfg_state;

	ep_cfg_state = GET_EP_CFG(ep_num) & 0x60;
	if (USBD_CFG_EPMODE_IN == ep_cfg_state)
	{
		usb_dev_in_endpoint_callback_func_t callback_func_in;

		callback_func_in = in_callback_functions[ep_num];
		if (NULL != callback_func_in)
		{
			callback_func_in(endpoint_callback_devs[ep_num]);
		}
	}
	else
	{
		usb_dev_out_endpoint_callback_func_t callback_func_out;
		uint32_t u32Len;
		uint8_t *pu8Src;

		/* Get the address in USB buffer */
		pu8Src = (uint8_t *)(
				(uint32_t)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep_num));
		u32Len = USBD_GET_PAYLOAD_LEN(ep_num);

		callback_func_out = out_callback_functions[ep_num];
		if (NULL != callback_func_out)
		{
			callback_func_out(endpoint_callback_devs[ep_num], pu8Src, u32Len);
		}
		/* Prepare for nex OUT packet */
		USBD_SET_PAYLOAD_LEN(ep_num, max_pckt_sizes[ep_num]);
	}
}



/*--------------------------------------------------------------------------*/
void USBD_IRQHandler(void)
{
	uint32_t u32IntSts = USBD_GET_INT_FLAG();
	uint32_t u32State = USBD_GET_BUS_STATE();

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_FLDET)
	{
		// Floating detect
		USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

		if(USBD_IS_ATTACHED())
		{
			// Enable GPB15(VBUS) pull down state to solute suspend event issue.
			//GPIO_EnablePullState(PB,BIT15,GPIO_PUSEL_PULL_DOWN);
			/* USB Plug In */
			USBD_ENABLE_USB();
		}
		else
		{
			// Disable GPB15 pull down state.
		//	GPIO_DisablePullState(PB,BIT15);
			/* USB Un-plug */
			USBD_DISABLE_USB();
		}
	}

	//------------------------------------------------------------------

	if ( u32IntSts & USBD_INTSTS_SOF )
	{
		/* Clear event flag */
		USBD_CLR_INT_FLAG(USBD_INTSTS_SOF);
	}

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_BUS)
	{
		/* Clear event flag */
		USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

		if(u32State & USBD_STATE_USBRST) {
			/* Bus reset */
			USBD_ENABLE_USB();
			USBD_SwReset();
			DBG_PRINTF("Bus reset\n");
		}
		if(u32State & USBD_STATE_SUSPEND) {
			/* Enable USB but disable PHY */
			USBD_DISABLE_PHY();
			DBG_PRINTF("Suspend\n");
		}
		if(u32State & USBD_STATE_RESUME) {
			/* Enable USB and enable PHY */
			USBD_ENABLE_USB();
			DBG_PRINTF("Resume\n");
		}
	}

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_USB)
	{
		uint8_t i;
		uint32_t endpoint_status_flag;

		// EP events
		if(u32IntSts & USBD_INTSTS_EP0) {
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
			// control IN
			USBD_CtrlIn();
		}

		if(u32IntSts & USBD_INTSTS_EP1) {
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

			// control OUT
			USBD_CtrlOut();
		}

		for (i = 2; i < 12; i++)
		{
			endpoint_status_flag = 1 << (USBD_INTSTS_EPEVT0_Pos + i);
			if (u32IntSts & endpoint_status_flag)
			{
				/* Clear event flag */
				USBD_CLR_INT_FLAG(endpoint_status_flag);
				EP_Handler(i);
			}
		}

		// USB event
		if(u32IntSts & USBD_INTSTS_SETUP) {
			// Setup packet
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

			/* Clear the data IN/OUT ready flag of control end-points */
			USBD_STOP_TRANSACTION(EP0);
			USBD_STOP_TRANSACTION(EP1);

			USBD_ProcessSetupPacket();
		}
	}
}


static void class_request(void)
{
	uint8_t buf[8];
	uint16_t USBwIndex;
	usb_dev_interface_request_callback_func_t callback_func;

	USBD_GetSetupPacket(buf);


	if (0x01 == (buf[0] & 0x1f))// class/interface request
	{
		USBwIndex = buf[4];
		if (MAX_NUM_OF_ITERFACES < USBwIndex)
		{
			CRITICAL_ERROR("interface number is to high \n");
		}

		callback_func = interface_callback_functions[USBwIndex];
		if (NULL != callback_func)
		{
			callback_func(interface_callback_devs[USBwIndex], buf);
		}
	}
	else if (0x02 == (buf[0] & 0x1f))// class/endpoint request
	{
		USBwIndex = buf[4];
		if (MAX_NUM_OF_ENDPOINTS < USBwIndex)
		{
			CRITICAL_ERROR("endpoint number is to high \n");
		}

		callback_func = endpoint_request_callback_functions[USBwIndex];
		if (NULL != callback_func)
		{
			callback_func(endpoint_callback_devs[USBwIndex], buf);
		}
	}

}


static void register_interfaces(
		struct register_interfaces_t *register_interfaces)
{
	uint8_t i;

	if (USB_STATE_STARTED == usb_state)
	{
		CRITICAL_ERROR("should be done before usb started");
	}
	for(i = 0; i < register_interfaces->num_of_interfaces; i++)
	{
		struct register_interface_t *register_interface;
		uint8_t interface_num;

		register_interface = &register_interfaces->register_interface_arr[i];
		interface_num = register_interface->interfaces_num;
		if (MAX_NUM_OF_ITERFACES < interface_num)
		{
			CRITICAL_ERROR("interface number is to high \n");
		}
		interface_callback_devs[interface_num] =
								register_interfaces->callback_dev;
		interface_callback_functions[interface_num] =
								register_interface->interface_func;
	}
}


static void set_request_out_buffer(
		struct set_request_out_buffer_t *set_request_out_buffer)
{
	USBD_PrepareCtrlOut(
		(uint8_t*)set_request_out_buffer->data, set_request_out_buffer->size);
	USBD_PrepareCtrlIn(0, 0);
}


static void set_request_in_buffer(
		struct set_request_in_buffer_t *set_request_in_buffer)
{
	USBD_PrepareCtrlIn(
			set_request_in_buffer->data, set_request_in_buffer->size);
	// Trigger next Control Out DATA1 Transaction.
	/* Status stage */
	USBD_PrepareCtrlOut(0, 0);
}


static void set_data_to_in_endpoint_func(
		struct set_data_to_in_endpoint_t *set_data_to_in_endpoint)
{
	uint8_t endpoint_num;
	uint8_t *pu8Dest;
	uint8_t const *pu8Src;
	size_t  size;
	uint16_t i;

	endpoint_num = set_data_to_in_endpoint->endpoint_num;
	pu8Dest = (uint8_t *)((uint32_t)USBD_BUF_BASE +
						USBD_GET_EP_BUF_ADDR(endpoint_num));

	size = set_data_to_in_endpoint->size;
	pu8Src = set_data_to_in_endpoint->data;
	//MUST BE COPIED BY  SINGLE BYTES
	//memcpy(pu8Dest, set_data_to_in_endpoint->data, size);

	for (i = 0; i < size; i++)
	{
		*pu8Dest++ = *pu8Src++;
	}
	USBD_SET_PAYLOAD_LEN(endpoint_num, size);
}


static void set_endpoint_func(struct set_endpoints_t *set_endpoints)
{
	int i;

	if (USB_STATE_STARTED == usb_state)
	{
		CRITICAL_ERROR("should be done before usb started");
	}
	for (i = 0; i < set_endpoints->num_of_endpoints; i++)
	{
		uint16_t max_pckt_size;
		uint8_t endpoint_type;

		if (MAX_NUM_OF_ENDPOINTS == endpoints_count)
		{
			CRITICAL_ERROR("no free endpoints left \n");
		}
		set_endpoints->endpoints_num_arr[i] = endpoints_count;
		endpoint_callback_devs[endpoints_count] = set_endpoints->callback_dev;
		endpoint_request_callback_functions[endpoints_count] =
							set_endpoints->endpoint_request_callback_func[i];


		/* Buffer range for endpoint */
		USBD_SET_EP_BUF_ADDR(endpoints_count, available_buff_pointer);
		max_pckt_size = set_endpoints->max_pckt_sizes[i];
		available_buff_pointer += max_pckt_size;
		// hw requires alignment by 8 bytes:
		available_buff_pointer = (available_buff_pointer + 7) & (~0x7);

		if (1024 < available_buff_pointer)
		{
			CRITICAL_ERROR("usb sram area is 1024 bytes only");
		}
		max_pckt_sizes[endpoints_count] = max_pckt_size;

		endpoint_type = set_endpoints->endpoints_type_arr[i];
		switch (endpoint_type)
		{
		case USB_DEVICE_API_EP_TYPE_ISO_OUT :
			out_callback_functions[endpoints_count] =
									set_endpoints->out_func_arr[i];
			in_callback_functions[endpoints_count] = NULL;
			USBD_CONFIG_EP(endpoints_count,
					USBD_CFG_EPMODE_OUT | USBD_CFG_TYPE_ISO | endpoints_count);
			/* trigger to receive OUT data */
			USBD_SET_PAYLOAD_LEN(endpoints_count, max_pckt_size);
			break;
		case USB_DEVICE_API_EP_TYPE_BULK_OUT :
		case USB_DEVICE_API_EP_TYPE_INTERRUPT_OUT :
			out_callback_functions[endpoints_count] =
									set_endpoints->out_func_arr[i];
			in_callback_functions[endpoints_count] = NULL;
			USBD_CONFIG_EP(endpoints_count,
					USBD_CFG_EPMODE_OUT | endpoints_count);
			/* trigger to receive OUT data */
			USBD_SET_PAYLOAD_LEN(endpoints_count, max_pckt_size);
			break;
		case USB_DEVICE_API_EP_TYPE_BULK_IN :
		case USB_DEVICE_API_EP_TYPE_INTERRUPT_IN :
			in_callback_functions[endpoints_count] =
								set_endpoints->in_func_arr[i];
			out_callback_functions[endpoints_count] = NULL;
			USBD_CONFIG_EP(endpoints_count,
					USBD_CFG_EPMODE_IN | endpoints_count);
			break;
		case USB_DEVICE_API_EP_TYPE_ISO_IN :
			in_callback_functions[endpoints_count] =
								set_endpoints->in_func_arr[i];
			out_callback_functions[endpoints_count] = NULL;
			USBD_CONFIG_EP(endpoints_count,
					USBD_CFG_EPMODE_IN | USBD_CFG_TYPE_ISO | endpoints_count);
			break;
		default :
			CRITICAL_ERROR("NOT EMPLEMENTED YET \n");
		}

		endpoints_count++;
		//if (6 == endpoints_count) endpoints_count +=2; //TODO : to remove
	}
}


static void device_start(struct usb_i94xxx_cfg_t *cfg_hndl)
{
	uint32_t freq;

	if (init_done)
	{
		return;
	}
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B13_USBD_DN);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B14_USBD_DP);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B15_USBD_VBUS);

	DEV_IOCTL_0_PARAMS(i94xxx_usb_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(i94xxx_usb_clk_dev,
						CLK_IOCTL_SET_PARENT, cfg_hndl->src_clock);
	DEV_IOCTL_0_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_ENABLE);
	freq = 48000000;
	DEV_IOCTL_1_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_SET_FREQ, &freq);
	DEV_IOCTL_0_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_ENABLE);

	//irq_register_device_on_interrupt(USBD_IRQn, adev);
	irq_register_interrupt(USBD_IRQn, USBD_IRQHandler);
	irq_set_priority(USBD_IRQn, INTERRUPT_PRIORITY_FOR_USBD );
	irq_enable_interrupt(USBD_IRQn);
	init_done = 1;
}


static void usb_device_start()
{
	if ((NULL == l_gsInfo.gu8DevDesc) || (NULL == l_gsInfo.gu8ConfigDesc) ||
		(NULL == l_gsInfo.gu8StringDesc)  || (NULL == l_gsInfo.gu32BOSDesc))
	{
		CRITICAL_ERROR("l_gsInfo structure should be initialized");
	}

	if (USB_STATE_SETTING == usb_state)
	{
		usb_state = USB_STATE_STARTED;
	}
	else if (USB_STATE_STARTED == usb_state)
	{
		return;
	}

	/* usb initial */
	USBD_Open(&l_gsInfo, class_request, NULL);

	/* Endpoint configuration */

	/* Init setup packet buffer */
	/* Buffer range for setup packet -> [0 ~ 0x7] */
	USBD->STBUFSEG = SETUP_BUF_BASE;

	/*****************************************************/
	/* EP0 ==> control IN endpoint, address 0 */
	USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
	/* Buffer range for EP0 */
	USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

	/* EP1 ==> control OUT endpoint, address 0 */
	USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
	/* Buffer range for EP1 */
	USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);


	/* use following code instead of USBD_Start() because USBD_Start uses
	* CLK_SysTickDelay function and it touches systick that is used in our
	* framework as OS tick
	*/
	os_delay_ms(100);
	/* Disable software-disconnect function */
	USBD_CLR_SE0();

	USBD->ATTR |= USBD_ATTR_DPPUEN_Msk;

	/* Clear USB-related interrupts before enable interrupt */
	USBD_CLR_INT_FLAG(USBD_INT_BUS |
			USBD_INT_USB | USBD_INT_FLDET | USBD_INT_WAKEUP);

	/* Enable USB-related interrupts. */
	USBD_ENABLE_INT(USBD_INT_BUS | USBD_INT_USB |
			USBD_INT_FLDET | USBD_INT_WAKEUP | USBD_INTEN_SOFIEN_Msk);

}


static void set_descriptors(struct set_device_descriptors_t *descriptors)
{
	if (USB_STATE_STARTED == usb_state)
	{
		CRITICAL_ERROR("should be done before usb started");
	}
	l_gsInfo.gu8DevDesc = descriptors->device_desc;
	l_gsInfo.gu8ConfigDesc = descriptors->config_desc;
	l_gsInfo.gu8StringDesc = descriptors->pointers_to_strings_descs;
	l_gsInfo.gu8HidReportDesc = descriptors->hid_report_desc;
	l_gsInfo.gu32BOSDesc = descriptors->BOS_desc;
	l_gsInfo.gu32HidReportSize = descriptors->hid_report_size;
	l_gsInfo.gu32ConfigHidDescIdx = descriptors->config_hid_desc_index;
}


/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t usb_i94xxx_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_i94xxx_cfg_t *cfg_hndl;

	if ((0 == init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		device_start(cfg_hndl);
		break;
	case IOCTL_USB_DEVICE_SET_DESCRIPTORS :
		set_descriptors(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_START :
		usb_device_start();
		break;
	case IOCTL_USB_DEVICE_SET_ENDPOINTS :
		set_endpoint_func(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT :
		set_data_to_in_endpoint_func(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_REGISTER_INTERFACES :
		register_interfaces(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER :
		set_request_in_buffer(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER :
		set_request_out_buffer(aIoctl_param1);
		break;
	case IOCTL_USB_DEVICE_SET_STALL:
		USBD_SetStall(0);
		break;
	default :
		return 1;
	}
	return 0;
}

#define    MODULE_NAME                  usb_i94xxx
#define    MODULE_IOCTL_FUNCTION        usb_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct usb_i94xxx_cfg_t
#include "add_module.h"
