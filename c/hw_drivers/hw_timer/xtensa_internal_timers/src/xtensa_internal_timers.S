/*
//-----------------------------------------------------------------------------
// Copyright (c) 2003-2015 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------
*/

#ifdef __ASSEMBLER__
#include    <xtensa/coreasm.h>
#endif

#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>

#ifdef __ASSEMBLER__
#ifdef __XTENSA_CALL0_ABI__
  /* Call0 */
  #define ENTRY(sz)     entry1  sz
    .macro  entry1 size=0x10
    addi    sp, sp, -\size
    s32i    a0, sp, 0
    .endm
  #define ENTRY0
  #define RET(sz)       ret1    sz
    .macro  ret1 size=0x10
    l32i    a0, sp, 0
    addi    sp, sp, \size
    ret
    .endm
  #define RET0          ret
#else
  /* Windowed */
  #define ENTRY(sz)     entry   sp, sz
  #define ENTRY0        entry   sp, 0x10
  #define RET(sz)       retw
  #define RET0          retw
#endif
#endif

/*
****************************************************************************
*                void _xtensa_internal_timer_timer_asm_rearm(void)
*
*
*******************************************************************************
*/
    .globl  _xtensa_internal_timer_timer_asm_rearm
    .type   _xtensa_internal_timer_timer_asm_rearm,@function
    .align  4
_xtensa_internal_timer_timer_asm_rearm:

    /*
    Xtensa timers work by comparing a cycle counter with a preset value.
    Once the match occurs an interrupt is generated, and the handler has to
    set a new cycle count into the comparator. To avoid clock drift due to
    interrupt latency, the new cycle count is computed from the old,
    not the time the interrupt was serviced. However if a timer interrupt
    is ever serviced more than one tick late, it is necessary to process
    multiple ticks until the new cycle count is in the future, otherwise
    the next timer interrupt would not occur until after the cycle
    counter had wrapped (2^32 cycles later).

    do {
        ticks++;
        old_ccompare = read_ccompare_i();
        write_ccompare_i( old_ccompare + divisor );
        service one tick;
        diff = read_ccount() - old_ccompare;
    } while ( diff > divisor );
    */

    ENTRY(16)


    /* Update the timer comparator for the next tick. */
    movi    a3, _xtensa_internal_timers_tick_divisor
    l32i    a2, a3, 0                   /* a2 = comparator increment         */
    rsr     a3, (CCOMPARE + 0 )             /* a3 = old comparator value     */
    add     a4, a3, a2                  /* a4 = new comparator value         */
    wsr     a4, (CCOMPARE + 0 )			/* update comp. and clear interrupt*/
    esync

    RET(16)

/*
****************************************************************************
*                void _xtensa_internal_timer_timer_asm_init(void)
*
* Initialize timer and timer interrrupt
* handler (_xt_tick_divisor_init() has already been been called).
* Callable from C (obeys ABI conventions on entry).
*
*******************************************************************************
*/
    .globl  _xtensa_internal_timer_timer_asm_init
    .type   _xtensa_internal_timer_timer_asm_init,@function
    .align  4
_xtensa_internal_timer_timer_asm_init:

    ENTRY(16)

    /* Set up the periodic tick timer (assume enough time to complete init). */
    movi    a2, _xtensa_internal_timers_tick_divisor
    l32i    a3, a2, 0
    rsr     a2, CCOUNT              /* current cycle count */
    add     a2, a2, a3              /* time of first timer interrupt */
    wsr     a2, (CCOMPARE + 0 )         /* set the comparator */

    RET(16)
