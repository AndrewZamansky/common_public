/*
 * file : freeRtos_xtensa_port.c
 *
 *
 */

//#include "irq_api.h"
#include <xtensa/tie/xt_interrupt.h>


/*
 * this __os_flag variable should be defined, otherwise _sbrk_r will not
 * check correctly the remained size of stack
 */
char __os_flag = 1;

void os_start_arch_related_components()
{
}


void os_sleep_arch_related_components()
{
	XT_WAITI(0);
}
