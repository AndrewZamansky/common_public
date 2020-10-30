/*
 *
 *   file  :  gpio_i94xxx.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "I94100.h"
#include "gpio.h"

#include "gpio_i94xxx_api.h"
#include "gpio_i94xxx.h"

#include "gpio_api.h"
#include "irq_api.h"
#include "pin_control_api.h"
#include "errors_api.h"
#include "os_wrapper.h"

#include <stdlib.h>



#if !defined(INTERRUPT_PRIORITY_FOR_GPIO)
	#error "INTERRUPT_PRIORITY_FOR_GPIO should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_GPIO)
	#error "priority should be lower then maximal priority for os syscalls"
#endif


static uint8_t  num_of_device_on_intA = 0;
static struct dev_desc_t **dev_on_int_listA = NULL;
static uint8_t  num_of_device_on_intB = 0;
static struct dev_desc_t **dev_on_int_listB = NULL;
static uint8_t  num_of_device_on_intC = 0;
static struct dev_desc_t **dev_on_int_listC = NULL;
static uint8_t  num_of_device_on_intD = 0;
static struct dev_desc_t **dev_on_int_listD = NULL;


static void notify_listeners(GPIO_T *GPIOx,
		struct dev_desc_t **dev_on_int_list,
		uint8_t  num_of_device_on_int)
{
	uint8_t i;
	uint32_t int_flag;

	int_flag = GPIOx->INTSRC;
	GPIO_CLR_INT_FLAG(GPIOx, 0xFFFF);// clear all
	for (i = 0; i < num_of_device_on_int; i++)
	{
		struct dev_desc_t * curr_dev;
		struct gpio_i94xxx_runtime_t *runtime_handle;
		struct dev_desc_t * callback_dev;
		struct gpio_i94xxx_config_t *config_handle;

		curr_dev = dev_on_int_list[i];
		runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(gpio_i94xxx, curr_dev);
		if ( 0 == (runtime_handle->pin_mask & int_flag))
		{
			continue;
		}

		config_handle = DEV_GET_CONFIG_DATA_POINTER(gpio_i94xxx, curr_dev);
		callback_dev = config_handle->client_dev;
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_INTERRUPT_ARRIVED);
		}
	}
}


IRQ_ATTR void gpio_i94xxx_isrA()
{
	notify_listeners((GPIO_T *)GPIOA_BASE,
			dev_on_int_listA, num_of_device_on_intA);
}


IRQ_ATTR void gpio_i94xxx_isrB()
{
	notify_listeners((GPIO_T *)GPIOB_BASE,
			dev_on_int_listB, num_of_device_on_intB);
}


IRQ_ATTR void gpio_i94xxx_isrC()
{
	notify_listeners((GPIO_T *)GPIOC_BASE,
			dev_on_int_listC, num_of_device_on_intC);
}


IRQ_ATTR void gpio_i94xxx_isrD()
{
	notify_listeners((GPIO_T *)GPIOD_BASE,
			dev_on_int_listD, num_of_device_on_intD);
}


uint8_t gpio_i94xxx_register_interrupt(struct dev_desc_t *adev,
										struct dev_desc_t *client_dev)
{
	uint8_t  *p_num_of_callback_devices;
	uint8_t  num_of_callback_devices;
	struct dev_desc_t ***callback_devices_list;
	struct dev_desc_t **new_callback_devices_list;
	struct gpio_i94xxx_config_t *config_handle;
	GPIO_T* GPIOx;
	IRQn_Type IRQn;
	uint8_t i;
	uint8_t   pin_arr_size;
	uint8_t   *pin_arr;
	isr_t  isr;

	if(NULL == adev)
	{
		CRITICAL_ERROR("Device not specified");
	}

	config_handle = DEV_GET_CONFIG_DATA_POINTER(gpio_i94xxx, adev);
	pin_arr_size = config_handle->pin_arr_size;
	pin_arr = config_handle->pin_arr;
	GPIOx = (GPIO_T*)config_handle->port_num;

	if(NULL != client_dev)
	{
		config_handle->client_dev = client_dev;
	}

	//Set the required IRQn so that button will trigger interrupt
	if(PA == GPIOx)
	{
		IRQn = GPA_IRQn;
		p_num_of_callback_devices = &num_of_device_on_intA;
		callback_devices_list = &dev_on_int_listA;
		isr = gpio_i94xxx_isrA;
	}
	else if(PB == GPIOx)
	{
		IRQn = GPB_IRQn;
		p_num_of_callback_devices = &num_of_device_on_intB;
		callback_devices_list = &dev_on_int_listB;
		isr = gpio_i94xxx_isrB;
	}
	else if(PC == GPIOx)
	{
		IRQn = GPC_IRQn;
		p_num_of_callback_devices = &num_of_device_on_intC;
		callback_devices_list = &dev_on_int_listC;
		isr = gpio_i94xxx_isrC;
	}
	else if(PD == GPIOx)
	{
		IRQn = GPD_IRQn;
		p_num_of_callback_devices = &num_of_device_on_intD;
		callback_devices_list = &dev_on_int_listD;
		isr = gpio_i94xxx_isrD;
	}
	else
	{
		CRITICAL_ERROR("GPIO Port not set");
	}

	num_of_callback_devices = *p_num_of_callback_devices;
	if (255 == num_of_callback_devices)
	{
		CRITICAL_ERROR("cannot register more than 255 devices");
	}
	num_of_callback_devices++;
	new_callback_devices_list = (struct dev_desc_t **)os_safe_realloc(
			*callback_devices_list,
			num_of_callback_devices * sizeof(struct dev_desc_t *));
	errors_api_check_if_malloc_succeed(new_callback_devices_list);

	new_callback_devices_list[num_of_callback_devices - 1] = adev;
	*p_num_of_callback_devices = num_of_callback_devices;
	*callback_devices_list = new_callback_devices_list;

	irq_register_interrupt(IRQn, isr);
	irq_set_priority(IRQn , INTERRUPT_PRIORITY_FOR_GPIO );
	irq_enable_interrupt(IRQn);

	//Current Button Manager needs to be set for both edges.
	for (i = 0; i < pin_arr_size; i++)
	{
		GPIO_EnableInt(GPIOx, pin_arr[i], GPIO_INT_BOTH_EDGE);
	}

	return 0;
}


static void init_gpio(struct dev_desc_t *adev,
		struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle)
{
	uint8_t   pin_arr_size;
	uint8_t   *pin_arr;
	uint8_t   *pin_arr_idle_state;
	uint8_t   i;
	uint8_t   curr_pin;
	uint16_t   pin_mask;
	uint8_t   curr_idle_state;
	uint8_t   pin_bitwise_idle_values0;
	uint8_t   pin_bitwise_idle_values1;
	GPIO_T*   GPIOx;
	uint32_t  pin_control;

	pin_arr_size = config_handle->pin_arr_size;
	pin_arr = config_handle->pin_arr;
	pin_arr_idle_state = config_handle->pin_arr_idle_state;
	GPIOx = (GPIO_T*)config_handle->port_num;

	pin_mask = 0;
	pin_bitwise_idle_values0 = 0;
	pin_bitwise_idle_values1 = 0;
	pin_control = ((config_handle->port_num - GPIOA_BASE) / 0x40) << 8;
	for (i = 0; i < pin_arr_size; i++)
	{
		curr_pin = pin_arr[i];
		if (15 < curr_pin)
		{
			CRITICAL_ERROR("pin number should be less than 15");
		}
		pin_control |= (curr_pin << 4);
		// last 4 bits of pin_control are equal to 0, to select GPIO function
		pin_control_api_set_pin_function(pin_control);

		pin_mask |= (1 << curr_pin);

		curr_idle_state = pin_arr_idle_state[i];
		if (1 < curr_idle_state)
		{
			CRITICAL_ERROR("idle state should be 0 or 1");
		}

		if (curr_pin < 8)
		{
			pin_bitwise_idle_values0 |= (curr_idle_state << curr_pin);
		}
		else
		{
			pin_bitwise_idle_values1 |= (curr_idle_state << (curr_pin - 8));
		}

		GPIO_DisableInt(GPIOx, curr_pin);
	}
	GPIO_SetMode(GPIOx, pin_mask, config_handle->mode);
	GPIOx->SMTEN |= pin_mask;
	runtime_handle->pin_mask = pin_mask;
	runtime_handle->pin_bitwise_idle_values[0] = pin_bitwise_idle_values0;
	runtime_handle->pin_bitwise_idle_values[1] = pin_bitwise_idle_values1;

	if(NULL != config_handle->client_dev)
	{
		gpio_i94xxx_register_interrupt(adev, NULL);
	}
	runtime_handle->init_done = 1;
}


static void  gpio_i94xxx_read(struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle,
		struct gpio_api_read_t  *gpio_api_read)
{
	uint32_t curr_read_values;
	uint32_t pin_mask;
	GPIO_T* GPIOx;

	gpio_api_read->values_arr_size = 2;
	gpio_api_read->pin_bitwise_idle_values =
			runtime_handle->pin_bitwise_idle_values;
	gpio_api_read->pin_bitwise_curr_values =
			runtime_handle->pin_bitwise_curr_values;

	pin_mask = runtime_handle->pin_mask;
	GPIOx = (GPIO_T*)config_handle->port_num;
	curr_read_values = GPIOx->PIN;
	curr_read_values &= pin_mask;
	gpio_api_read->pin_bitwise_curr_values[0] = curr_read_values & 0xff;
	gpio_api_read->pin_bitwise_curr_values[1] = (curr_read_values >> 8) & 0xff;
}


static void  test_if_pins_not_in_idle(
		struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle, uint8_t  *not_idle)
{
	struct gpio_api_read_t  gpio_api_read;

	gpio_i94xxx_read(config_handle, runtime_handle, &gpio_api_read);
	// use XOR to check if values are different from idles
	if ((gpio_api_read.pin_bitwise_idle_values[0] ^
			gpio_api_read.pin_bitwise_curr_values[0]) ||
		(gpio_api_read.pin_bitwise_idle_values[1] ^
			gpio_api_read.pin_bitwise_curr_values[1]))
	{
		*not_idle = 1;
	}
	else
	{
		*not_idle = 0;
	}
}


static void  gpio_i94xxx_set(struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle)
{
	uint32_t pin_mask;
	GPIO_T* GPIOx;
	uint32_t volatile * pDOUT;

	pin_mask = runtime_handle->pin_mask;
	GPIOx = (GPIO_T*)config_handle->port_num;
	pDOUT = &GPIOx->DOUT;
	*pDOUT |= pin_mask;
}


static void  gpio_i94xxx_clear(struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle)
{
	uint32_t pin_mask;
	GPIO_T* GPIOx;
	uint32_t volatile * pDOUT;

	pin_mask = runtime_handle->pin_mask;
	GPIOx = (GPIO_T*)config_handle->port_num;
	pDOUT = &GPIOx->DOUT;
	*pDOUT &= (~pin_mask);
}


static void stop_gpio(struct dev_desc_t *adev,
		struct gpio_i94xxx_config_t *config_handle,
		struct gpio_i94xxx_runtime_t *runtime_handle)
{
	uint8_t   pin_arr_size;
	uint8_t   *pin_arr;
	uint8_t   i;
	uint8_t   curr_pin;
	GPIO_T*   GPIOx;
	uint32_t  pin_control;

	pin_arr_size = config_handle->pin_arr_size;
	pin_arr = config_handle->pin_arr;
	GPIOx = (GPIO_T*)config_handle->port_num;

	pin_control = ((config_handle->port_num - GPIOA_BASE) / 0x40) << 8;
	for (i = 0; i < pin_arr_size; i++)
	{
		curr_pin = pin_arr[i];
		if (15 < curr_pin)
		{
			CRITICAL_ERROR("pin number should be less than 15");
		}
		pin_control |= (curr_pin << 4);
		pin_control_api_clear_pin_function(pin_control);

		GPIO_DisableInt(GPIOx, curr_pin);
	}
	runtime_handle->pin_mask = 0;
	runtime_handle->init_done = 0;
}

/**
 * gpio_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t gpio_i94xxx_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct gpio_i94xxx_config_t *config_handle;
	struct gpio_i94xxx_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(gpio_i94xxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(gpio_i94xxx, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
#error "not implemented"
	case IOCTL_GPIO_I94XXX_SET_PORT_PARAM :
		config_handle->port_num =  *(uint32_t*)aIoctl_param1 ;
		break;
	case IOCTL_GPIO_I94XXX_SET_SINGLE_PIN_NUMBER_PARAM :
		{
			uint8_t pin_num;
			pin_num = *(uint8_t*)aIoctl_param1 ;
			config_handle->pin_num_mask = 1 << pin_num;
		}
		break;
	case IOCTL_GPIO_I94XXX_SET_MODE_PARAM :
		config_handle->mode = *(uint32_t*)aIoctl_param1 ;
		break;
#endif
	case IOCTL_ADD_ISR_CALLBACK_DEV:
		//Register interrupt and client device.
		gpio_i94xxx_register_interrupt(adev, aIoctl_param1);
		break;

	case IOCTL_DEVICE_START :
		init_gpio(adev, config_handle, runtime_handle);
		break;

	case IOCTL_GPIO_PIN_SET :
		gpio_i94xxx_set(config_handle, runtime_handle);
		break;

	case IOCTL_GPIO_PIN_CLEAR :
		gpio_i94xxx_clear(config_handle, runtime_handle);
		break;

	case IOCTL_GPIO_PIN_READ :
		gpio_i94xxx_read(config_handle, runtime_handle, aIoctl_param1);
		break;

	case IOCTL_GPIO_TEST_IF_PINS_NOT_IN_IDLE :
		test_if_pins_not_in_idle(config_handle, runtime_handle, aIoctl_param1);
		break;

	case IOCTL_DEVICE_STOP :
		stop_gpio(adev, config_handle, runtime_handle);
		break;

	default :
		return 1;
	}
	return 0;
}


#define MODULE_NAME               gpio_i94xxx
#define MODULE_IOCTL_FUNCTION     gpio_i94xxx_ioctl

/* add this #ifdef block to avoid unused variable warning*/
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	static mapped_set_to_size_param_t  gpio_i94xxx_port_mapped_set[] = {
			{"a" , GPIO_I94XXX_API_PORT_A},
			{"b" , GPIO_I94XXX_API_PORT_B},
			{"c" , GPIO_I94XXX_API_PORT_C},
			{"d" , GPIO_I94XXX_API_PORT_D}
		};

	static mapped_set_to_size_param_t  gpio_i94xxx_mode_mapped_set[] = {
			{"out" , GPIO_I94XXX_API_MODE_OUT_PP},
			{"in"  , GPIO_I94XXX_API_MODE_IN}
		};
#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY                          \
	{                                                             \
		{                                                         \
			GPIO_I94XXX_API_PORT_STR,                             \
			IOCTL_GPIO_I94XXX_SET_PORT_PARAM,                     \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE,                    \
			MAPPED_SET_TO_SIZE_PARAM(gpio_i94xxx_port_mapped_set) \
		},                                                        \
		{                                                         \
			GPIO_I94XXX_API_SINGLE_PIN_STR,                       \
			IOCTL_GPIO_I94XXX_SET_SINGLE_PIN_NUMBER_PARAM,        \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_UINT8,                                 \
			MAPPED_SET_DUMMY_PARAM()                              \
		},                                                        \
		{                                                         \
			GPIO_I94XXX_API_MODE_STR,                             \
			IOCTL_GPIO_I94XXX_SET_MODE_PARAM,                     \
			IOCTL_VOID,                                           \
			DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE,                    \
			MAPPED_SET_TO_SIZE_PARAM(gpio_i94xxx_mode_mapped_set) \
		}                                                         \
	}

#include "add_module.h"
