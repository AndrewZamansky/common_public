/* ****************************************
 *****************************************
	@file   board_init.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "cpu_config.h"
#include "_project.h"


/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

/**********   exported variables    **************/

/***********   loacal variables    **************/

uint32_t board_init_before_main_function()
{
    SYS_UnlockReg();
	return 0;
}
