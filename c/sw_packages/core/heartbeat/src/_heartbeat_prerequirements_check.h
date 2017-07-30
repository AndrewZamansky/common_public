/*
 * file : heartbeat_prerequirements_check.h
 *
 *
 */

#ifndef _heartbeat_prerequirements_check_H
#define _heartbeat_prerequirements_check_H

#include "global_prerequirements_check.h"

/***************************/
/******** test defines *****/

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/


/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/

#ifndef irq_unblock_all
enum{
	irq_unblock_all___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(irq_unblock_all)
};
#endif

#ifndef irq_block_all
enum{
	irq_block_all___not_declared_in_os_wrapper_h__as_required_by_async_tx_wrapper = sizeof(irq_block_all)
};
#endif

/***************************/




#endif /* */
