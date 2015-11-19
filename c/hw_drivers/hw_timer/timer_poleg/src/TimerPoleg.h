/*
 * file : TimerPoleg.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _Timer_H
#define _Timer_H

#define CLK_BA				0xF0801000
#define CLKEN1				(CLK_BA+0x00)			// Clock Enable 1
#define CLKSEL				(CLK_BA+0x04)			// Clock select
#define CLKDIV1				(CLK_BA+0x08)			// Clock Divider Control Register 1

#define CLKSEL_TIMCKSEL_MASK (0x3 << 14)
#define CLKDIV1_TIMCKDIV_MASK (0x1f << 21)


#define TMR1_BA 			0xF0008000
#define TMR2_BA 			0xF0009000
#define TMR3_BA 			0xF000A000
#define TMR_P_TO_OFFSET(m)	((m/2)*0x20 + (m%2)*4)
#define TMR_M_TO_BA(n)		((n-1) *0x1000)

#define TICR_1(n)			((TMR1_BA+0x08 + TMR_P_TO_OFFSET(n)))
#define TISR_1				((TMR1_BA+0x18))			// Timer Interrupt Status for Timer 1
#define WTCR_1				((TMR1_BA+0x1C))			// Watchdog Timer Control for Timer 1
#define TCSR_1(n)			((TMR1_BA +TMR_P_TO_OFFSET(n)+0 ))
#define TDR_1(n)			((TMR1_BA +TMR_P_TO_OFFSET(n)+0x10))

#define TISR_2				((TMR2_BA+0x18))			// Timer Interrupt Status for Timer 2
#define WTCR_2				((TMR2_BA+0x1C))			// Watchdog Timer Control for Timer 2
#define TICR_2(n)			((TMR2_BA+0x0C +TMR_P_TO_OFFSET(n)))
#define TCSR_2(n)			((TMR2_BA +TMR_P_TO_OFFSET(n)+0 ))
#define TDR_2(n)			((TMR2_BA +TMR_P_TO_OFFSET(n)+0x10))

#define TISR_3				((TMR3_BA+0x18))			// Timer Interrupt Status for Timer 3
#define WTCR_3				((TMR3_BA+0x1C))			// Watchdog Timer Control for Timer 3
#define TICR_3(n)			((TMR3_BA+0x28+TMR_P_TO_OFFSET(n)))
#define TCSR_3(n)			((TMR3_BA +TMR_P_TO_OFFSET(n)+0 ))
#define TDR_3(n)			((TMR3_BA +TMR_P_TO_OFFSET(n)+0x10))

#define TCSR(n,m)			((TMR1_BA +TMR_P_TO_OFFSET(n)+0 + TMR_M_TO_BA(m)))
#define TISR(n)				((TMR1_BA+0x18)+(TMR_M_TO_BA(n)))		// Timer Interrupt Status
#define TICR(n,m)			((TMR1_BA +TMR_P_TO_OFFSET(n)+8 + TMR_M_TO_BA(m)))
#define WTCR(n)				((TMR1_BA+0x1C)+(TMR_M_TO_BA(n)))		// Watchdog Timer Control
#define TDR(n,m)			((TMR1_BA +TMR_P_TO_OFFSET(n)+0x10 + TMR_M_TO_BA(m)))

#define INT_START_NUM		32
#define TIMER_INT_0			(32 + INT_START_NUM)			// Timer module 0 Timer 0
#define TIMER_INT_1			(33 + INT_START_NUM)			// Timer module 0 Timer 1
#define TIMER_INT_2			(34 + INT_START_NUM)			// Timer module 0 Timer 2
#define TIMER_INT_3			(35 + INT_START_NUM)			// Timer module 0 Timer 3
#define TIMER_INT_4			(36 + INT_START_NUM)			// Timer module 0 Timer 4
#define TIMER_INT_5			(37 + INT_START_NUM)			// Timer module 1 Timer 0
#define TIMER_INT_6			(38 + INT_START_NUM)			// Timer module 1 Timer 1
#define TIMER_INT_7			(39 + INT_START_NUM)			// Timer module 1 Timer 2
#define TIMER_INT_8			(40 + INT_START_NUM)			// Timer module 1 Timer 3
#define TIMER_INT_9			(41 + INT_START_NUM)			// Timer module 1 Timer 4
#define TIMER_INT_10		(42 + INT_START_NUM)			// Timer module 2 Timer 0
#define TIMER_INT_11		(43 + INT_START_NUM)			// Timer module 2 Timer 1
#define TIMER_INT_12		(44 + INT_START_NUM)			// Timer module 2 Timer 2
#define TIMER_INT_13		(45 + INT_START_NUM)			// Timer module 2 Timer 3
#define TIMER_INT_14		(46 + INT_START_NUM)			// Timer module 2 Timer 4
#define WDF_INT_0			(47 + INT_START_NUM)			// Timer module 0 watchdog
#define WDF_INT_1			(48 + INT_START_NUM)			// Timer module 1 watchdog
#define WDF_INT_2			(49 + INT_START_NUM)			// Timer module 2 watchdog


#define TCSR_FREEZE_EN		(1<<31)
#define TCSR_CEN				(1<<30)
#define TCSR_IEN				(1<<29)
#define TCSR_MODE			(3<<27)
#define TCSR_MODE_ONE_SHOT	(0<<27)
#define TCSR_MODE_PERIODIC	(1<<27)
#define TCSR_CRST			(1<<26)

#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
