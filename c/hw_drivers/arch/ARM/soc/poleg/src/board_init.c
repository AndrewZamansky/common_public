/* ****************************************
 *****************************************
	@file   board_init.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project.h"


/***************   defines    *******************/

 #define PALLADIUM_ACTUAL_PLLX_ADDR       (0xffff3fc4)


/***************   typedefs    *******************/



/**********   external variables    **************/

/**********   exported variables    **************/

int running_on_palladium = 0 ;

/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/

uint32_t board_init_before_main_function()
{
	uint32_t actuall_palladium_freq;

#if defined(CONFIG_CODE_LOCATION_INTERNAL_SRAM)
/* remap 0x100 bytes of 0xffff0000 to sram  */
	*((volatile uint32_t *)0xf0800074) |=  (1<<18);
#else
	*((volatile uint32_t *)0xf0800074) &=  (~(1<<18));
#endif

	actuall_palladium_freq = *((uint32_t*)PALLADIUM_ACTUAL_PLLX_ADDR);
	// assuming that if freq < 10MHz then we are on palladium
	if ((0 < actuall_palladium_freq) && ( 10000 > actuall_palladium_freq) )
	{
		running_on_palladium = 1;
	}


	return 0;
}
