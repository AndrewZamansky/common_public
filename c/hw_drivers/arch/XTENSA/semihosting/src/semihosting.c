/* ****************************************
 *****************************************
	@file   <semihosting.c>

	@date   :
	@project  :
	@description :
***************************************** */

/* ------------------------ INCLUDES ---------------------------------------*/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "semihosting_api.h"

#include "dev_management_api.h"

#include "PRINTF_api.h"
#include <stdio.h>

#include "semihosting.h"
#include "_semihosting_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "semihosting_add_component.h"

/* ------------------------ defines ------------------------------*/

/* ------------------------typedefs ------------------------------*/

/* ---------------------------- External variables --------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------------ Exported variables -----------------------*/

/* ------------------------------ Local variables ---------------------------*/


/* ------------------------ Local Functions  -------------------------------*/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

/*
 * to enable semihosting run following set of gdb commands,
 * or add these commands to list of commands that are executed
 * automatically on connection
 *
 * break main
 * commands
 * set var global_enable_semihosting=1
 * end
 *
 */
volatile uint8_t global_enable_semihosting = 0;

/**
 * semihosting_pwrite()
 *
 * return:
 */
size_t semihosting_pwrite( struct dev_desc_t *adev,
		const uint8_t *apData , size_t aLength, size_t aOffset)
{
	struct semihosting_instance_t *handle;
	struct dev_desc_t *   callback_dev;

	if (0 == global_enable_semihosting) return 0;

	handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	callback_dev = handle->callback_dev;
	puts(apData);
	if(NULL != callback_dev)
	{
		/* !!! to avoid recursivity in
		 * semihosting transmited length should be '>=aLength' */
		DEV_CALLBACK_1_PARAMS(callback_dev , CALLBACK_TX_DONE,(void*)aLength);
	}
	return aLength;
}


/**
 * semihosting_ioctl()
 *
 * return:
 */
uint8_t semihosting_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct semihosting_instance_t *handle;

	handle = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
		case IOCTL_DEVICE_START :
			break;
		case IOCTL_SET_CALLBACK_DEV:
		case IOCTL_SET_ISR_CALLBACK_DEV:
			handle->callback_dev =(struct dev_desc_t *) aIoctl_param1;
			break;
		default :
			return 1;
	}
	return 0;
}
