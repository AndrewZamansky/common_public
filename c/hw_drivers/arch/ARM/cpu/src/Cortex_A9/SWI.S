

#ifdef NEVER_DEFINED
 /* if preprosseing enabled should never enter here*/
 .err
 .print "!! add   \"-x assembler-with-cpp\"  as first argument to gcc to enable preprossesing  !!"
 .end
#endif


.syntax unified

.global do_software_interrupt_asm
do_software_interrupt_asm:
	/* save r0-r12 , lr , spsr registers :  */
	PUSH {r0-r12,lr}
	MRS r3,spsr
	PUSH {r3}
	/* check if not semihosting :  */
	LDR r4,=0x123456
	LDR r0, [lr, #-4]   /* save swi number in r0 . not_os_swi_handler: will depend on r0 */
	BIC r0, #0xFF000000

	MOV r3,#0
	CMP r4,r0
	BEQ end_of_swi_handler 	/*  if  semihosting :  */

	CMP r0,#0x0
	BNE not_os_swi_handler 	/*  if  not OS then skip :  */


	LDMFD sp!, {r3}         /* Unstack SPSR... */
    MSR spsr_cxsf, r3         /* ...and restore it. */
    LDMFD sp!, {r0-r12, lr} /* Restore registers. */
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
    LDMFD sp!, {r3}         /* Unstack SPSR... */
    MSR spsr_cxsf, r3         /* ...and restore it. */
    LDMFD sp!, {r0-r12, lr} /* Restore registers. */
    BNE execute_semihosting
    MOVS pc, lr           /* Return if SWI handled. */

execute_semihosting:
	LDR r4,=_inner_vector_table_semihosting_entry  /* go to semihosting entry  */
    BX r4


.global return_from_semihosting
return_from_semihosting:
	movs pc,lr

/********* next function is called externally *********/
.global bkpt_asm
bkpt_asm:
	push {r4,lr}
	svc 0x123456
	pop {r4,pc}



#ifdef PREDEFINED_OS_SWI_HANDLER
	_OS_Handle_text: .word PREDEFINED_OS_SWI_HANDLER
#else
	_OS_Handle_text: .word OS_SWI_Handler
#endif

