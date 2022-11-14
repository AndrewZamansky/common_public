/*******************************************************************************
Copyright (c) 2006-2015 Cadence Design Systems Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

/******************************************************************************
  Xtensa-specific interrupt and exception functions for RTOS ports.
  Also see xtensa_intr_asm.S.
******************************************************************************/

#include "project_config.h"
#include <stdlib.h>
#include <stdint.h>

#include <xtensa/config/core.h>

#include "irq_api.h"
#include "xtensa_ops.h"


/*
  Default handler for unhandled interrupts.
*/
void xt_unhandled_interrupt(void * arg)
{
    exit(-1);
}


/*
  Default handler for unhandled exceptions.
*/
void xt_unhandled_exception(XtExcFrame *frame)
{
    exit(-1);
}

#ifdef CONFIG_FREE_RTOS
/* Handler table is in xtensa_intr_asm.S */

extern xt_exc_handler _xt_exception_table[XCHAL_EXCCAUSE_NUM];



/*
  This function registers a handler for the specified exception.
  The function returns the address of the previous handler.
  On error, it returns 0.
*/
xt_exc_handler xt_set_exception_handler(int n, xt_exc_handler f)
{
    xt_exc_handler old;

    if( n < 0 || n >= XCHAL_EXCCAUSE_NUM )
        return 0;       /* invalid exception number */

    old = _xt_exception_table[n];

    if (f) {
        _xt_exception_table[n] = f;
    }
    else {
        _xt_exception_table[n] = &xt_unhandled_exception;
    }

    return ((old == &xt_unhandled_exception) ? 0 : old);
}



/* Handler table is in xtensa_intr_asm.S */

struct xt_handler_table_entry {
    void * handler;
    void * arg;
};

extern struct xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS];





/*
  This function registers a handler for the specified interrupt. The "arg"
  parameter specifies the argument to be passed to the handler when it is
  invoked. The function returns the address of the previous handler.
  On error, it returns 0.
*/
xt_handler xt_set_interrupt_handler(int n, xt_handler f, void * arg)
{
    struct xt_handler_table_entry * entry;
    xt_handler               old;

    if( n < 0 || n >= XCHAL_NUM_INTERRUPTS )
        return 0;       /* invalid interrupt number */
    if( Xthal_intlevel[n] > XCHAL_EXCM_LEVEL )
        return 0;       /* priority level too high to safely handle in C */

    entry = _xt_interrupt_table + n;
    old   = entry->handler;

    if (f) {
        entry->handler = f;
        entry->arg     = arg;
    }
    else {
        entry->handler = &xt_unhandled_interrupt;
        entry->arg     = (void*)n;
    }

    return ((old == &xt_unhandled_interrupt) ? 0 : old);
}

#else

xt_handler xt_set_interrupt_handler(int n, xt_handler f, void * arg)
{
	_xtos_set_interrupt_handler_arg(n, f, arg);
}


#endif


static uint32_t last_intenable_val = 0;
static uint8_t all_interrupts_blocked = 1;
extern uint32_t _xt_intenable;

void	irq_block_all(void)
{
	all_interrupts_blocked = 1;
	last_intenable_val = _xt_intenable;
	xt_ints_off(0xffffffff);
}

void	irq_unblock_all(void)
{
	all_interrupts_blocked = 0;
	xt_ints_on(last_intenable_val);
}


void irq_enable_interrupt(uint16_t int_num)
{
	if (all_interrupts_blocked)
	{
		last_intenable_val |= 1 << int_num;
	}
	else
	{
		xt_ints_on( 1 << int_num );
	}
}
