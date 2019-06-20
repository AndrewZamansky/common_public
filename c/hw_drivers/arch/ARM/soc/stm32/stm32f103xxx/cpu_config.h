/*
 * file : cpu_config.h
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

#ifndef __ASSEMBLER__
/*IRQn_Type can be defined in different files describing different socs .
 so it needs to be disabled in api functions */
#define IRQn_Type IRQn_Type_TMP
#include "stm32f10x.h"
#undef IRQn_Type

#endif  // for __ASSEMBLER__

#include "dt_interrupts_num.h"

#define __NVIC_PRIO_BITS	4

#define INTERRUPT_LOWEST_PRIORITY   ((1 << __NVIC_PRIO_BITS) - 1)
#define INTERRUPT_PRIORITY_ABOVE_LOWEST(level)  \
								(INTERRUPT_LOWEST_PRIORITY - level)

#endif /* */
