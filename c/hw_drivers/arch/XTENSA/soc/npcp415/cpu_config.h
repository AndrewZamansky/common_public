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
	AUDIO_READY_IRQ                    = 0,
	AUDIO_I2C_WRITE_FIFO_NOT_EMPTY_IRQ = 1,
	AUDIO_I2C_READ_FIFO_NOT_FULL_IRQ   = 2,
	TIMER_IRQ =                XCHAL_TIMER_INTERRUPT(0),
	NUM_OF_EXT_IRQs =          XCHAL_TIMER_INTERRUPT(0),


} ;

#endif // __ASSEMBLER__

#endif
