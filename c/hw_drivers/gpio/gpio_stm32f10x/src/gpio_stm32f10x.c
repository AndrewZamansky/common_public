/*
 *
 *   file  :  GPIO_STM32F10x.c
 *
 */



/***************   includes    *******************/
#include "stm32f10x_gpio.h"
#include "gpio_api.h"
#include "gpio_stm32f10x_api.h"
#include "gpio_stm32f10x.h"
#include "pin_control_api.h"
#include "dt_pin_control_stm32f10x.h"




/***************   defines    *******************/
#define GPIO_STM32F10x_MAX_NUM_OF_GPIOS 3



/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/

static GPIO_TypeDef  *ports[] = {GPIOA, GPIOB, GPIOC};


uint8_t gpio_stm32f10x_register_interrupt(struct dev_desc_t *adev,
										struct dev_desc_t *client_dev)
{
	CRITICAL_ERROR("TODO");

	return 0;
}


static void init_gpio(struct dev_desc_t *adev,
		struct gpio_stm32f10x_config_t *config_handle,
		struct gpio_stm32f10x_runtime_t *runtime_handle)
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
	uint8_t   port_num;
	uint32_t  pin_control;
	GPIO_TypeDef* GPIOx;
	GPIO_InitTypeDef GPIO_InitStructure;

	if (GPIO_STM32F10x_MAX_NUM_OF_GPIOS <= config_handle->port_num)
	{
		CRITICAL_ERROR("port to big");
	}

	pin_arr_size = config_handle->pin_arr_size;
	pin_arr = config_handle->pin_arr;
	pin_arr_idle_state = config_handle->pin_arr_idle_state;
	port_num = config_handle->port_num;
	GPIOx = ports[port_num];

	pin_mask = 0;
	pin_bitwise_idle_values0 = 0;
	pin_bitwise_idle_values1 = 0;

	// pin field will be filled in 'for' loop
	pin_control = FILL_PIN_DATA(port_num, 0, 4, DUMMY_AF_REMAP_REG, 0, 0, 0);
	for (i = 0; i < pin_arr_size; i++)
	{
		curr_pin = pin_arr[i];
		if (15 < curr_pin)
		{
			CRITICAL_ERROR("pin number should be less than 15");
		}
		pin_control |= (curr_pin << PIN_POS);
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

		//GPIO_DisableInt(GPIOx, curr_pin); TODO !!
	}

	GPIO_InitStructure.GPIO_Pin =  pin_mask;
	GPIO_InitStructure.GPIO_Mode = config_handle->mode ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOx, &GPIO_InitStructure );

	runtime_handle->pin_mask = pin_mask;
	runtime_handle->pin_bitwise_idle_values[0] = pin_bitwise_idle_values0;
	runtime_handle->pin_bitwise_idle_values[1] = pin_bitwise_idle_values1;

	if(NULL != config_handle->client_dev)
	{
		CRITICAL_ERROR("TODO: implement input GPIO");
		gpio_stm32f10x_register_interrupt(adev, NULL);
	}
	runtime_handle->init_done = 1;
}


static void  gpio_stm32f10x_read(struct gpio_stm32f10x_config_t *config_handle,
		struct gpio_stm32f10x_runtime_t *runtime_handle,
		struct gpio_api_read_t  *gpio_api_read)
{
	uint32_t curr_read_values;
	GPIO_TypeDef* GPIOx;

	gpio_api_read->values_arr_size = 2;
	gpio_api_read->pin_bitwise_idle_values =
			runtime_handle->pin_bitwise_idle_values;
	gpio_api_read->pin_bitwise_curr_values =
			runtime_handle->pin_bitwise_curr_values;

	GPIOx = ports[config_handle->port_num];
	curr_read_values = GPIO_ReadInputDataBit( GPIOx, runtime_handle->pin_mask );
	gpio_api_read->pin_bitwise_curr_values[0] = curr_read_values & 0xff;
	gpio_api_read->pin_bitwise_curr_values[1] = (curr_read_values >> 8) & 0xff;
}


static void  test_if_pins_not_in_idle(
		struct gpio_stm32f10x_config_t *config_handle,
		struct gpio_stm32f10x_runtime_t *runtime_handle, uint8_t  *not_idle)
{
	struct gpio_api_read_t  gpio_api_read;

	gpio_stm32f10x_read(config_handle, runtime_handle, &gpio_api_read);
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


static void stop_gpio(struct dev_desc_t *adev,
		struct gpio_stm32f10x_config_t *config_handle,
		struct gpio_stm32f10x_runtime_t *runtime_handle)
{
	uint8_t   pin_arr_size;
	uint8_t   *pin_arr;
	uint8_t   i;
	uint8_t   curr_pin;
	uint32_t  pin_control;
	uint8_t  port_num;

	pin_arr_size = config_handle->pin_arr_size;
	pin_arr = config_handle->pin_arr;
	port_num = config_handle->port_num;

	// pin field will be filled in 'for' loop
	pin_control = FILL_PIN_DATA(port_num, 0, 4, DUMMY_AF_REMAP_REG, 0, 0, 0);
	for (i = 0; i < pin_arr_size; i++)
	{
		curr_pin = pin_arr[i];
		if (15 < curr_pin)
		{
			CRITICAL_ERROR("pin number should be less than 15");
		}
		pin_control |= (curr_pin << 4);
		pin_control_api_clear_pin_function(pin_control);

		//GPIO_DisableInt(GPIOx, curr_pin); TODO !!
	}
	runtime_handle->pin_mask = 0;
	runtime_handle->init_done = 0;
}


/**
 * gpio_stm32f10x_ioctl()
 *
 * return:
 */
uint8_t gpio_stm32f10x_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct gpio_stm32f10x_config_t *config_handle;
	struct gpio_stm32f10x_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(gpio_stm32f10x, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(gpio_stm32f10x, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
#error "not implemented"
	case IOCTL_GPIO_STM32F10X_SET_PORT_PARAM :
		{
			handle->port_num = ((char*)aIoctl_param1)[0]-'a';
		}
		break;
	case IOCTL_GPIO_STM32F10X_SET_PIN_PARAM :
		{
			uint8_t pin_num;
			pin_num = atoi((char*)aIoctl_param1);
			handle->pin_num_mask = 1 << pin_num;
		}
		break;
	case IOCTL_GPIO_STM32F10X_SET_MODE_PARAM :
		if (0 == memcmp((uint8_t*) aIoctl_param1, "output" , sizeof("output") ))
		{
			handle->mode = GPIO_Mode_Out_PP;
		}
		else // other modes will be added later
		{
//				switch(pInstance->mode)
//				{
//					case GPIO_API_IN_FLOATING:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//						break;
//					case GPIO_API_IN_PULL_DOWN:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//						break;
//					case GPIO_API_IN_PULL_UP:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//						break;
//					case GPIO_API_OUT:
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//						break;
//					default :
//						return 1;
//				}
			return 1;
		}
		break;
#endif
	case IOCTL_ADD_ISR_CALLBACK_DEV:
		//Register interrupt and client device.
		gpio_stm32f10x_register_interrupt(adev, aIoctl_param1);
		break;

	case IOCTL_DEVICE_START :
		init_gpio(adev, config_handle, runtime_handle);
		break;

	case IOCTL_GPIO_PIN_SET :
		GPIO_SetBits(ports[config_handle->port_num], runtime_handle->pin_mask );
		break;

	case IOCTL_GPIO_PIN_CLEAR :
		GPIO_ResetBits(
				ports[config_handle->port_num], runtime_handle->pin_mask);
		break;

	case IOCTL_GPIO_PIN_READ :
		gpio_stm32f10x_read(config_handle, runtime_handle, aIoctl_param1);
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

#define MODULE_NAME                     gpio_stm32f10x
#define MODULE_IOCTL_FUNCTION           gpio_stm32f10x_ioctl

#define MODULE_CONFIGURABLE_PARAMS_ARRAY   {  \
		{IOCTL_GPIO_STM32F10X_SET_PORT_PARAM , IOCTL_VOID ,\
				GPIO_STM32F10X_API_PORT_STR, NOT_FOR_SAVE}, \
		{IOCTL_GPIO_STM32F10X_SET_PIN_PARAM , IOCTL_VOID , \
				GPIO_STM32F10X_API_PIN_STR, NOT_FOR_SAVE}, \
		{IOCTL_GPIO_STM32F10X_SET_MODE_PARAM , IOCTL_VOID ,\
				GPIO_STM32F10X_API_MODE_STR, NOT_FOR_SAVE}, \
	}

#include "add_module.h"
