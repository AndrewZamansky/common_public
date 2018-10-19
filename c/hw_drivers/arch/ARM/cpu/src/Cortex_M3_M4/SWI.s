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
	/* bit 2 in EXC_RETURN value (in LR) shows what
	 stack was used before exception happened*/
	tst lr, #4
	ite eq
	mrseq r0, MSP
	mrsne r0, PSP

	/* on cortex-m3 stack on svc is : r0,r1,r2,r3,r12,lr,pc,xpsr  */
	ldr r1,[r0, 6 * 4] /* load pc that was before SWI exception*/

	/* [pc - 2] is pointing on 'bkpt imm' in code that caused this exception.
	* bkpt imm = 0xDFXX; when XX is imm value. */
	LDRB r0, [r1, #-2] /*load imm to r0*/

	CMP r0,#0xAB
	BNE not_semihosting_swi /*  if  not OS then skip :  */
	LDR r4,=_semihosting_breakpoint  /* go to semihosting entry  */
	BX r4

not_semihosting_swi:

	/* r0 contains swi number and visible as first argument  */
	push {lr}
	ldr r1,=do_app_software_interrupt
	/*orr r0 ,r0, #0x01*/  /*remain in thumb*/
	blx r1
	pop {pc}


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
