#ifdef NEVER_DEFINED
 /* if preprosseing enabled should never enter here*/
 .err
 .print "!! add   \"-x assembler-with-cpp\"  as first argument to gcc to enable preprossesing  !!"
 .end
#endif

#include "cpu_config.h"

#ifndef NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS
 .err
 .print "!! NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS should be defined  !!"
 .end
#endif

#if (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 32)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	128
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 64)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	256
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 96)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	384
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 128)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	512
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 160)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	640
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 192)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	768
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 224)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	896
#elif  (NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS < 256)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	1024
#endif

.cpu cortex-m3
.thumb
.section ._arm_vector_table,"ax"

.word 	Top_Of_Stacks
.word 	_startup
.word 	_NmiSR  /* non maskable interrupt from RCC */
.word 	_FaultIRQ
.word 	_dummy_trap     /* Populate if using MemManage (MPU)*/
.word 	_dummy_trap    /* Populate if using Bus fault*/
.word 	_dummy_trap    /* Populate if using Usage Fault*/
.space 	4,0     /*  reserved slots */
.word 	_dummy_SWI_IRQ
.word 	_dummy_trap 	/* 		Populate if using a debug monitor */
.word 	_dummy_trap 	/* 		 Reserved */
.word 	_dummy_trap 	/* 		 Populate if using pendable service request */
.word 	_dummy_trap 	/* 		 Populate if using SysTick */

// external interrupts start here, will be filled by hadlers in RAM vector table
.space BYTES_TO_KEEP_FOR_VECTOR_SPACE,0

/*end of vector_table */

.section ._arm_additional_table.1.start,"ax"
.thumb
.thumb_func
.global _startup
_startup:
	ldr r0, =do_startup
	blx r0
	nop /* alignment for 8 bytes */
	nop

/* next section is used to be a starting point if we want to use semihosting.
 * configure debugger to jump to this section on connection. look at map file
 * for exact adsress of this section to set for debugger jumping
 */
.section ._arm_additional_table.2.start_with_semihosting,"ax"
.thumb_func
.global _startup_with_semihosting
_startup_with_semihosting:
	ldr r0, =do_startup_with_debugger
	blx r0
	nop /* alignment for 8 bytes */
	nop

/* next section is used to set semihosting breakpoint for JLINK.
 * configure debugger semihosting breakpoint to this section on connection.
 * look at map file for exact adsress of this section
 */
.section ._arm_additional_table.3.semihosting_breakpoint,"ax"
.thumb_func
.global _semihosting_breakpoint
_semihosting_breakpoint:
	ldr r0, =return_from_semihosting
	blx r0
	nop  /* alignment for 8 bytes */
	nop


.section ._arm_additional_table.9999.end_of_additional_table,"ax"
.thumb_func
_end_of_additional_table:
	nop  /* alignment for 8 bytes */
	nop
	nop
	nop


/* default interrupts that are used before initialization of NIC */
.section .text._arm_vector_table,"ax"

_NmiSR:
    b  .

_FaultIRQ :
	b  .

_IlligalIRQ :
	b  .

_dummy_SWI_IRQ :
	b  .

_dummy_trap :
	b  .
