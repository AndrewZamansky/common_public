.cpu cortex-m3
.thumb
.section ._arm_vector_table,"ax"

.word 	Top_Of_Stacks
.word 	_inner_vector_table_startup_entry
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
.space 25,0     /*  not used for now */

.word	0 // on 32 (+15) position

.space 34,0     /*  not used for now */


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
