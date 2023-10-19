/*
 * file : cpu_config.h
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

#define XT_SIMULATOR   0
#define XT_BOARD       1

#include "xtensa_config.h"

#ifndef __ASSEMBLER__

enum IRQn
{
	NPCP415_AUDIO_READY_IRQ              = 0,
	NPCP415_I2C_WRITE_FIFO_NOT_EMPTY_IRQ = 1,
	NPCP415_I2C_READ_FIFO_NOT_FULL_IRQ   = 2,
	NPCP415_TIMER_IRQ =                XCHAL_TIMER_INTERRUPT(0),
	TIMER_IRQ_NUM_OF_EXT_IRQs =          XCHAL_TIMER_INTERRUPT(0),


} ;

#endif // __ASSEMBLER__

#endif
