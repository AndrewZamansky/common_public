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

	actuall_palladium_freq = *((uint32_t*)PALLADIUM_ACTUAL_PLLX_ADDR);
	// assuming that if freq < 10MHz then we are on palladium
	if ((0 < actuall_palladium_freq) && ( 10000 > actuall_palladium_freq) )
	{
		running_on_palladium = 1;
	}

	return 0;
}
