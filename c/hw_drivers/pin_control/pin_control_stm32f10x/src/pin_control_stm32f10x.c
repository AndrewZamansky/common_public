/*
 *
 * file :   pin_control_stm32f10x.c
 *
 *
 */



/********  includes *********************/
#include "pin_control_stm32f10x.h"

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "pin_control_api.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "dt_pin_control_stm32f10x.h"

/********  defines *********************/
#define AF_BASE_ADDR    0x40010000
/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/
#define MAX_NUM_OF_PORTS  3
static uint16_t pin_usage[MAX_NUM_OF_PORTS] = {0};
static GPIO_TypeDef  *ports[] = {GPIOA, GPIOB, GPIOC};
uint8_t port_usage_references[MAX_NUM_OF_PORTS] = {0};
uint8_t alternate_function_references = 0;

uint8_t pin_control_api_set_pin_function(uint32_t  new_pin_info)
{
	uint8_t port;
	uint8_t pin;
	uint8_t function;
	uint16_t curr_pin_usage;
	volatile uint32_t *pin_reg;
	uint8_t pos_in_reg;
	GPIO_TypeDef* GPIOx;
	uint32_t af_reg_offset;
	uint32_t *af_reg;

	port = (new_pin_info >> PORT_POS) & 0x3;
	if (MAX_NUM_OF_PORTS <= port)
	{
		CRITICAL_ERROR("port to big");
	}

	port_usage_references[port]++;
	if (1 == port_usage_references[port])
	{
		RCC_APB2PeriphClockCmd(1 << (port + 2), ENABLE);
	}
	pin = (new_pin_info >> PIN_POS) & 0xf;
	curr_pin_usage = pin_usage[port];
	if (curr_pin_usage & (1 << pin))
	{
		CRITICAL_ERROR("pin already in use");
	}
	pin_usage[port] |= (1 << pin);

	GPIOx = ports[port];
	if (pin < 8)
	{
		pin_reg = &GPIOx->CRL;
	}
	else
	{
		pin_reg = &GPIOx->CRH;
	}

	pos_in_reg = (pin % 8) * 4;
	*pin_reg &= ~(0xf << pos_in_reg);
	function = new_pin_info & 0xf;
	*pin_reg |= function << pos_in_reg;
	if ((function & 0x8) && (0 != (function & 0x3)))
	{
		alternate_function_references++;
		if (1 == alternate_function_references)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		}
	}
	af_reg_offset = (new_pin_info >> AF_MAP_REG_POS) & 0x1f;
	if ( DUMMY_AF_REMAP_REG != af_reg_offset)
	{
		uint16_t in_af_reg_field_pos;
		uint16_t in_af_reg_field_size;

		af_reg = (uint32_t*)(AF_BASE_ADDR + af_reg_offset);
		in_af_reg_field_pos = (new_pin_info >> AF_MAP_FIELD_PLACE_POS) & 0x1f;
		in_af_reg_field_size = (new_pin_info >> AF_MAP_FIELD_SIZE_POS) & 0xf;
		//clean field :
		*af_reg &= ~(((1 << in_af_reg_field_size) - 1) << in_af_reg_field_pos);
		//set field :
		*af_reg |=
			((new_pin_info >> AF_MAP_FUNC_POS) & 0xf) << in_af_reg_field_pos;
	}
	return 0;
}

// can be send only from module that set pin function before
uint8_t pin_control_api_clear_pin_function(uint32_t  old_pin_info)
{
	uint8_t port;
	uint8_t pin;
	uint8_t declared_mapping;
	uint8_t curr_mapping;
	uint8_t function;
	uint16_t curr_pin_usage;
	volatile uint32_t *pin_reg;
	uint8_t pos_in_reg;
	GPIO_TypeDef* GPIOx;
	uint32_t af_reg_offset;
	uint32_t *af_reg;

	port = (old_pin_info >> PORT_POS) & 0x3;
	if (MAX_NUM_OF_PORTS <= port)
	{
		CRITICAL_ERROR("port to big");
	}

	pin = (old_pin_info >> PIN_POS) & 0xf;
	curr_pin_usage = pin_usage[port];
	if ( 0 == (curr_pin_usage & (1 << pin)))
	{
		CRITICAL_ERROR("pin was not in use");
	}

	GPIOx = ports[port];
	if (pin < 8)
	{
		pin_reg = &GPIOx->CRL;
	}
	else
	{
		pin_reg = &GPIOx->CRH;
	}

	// set GPIO to input mode
	pos_in_reg = (pin % 8) * 4;
	*pin_reg &= ~(0xf << pos_in_reg); // clear field
	/* not checking if current pin function equal to declared because
	 * some bit of this field are altered by other modules (like GPIO)
	 */
	*pin_reg |= ~(0x4 << pos_in_reg); // set to input

	pin_usage[port] &= ~(1 << pin);

	af_reg_offset = (old_pin_info >> AF_MAP_REG_POS) & 0x1f;
	if ( DUMMY_AF_REMAP_REG != af_reg_offset)
	{
		uint16_t in_af_reg_field_pos;
		uint16_t in_af_reg_field_size;

		af_reg = (uint32_t*)(AF_BASE_ADDR + af_reg_offset);
		in_af_reg_field_pos = (old_pin_info >> AF_MAP_FIELD_PLACE_POS) & 0x1f;
		in_af_reg_field_size = (old_pin_info >> AF_MAP_FIELD_SIZE_POS) & 0xf;
		curr_mapping = (*af_reg >> in_af_reg_field_pos) &
									((1 << in_af_reg_field_size) - 1);
		declared_mapping = (old_pin_info >> AF_MAP_FUNC_POS) & 0xf;
		if ( declared_mapping != curr_mapping)
		{
			CRITICAL_ERROR("pin was not set with declared mapping");
		}
		//clean field (also the default value is 0):
		*af_reg &= ~(((1 << in_af_reg_field_size) - 1) << in_af_reg_field_pos);
	}

	function = old_pin_info & 0xf;
	if ((function & 0x8) && (0 != (function & 0x3)))
	{
		if (0 == alternate_function_references)
		{
			CRITICAL_ERROR("more request to stop AF then to start AF");
		}
		alternate_function_references--;
		if (0 == alternate_function_references)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, DISABLE);
		}
	}

	port_usage_references[port]--;
	if (0 == port_usage_references[port])
	{
		RCC_APB2PeriphClockCmd(1 << (port + 2), DISABLE);
	}


	return 0;
}


uint8_t pin_control_api_enable_input_schmitt_trigger(uint32_t  pin_info)
{
	CRITICAL_ERROR("TODO");
	return 0;
}
