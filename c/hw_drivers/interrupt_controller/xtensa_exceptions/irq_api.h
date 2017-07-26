
#ifndef _irq_api_H_
#define _irq_api_H_


#include <xtensa/hal.h>

#include "xtensa_context.h"


/* Typedef for C-callable interrupt handler function */
typedef void (*xt_handler)(void *);

///* Typedef for C-callable exception handler function */
typedef void (*xt_exc_handler)(XtExcFrame *);


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified exception.

    n        - Exception number (type)
    f        - Handler function address, NULL to uninstall handler.

  The handler will be passed a pointer to the exception frame, which is created
  on the stack of the thread that caused the exception.

  If the handler returns, the thread context will be restored and the faulting
  instruction will be retried. Any values in the exception frame that are
  modified by the handler will be restored as part of the context. For details
  of the exception frame structure see xtensa_context.h.
-------------------------------------------------------------------------------
*/
extern xt_exc_handler xt_set_exception_handler(int n, xt_exc_handler f);


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified interrupt.

    n        - Interrupt number.
    f        - Handler function address, NULL to uninstall handler.
    arg      - Argument to be passed to handler.
-------------------------------------------------------------------------------
*/
extern xt_handler xt_set_interrupt_handler(int n, xt_handler f, void * arg);


/*
-------------------------------------------------------------------------------
  Call this function to enable the specified interrupts.

    mask     - Bit mask of interrupts to be enabled.
-------------------------------------------------------------------------------
*/
extern void xt_ints_on(unsigned int mask);


/*
-------------------------------------------------------------------------------
  Call this function to disable the specified interrupts.

    mask     - Bit mask of interrupts to be disabled.
-------------------------------------------------------------------------------
*/
extern void xt_ints_off(unsigned int mask);


/*
-------------------------------------------------------------------------------
  Call this function to set the specified (s/w) interrupt.
-------------------------------------------------------------------------------
*/
static inline void xt_set_intset(unsigned int arg)
{
    xthal_set_intset(arg);
}


/*
-------------------------------------------------------------------------------
  Call this function to clear the specified (s/w or edge-triggered)
  interrupt.
-------------------------------------------------------------------------------
*/
static inline void xt_set_intclear(unsigned int arg)
{
    xthal_set_intclear(arg);
}



#endif
