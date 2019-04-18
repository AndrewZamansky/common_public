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
#ifdef CONFIG_CORTEX_M4
	#include "core_cm4.h"  /* Cortex-M4 processor and core peripherals   */
#endif

/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

/**********   exported variables    **************/

/***********   loacal variables    **************/

uint32_t board_init_before_main_function()
{
	   /* FPU settings -------------------*/
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	    SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
	                   (3UL << 11*2)  );               /* set CP11 Full Access */

	#endif

	return 0;
}
