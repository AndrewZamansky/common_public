/* ****************************************
 *****************************************
	@file   board_init.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project.h"
#include "cpu_config.h"


/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

/**********   exported variables    **************/

/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/

uint32_t board_init_before_main_function()
{
    /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
                   (3UL << 11*2)  );               /* set CP11 Full Access */
#endif


	return 0;
}
