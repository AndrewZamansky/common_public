#ifdef NEVER_DEFINED
 /* if preprosseing enabled should never enter here*/
 .err
 .print "!! add   \"-x assembler-with-cpp\"  as first argument to gcc to enable preprossesing  !!"
 .end
#endif

.global do_software_interrupt_asm

.syntax unified
.thumb
.thumb_func
do_software_interrupt_asm:


	ldr r1,[sp, 6*4] /* on cortex-m3 stack on svc is : r0,r1,r2,r3,r12,lr,pc,xpsr  */
	LDRB r0, [r1, #-2]   /* save swi number in r0 . not_os_swi_handler: will depend on r0 */
	LDR r4,=0xAB


	MOV r3,#0
	CMP r4,r0
	BEQ end_of_swi_handler 	/*  if  semihosting :  */

/*.ifndef CORTEX_M   in cortex-a  swi 0 used for OS  */
	CMP r0,#0x0
	BNE not_os_swi_handler 	/*  if  not OS then skip :  */


    ldr pc, _OS_Handle_text
	MOV r3,#1				/* r3 = 1 then semihosting will not be executed */
	B end_of_swi_handler
/*.endif*/

not_os_swi_handler:
    LDR pc, =do_app_software_interrupt /* call do_app_software_interrupt . */
     							      /* r0 contains swi number and visible as first argument  */
	MOV r3,#1		/* r3 = 1 then semihosting will not be executed */


end_of_swi_handler:
                            /* r3 = 1 if SWI handled */
    CMP r3, #1              /* Test if SWI has been handled. */
    BNE execute_semihosting
    MOVS pc, lr           /* Return if SWI handled. */

execute_semihosting:
	LDR r4,=_secondary_ram_vector_table_semihosting_entry  /* go to semihosting entry  */
    BX r4


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


