/*
 *
 * usb_i9xxxx.c

 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "usb_i9xxxx_api.h"
#ifdef CONFIG_I94XXX
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#endif


#include "irq_api.h"
#include "usb_device_api.h"
#include "pin_control_api.h"
#include "errors_api.h"

#include "usbd.h"
#include "string.h"

#include "os_wrapper.h"
#include "usb_i9xxxx.h"

//#include "audio_class.h"


#if !defined(INTERRUPT_PRIORITY_FOR_USBD)
	#error "INTERRUPT_PRIORITY_FOR_USBD should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_USBD)
	#error "priority should be lower then maximal priority for os syscalls"
#endif


/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    64 // must be aligned by 8 bytes
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE // must be aligned by 8 bytes

#define SETUP_BUF_BASE      0
#define SETUP_BUF_LEN       8// must be aligned by 8 bytes
#define EP0_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN         (EP0_MAX_PKT_SIZE)
#define EP1_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN         (EP1_MAX_PKT_SIZE)


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
static usb_dev_interface_request_callback_func_t
						last_interface_callback_func = NULL;
static struct dev_desc_t *last_interface_dev = NULL;
static usb_device_suspend_callback_t  usb_device_suspend_callback_func = NULL;
static usb_device_resume_callback_t  usb_device_resume_callback_func = NULL;
static usb_device_reset_callback_t  usb_device_reset_callback_func = NULL;

static uint8_t endpoints_count = 2;
static uint32_t available_buff_pointer = EP1_BUF_BASE + EP1_BUF_LEN;

#define USB_STATE_SETTING  0
#define USB_STATE_STARTED  1
static uint8_t usb_state = USB_STATE_SETTING;
static uint8_t init_done = 0;


//Windows 10 Compiler failed without this variable even though called in an
// 		unused function.
uint32_t CyclesPerUs      = 0;

#define DBG_PRINTF(...)


#define GET_EP_CFG(ep)     \
	(*((__IO uint32_t *) ((uint32_t)&USBD->EP[0].CFG + (uint32_t)((ep) << 4))))


static uint8_t *tmp_USB_out_buff = NULL;
static size_t max_endpoint_size = 0;

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
		if (max_endpoint_size < u32Len)
		{
			u32Len = max_endpoint_size;
		}
		// CANNOT USE REGULAR memcpy(). SHOULD BE COPIED BYTE BY BYTE
		USBD_MemCopy(tmp_USB_out_buff, pu8Src, u32Len);

		callback_func_out = out_callback_functions[ep_num];
		if (NULL != callback_func_out)
		{
			callback_func_out(
					endpoint_callback_devs[ep_num], tmp_USB_out_buff, u32Len);
		}
		/* Prepare for nex OUT packet */
		USBD_SET_PAYLOAD_LEN(ep_num, max_pckt_sizes[ep_num]);
	}
}



static void int_status_floating_detect()
{
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


static void int_status_bus_events()
{
	uint32_t u32State = USBD_GET_BUS_STATE();

	/* Clear event flag */
	USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

	if(u32State & USBD_STATE_USBRST)
	{
		/* Bus reset */
		USBD_ENABLE_USB();
		USBD_SwReset();
		if (NULL != usb_device_reset_callback_func)
		{
			usb_device_reset_callback_func();
		}
		DBG_PRINTF("Bus reset\n");
	}
	if(u32State & USBD_STATE_SUSPEND)
	{
		/* Enable USB but disable PHY */
		USBD_DISABLE_PHY();
		if (NULL != usb_device_suspend_callback_func)
		{
			usb_device_suspend_callback_func();
		}
		DBG_PRINTF("Suspend\n");
	}
	if(u32State & USBD_STATE_RESUME)
	{
		/* Enable USB and enable PHY */
		USBD_ENABLE_USB();
		if (NULL != usb_device_resume_callback_func)
		{
			usb_device_resume_callback_func();
		}
		DBG_PRINTF("Resume\n");
	}
}


static void int_status_usb_events(uint32_t u32IntSts)
{
	uint8_t i;
	uint32_t endpoint_status_flag;
	uint8_t get_out_data_done;

	// EP events
	if(u32IntSts & USBD_INTSTS_EP0)
	{
		USBD_CLR_INT_FLAG(USBD_INTSTS_EP0); /* Clear event flag */
		USBD_CtrlIn(); // control IN
	}

	if(u32IntSts & USBD_INTSTS_EP1)
	{
		USBD_CLR_INT_FLAG(USBD_INTSTS_EP1); /* Clear event flag */

		get_out_data_done = 0;
		USBD_CtrlOut(&get_out_data_done); // control OUT
		if ((get_out_data_done) && (NULL != last_interface_callback_func))
		{
			last_interface_callback_func(last_interface_dev,
					INTERFACE_CALLBACK_TYPE_DATA_OUT_FINISHED, NULL);
		}
	}

	for (i = 2; i < 12; i++)
	{
		endpoint_status_flag = 1 << (USBD_INTSTS_EPEVT0_Pos + i);
		if (u32IntSts & endpoint_status_flag)
		{
			USBD_CLR_INT_FLAG(endpoint_status_flag); /* Clear event flag */
			EP_Handler(i);
		}
	}

	if(u32IntSts & USBD_INTSTS_SETUP) // Setup packet
	{
		USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);/* Clear event flag */

		/* Clear the data IN/OUT ready flag of control end-points */
		USBD_STOP_TRANSACTION(EP0);
		USBD_STOP_TRANSACTION(EP1);

		USBD_ProcessSetupPacket();
	}
}


/* function : USBD_IRQHandler()
 *
 */
void USBD_IRQHandler(void)
{
	uint32_t u32IntSts = USBD_GET_INT_FLAG();

	if(u32IntSts & USBD_INTSTS_FLDET)
	{
		int_status_floating_detect();
	}

	if ( u32IntSts & USBD_INTSTS_SOF )
	{
		/* just clear event flag for now*/
		USBD_CLR_INT_FLAG(USBD_INTSTS_SOF);
	}

	if(u32IntSts & USBD_INTSTS_BUS)
	{
		int_status_bus_events();
	}

	if(u32IntSts & USBD_INTSTS_USB)
	{
		int_status_usb_events(u32IntSts);
	}
}


#define REQ_TYPE_POS           0
#define W_INDEX_LOW_POS        4
#define W_INDEX_HIGH_POS       4
#define INTERFACE_ID_POS    W_INDEX_LOW_POS
#define ENDPOINT_ID_POS     W_INDEX_LOW_POS
#define REQ_TYPE_MASK         0x60
#define REQ_TYPE_CLASS        0x20
#define REQ_RECIPIENT_MASK    0x1f
#define REQ_RECIPIENT_INTERFACE    0x01
#define REQ_RECIPIENT_ENDPOINT     0x02

static void class_request(void)
{
	uint8_t buf[8];
	uint16_t USBwIndex;
	uint8_t recipient;

	USBD_GetSetupPacket(buf);

	if (REQ_TYPE_CLASS != (buf[REQ_TYPE_POS] & REQ_TYPE_MASK))
	{
		return ;
	}

	recipient = buf[REQ_TYPE_POS] & REQ_RECIPIENT_MASK;
	if (REQ_RECIPIENT_INTERFACE == recipient)
	{
		usb_dev_interface_request_callback_func_t callback_func;

		USBwIndex = buf[INTERFACE_ID_POS];
		if (MAX_NUM_OF_ITERFACES < USBwIndex)
		{
			CRITICAL_ERROR("interface number is to high \n");
		}

		callback_func = interface_callback_functions[USBwIndex];
		last_interface_callback_func = callback_func;
		last_interface_dev = interface_callback_devs[USBwIndex];
		if (NULL != callback_func)
		{
			callback_func(
					last_interface_dev, INTERFACE_CALLBACK_TYPE_REQUEST, buf);
		}
	}
	else if (REQ_RECIPIENT_ENDPOINT == recipient)
	{
		usb_dev_endpoint_request_callback_func_t callback_func;

		USBwIndex = buf[ENDPOINT_ID_POS];
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


static void standard_set_interface_request(void)
{
	uint8_t buf[8];
	uint16_t USBwIndex;
	usb_dev_interface_request_callback_func_t callback_func;
	static struct dev_desc_t *interface_dev = NULL;

	USBD_GetSetupPacket(buf);

	USBwIndex = buf[INTERFACE_ID_POS];
	if (MAX_NUM_OF_ITERFACES < USBwIndex)
	{
		CRITICAL_ERROR("interface number is to high \n");
	}
	callback_func = interface_callback_functions[USBwIndex];
	interface_dev = interface_callback_devs[USBwIndex];
	if (NULL != callback_func)
	{
		callback_func(interface_dev,
				INTERFACE_CALLBACK_TYPE_STANDARD_SET_INTERFACE, buf);
	}

}


static void register_interfaces(
		struct register_interfaces_t *p_register_interfaces)
{
	uint8_t i;

	if (USB_STATE_STARTED == usb_state)
	{
		CRITICAL_ERROR("should be done before usb started");
	}
	for(i = 0; i < p_register_interfaces->num_of_interfaces; i++)
	{
		struct register_interface_t *register_interface;
		uint8_t interface_num;

		register_interface = &p_register_interfaces->register_interface_arr[i];
		interface_num = register_interface->interfaces_num;
		if (MAX_NUM_OF_ITERFACES < interface_num)
		{
			CRITICAL_ERROR("interface number is to high \n");
		}
		interface_callback_devs[interface_num] =
						p_register_interfaces->callback_dev;
		interface_callback_functions[interface_num] =
								register_interface->interface_func;
	}
}


static void set_request_out_buffer(
		struct set_request_out_buffer_t *p_set_request_out_buffer)
{
	USBD_PrepareCtrlOut(
		p_set_request_out_buffer->data, p_set_request_out_buffer->size);
	USBD_PrepareCtrlIn(0, 0);
}


static void set_request_in_buffer(
		struct set_request_in_buffer_t *p_set_request_in_buffer)
{
	USBD_PrepareCtrlIn(
			p_set_request_in_buffer->data, p_set_request_in_buffer->size);
	// Trigger next Control Out DATA1 Transaction.
	/* Status stage */
	USBD_PrepareCtrlOut(0, 0);
}


static void set_data_to_in_endpoint_func(
		struct set_data_to_in_endpoint_t *set_data_to_in_endpoint)
{
	uint8_t endpoint_num;
	uint8_t *pu8Dest;
	size_t size;

	endpoint_num = set_data_to_in_endpoint->endpoint_num;
	pu8Dest = (uint8_t *)((uint32_t)USBD_BUF_BASE +
						USBD_GET_EP_BUF_ADDR(endpoint_num));

	size = set_data_to_in_endpoint->size;
	//MUST BE COPIED BY  SINGLE BYTES. CANNOT USE memcpy()
	//memcpy(pu8Dest, set_data_to_in_endpoint->data, size);
	USBD_MemCopy(pu8Dest, (uint8_t*)set_data_to_in_endpoint->data, size);
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
		if ((0 == max_pckt_size) || (1024 <= max_pckt_size))
		{
			CRITICAL_ERROR("unsupported endpoint size\n");
		}

		if (max_endpoint_size < max_pckt_size)
		{
			tmp_USB_out_buff =
					(uint8_t*) os_safe_realloc(tmp_USB_out_buff, max_pckt_size);
			errors_api_check_if_malloc_succeed(tmp_USB_out_buff);
			max_endpoint_size = max_pckt_size;
		}

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


static void device_start(struct usb_i9xxxx_cfg_t *cfg_hndl)
{
	uint32_t freq;
	struct dev_desc_t  *usb_clk_dev;

	if (init_done)
	{
		return;
	}
#if defined(CONFIG_I94XXX)
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B13_USBD_DN);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B14_USBD_DP);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B15_USBD_VBUS);
	usb_clk_dev = i94xxx_usb_clk_dev;
#elif defined(CONFIG_I96XXX_M0)
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B01_USBD_DN);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B02_USBD_DP);
	pin_control_api_set_pin_function(PIN_CONTROL_DT_I94XXX_PIN_B00_USBD_VBUS);
	usb_clk_dev = i96xxx_usb_clk_dev;
#endif

	DEV_IOCTL_0_PARAMS(usb_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(usb_clk_dev, CLK_IOCTL_SET_PARENT, cfg_hndl->src_clock);
	DEV_IOCTL_0_PARAMS(usb_clk_dev, CLK_IOCTL_ENABLE);
	freq = 48000000;
	DEV_IOCTL_1_PARAMS(usb_clk_dev, CLK_IOCTL_SET_FREQ, &freq);
	DEV_IOCTL_0_PARAMS(usb_clk_dev, CLK_IOCTL_ENABLE);

	//irq_register_device_on_interrupt(USBD_IRQn, adev);
	irq_register_interrupt(USBD_IRQn, USBD_IRQHandler);
	irq_set_priority(USBD_IRQn, INTERRUPT_PRIORITY_FOR_USBD );
	irq_enable_interrupt(USBD_IRQn);
	init_done = 1;
}


static void usb_device_start()
{
	if ((NULL == l_gsInfo.gu8DevDesc) || (NULL == l_gsInfo.gu8ConfigDesc) ||
		(NULL == l_gsInfo.get_str_desc_func) || (NULL == l_gsInfo.gu32BOSDesc))
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
	USBD_Open(&l_gsInfo, class_request, standard_set_interface_request);

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


static usb_device_get_str_desc_func_t usb_device_get_str_desc_func;
static void get_str_desc_func(uint16_t str_desc_index, const uint8_t **desc)
{
	usb_device_get_str_desc_func(str_desc_index, desc);
}


static void set_descriptors(struct set_device_descriptors_t *descriptors)
{
	if (USB_STATE_STARTED == usb_state)
	{
		CRITICAL_ERROR("should be done before usb started");
	}
	usb_device_get_str_desc_func =
			descriptors->usb_device_get_str_desc_func;
	l_gsInfo.gu8DevDesc = descriptors->device_desc;
	l_gsInfo.gu8ConfigDesc = descriptors->config_desc;
	l_gsInfo.get_str_desc_func = get_str_desc_func;
	l_gsInfo.gu8HidReportDesc = descriptors->hid_report_desc;
	l_gsInfo.gu32BOSDesc = descriptors->BOS_desc;
	l_gsInfo.gu32HidReportSize = descriptors->hid_report_size;
	l_gsInfo.gu32ConfigHidDescIdx = descriptors->config_hid_desc_index;
}


/**
 * usb_i9xxxx_ioctl()
 *
 * return:
 */
static uint8_t usb_i9xxxx_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_i9xxxx_cfg_t *cfg_hndl;

	if ((0 == init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_i9xxxx, adev);

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
	case IOCTL_USB_DEVICE_STALL_CONTROL_ENDPOINT:
		USBD_SetStall(0);
		break;
	case IOCTL_USB_DEVICE_SET_SUSPEND_CALLBACK:
		usb_device_suspend_callback_func = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_RESUME_CALLBACK:
		usb_device_resume_callback_func = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_RESET_CALLBACK:
		usb_device_reset_callback_func = aIoctl_param1;
		break;
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  usb_i9xxxx
#define MODULE_IOCTL_FUNCTION        usb_i9xxxx_ioctl
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
