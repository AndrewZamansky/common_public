/*
 *
 * file :   pin_control_i9xxxx.c
 *
 *
 */



/********  includes *********************/
#include "pin_control_i9xxxx.h"

#include "pin_control_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#ifdef CONFIG_I94XXX
	#include "I94100.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
#endif


/********  defines *********************/

/********  types  *********************/


/********  externals *********************/


/********  local variables *********************/
#define PIN_FUNCTIONS_BASE_ADDR       (SYS_BASE + 0x30)
#define PIN_SCMITT_TRIGGER_BASE_ADDR  (GPIOA_BASE + 0x24)
#define PIN_GPIO_MODE_BASE_ADDR       (GPIOA_BASE + 0x00)
#define PIN_GPIO_PULL_UP_DOWN_BASE_ADDR       (GPIOA_BASE + 0x30)

#define MAX_NUM_OF_PORTS  4
static uint16_t pin_usage[MAX_NUM_OF_PORTS] = {0};

uint8_t pin_control_api_set_pin_function(uint32_t  new_pin_info)
{
	uint8_t port;
	uint8_t pin;
	uint8_t function;
	uint16_t curr_pin_usage;
	uint32_t pin_reg;
	uint8_t pos_in_reg;

	port = (new_pin_info >> 8) & 0xf;
	if (MAX_NUM_OF_PORTS <= port)
	{
		CRITICAL_ERROR("port to big");
	}

	pin = (new_pin_info >> 4) & 0xf;
	curr_pin_usage = pin_usage[port];
	if (curr_pin_usage & (1 << pin))
	{
		CRITICAL_ERROR("pin already in use");
	}
	pin_usage[port] |= (1 << pin);

	function = new_pin_info & 0xf;

	pin_reg = PIN_FUNCTIONS_BASE_ADDR + (port * 8) + ((pin / 8) * 4);
	pos_in_reg = (pin % 8) * 4;
	*((uint32_t *)pin_reg) &= ~(0xf << pos_in_reg);
	*((uint32_t *)pin_reg) |= function << pos_in_reg;

	return 0;
}

// can be send only from module that set pin function before
uint8_t pin_control_api_clear_pin_function(uint32_t  old_pin_info)
{
	uint8_t port;
	uint8_t pin;
	uint8_t declared_function;
	uint8_t curr_function;
	uint16_t curr_pin_usage;
	uint32_t pin_reg;
	uint8_t pos_in_reg;

	port = (old_pin_info >> 8) & 0xf;
	if (MAX_NUM_OF_PORTS <= port)
	{
		CRITICAL_ERROR("port to big");
	}

	pin = (old_pin_info >> 4) & 0xf;
	curr_pin_usage = pin_usage[port];
	if ( 0 == (curr_pin_usage & (1 << pin)))
	{
		CRITICAL_ERROR("pin was not in use");
	}

	declared_function = old_pin_info & 0xf;

	// set GPIO to input mode
	pin_reg = PIN_GPIO_MODE_BASE_ADDR + (port * 0x40);
	*((uint32_t *)pin_reg) &= ~(3 << (2 * pin));

	// set GPIO to high-z mode
	pin_reg = PIN_GPIO_PULL_UP_DOWN_BASE_ADDR + (port * 0x40);
	*((uint32_t *)pin_reg) &= ~(3 << (2 * pin));

	pin_reg = PIN_FUNCTIONS_BASE_ADDR + (port * 8) + ((pin / 8) * 4);
	pos_in_reg = (pin % 8) * 4;
	curr_function = (*((uint32_t *)pin_reg) >> pos_in_reg) & 0xf;
	if ( declared_function != curr_function)
	{
		CRITICAL_ERROR("pin was not set with declared function");
	}
	*((uint32_t *)pin_reg) &= ~(0xf << pos_in_reg); // set to default (GPIO)
	pin_usage[port] &= ~(1 << pin);

	return 0;
}



uint8_t pin_control_api_enable_input_schmitt_trigger(uint32_t  pin_info)
{
	uint8_t port;
	uint8_t pin;
	uint16_t curr_pin_usage;
	uint32_t pin_reg;

	port = (pin_info >> 8) & 0xf;
	if (MAX_NUM_OF_PORTS <= port)
	{
		CRITICAL_ERROR("port to big");
	}

	pin = (pin_info >> 4) & 0xf;
	curr_pin_usage = pin_usage[port];
	if ( 0 == (curr_pin_usage & (1 << pin)))
	{
		CRITICAL_ERROR("pin was not in use");
	}

	pin_reg = PIN_SCMITT_TRIGGER_BASE_ADDR + (port * 0x40);
	*((uint32_t *)pin_reg) |= (1 << pin);

	return 0;
}
