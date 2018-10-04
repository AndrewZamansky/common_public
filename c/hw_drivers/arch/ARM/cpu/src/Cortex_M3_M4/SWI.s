#ifdef NEVER_DEFINED
 /* if preprosseing enabled should never enter here*/
 .err
 .print "!! add   \"-x assembler-with-cpp\"  as first argument to gcc to enable preprossesing  !!"
 .end
#endif

#include "project_config.h"

.global do_software_interrupt_asm

.syntax unified
.thumb
.thumb_func
do_software_interrupt_asm:

	/* on cortex-m3 stack on svc is : r0,r1,r2,r3,r12,lr,pc,xpsr  */
	ldr r1,[sp, 6 * 4] /* load pc that was before SWI exception*/

	/* [pc - 2] is pointing on 'bkpt imm' in code that caused this exception.
	* bkpt imm = 0xDFXX; when XX is imm value. */
	LDRB r0, [r1, #-2] /*load imm to r0*/

	CMP r0,#0xAB
	BNE not_semihosting_swi /*  if  not OS then skip :  */
	LDR r4,=_semihosting_breakpoint  /* go to semihosting entry  */
	BX r4

not_semihosting_swi:


/*.ifndef CORTEX_M   in cortex-a  swi 0 used for OS  */
	CMP r0,#0x0
	BNE not_os_swi_handler 	/*  if  not OS then skip :  */

	ldr pc, _OS_Handle_text
	B end_of_swi_handler
/*.endif*/

not_os_swi_handler:
	/* r0 contains swi number and visible as first argument  */
	LDR pc, =do_app_software_interrupt /* call do_app_software_interrupt . */

end_of_swi_handler:
	MOVS pc, lr           /* Return if SWI handled. */


/********* next function is called after JLINK finishs semihosting *********/
.global return_from_semihosting
.thumb_func
return_from_semihosting:
	movs pc,lr

/********* next function is called externally *********/
.global bkpt_asm
bkpt_asm:
	push {r4,lr}
	bkpt #0xab
	pop {r4,pc}


_OS_Handle_text: .word OS_SWI_Handler
