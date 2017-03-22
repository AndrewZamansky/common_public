#ifdef NEVER_DEFINED
 /* if preprosseing enabled should never enter here*/
 .err
 .print "!! add   \"-x assembler-with-cpp\"  as first argument to gcc to enable preprossesing  !!"
 .end
#endif

#include "project_config.h"

#ifndef CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER
 .err
 .print "!! CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER should be defined  !!"
 .end
#endif

#if (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 32)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	128
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 64)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	256
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 96)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	384
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 128)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	512
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 160)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	640
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 192)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	768
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 224)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	896
#elif  (CONFIG_DT_MAXIMAL_INTERRUPT_NUMBER < 256)
	#define BYTES_TO_KEEP_FOR_VECTOR_SPACE	1024
#endif

.cpu cortex-m3
.thumb
.section ._arm_vector_table,"ax"

.word 	Top_Of_Stacks
.word 	_secondary_rom_vector_table_startup_entry
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

// external interrupts start here
.space BYTES_TO_KEEP_FOR_VECTOR_SPACE,0     /*  not used for now */


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
