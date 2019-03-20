/*
;----------------------------------------------------------
; file : startup.s
;----------------------------------------------------------
*/


.syntax unified

.global do_startup
.global do_startup_with_debugger


.section .text.startup , "ax"

.extern smihosting_is_active

.global do_startup

.thumb_func
do_startup:
//	cpsid if /*disable interrupts and set mode before branch*/
	bl do_startup_common
 	ldr r0, =main
 	/*orr r0,r0,#0x01*/  /*remain in thumb*/
 	blx r0
	b 	.

.thumb_func
do_startup_with_debugger:

//	cpsid if /*disable interrupts and set mode before branch*/
	bl do_startup_common

	/* enable semihosting */
 	ldr	  r0, =smihosting_is_active
 	mov   r1,#0x01
 	str   r1,[r0]

 	ldr r0, =main
 	/*orr r0,r0,#0x01*/  /*remain in thumb*/
 	blx r0
	b 	.


.thumb_func
do_startup_common:
	cpsid i /*disable interrupts*/
	ldr   r0,=Top_Of_Stacks
	mov   sp, r0
	push {lr}

	mov r0,sp
 	ldr r1,=init_after_startup
 	/*orr r0 ,r0, #0x01*/  /*remain in thumb*/
 	blx r1
 	pop {pc}


.global __SETPRIMASK
__SETPRIMASK:
  cpsid i
  bx r14
.thumb_func
.global __RESETPRIMASK
__RESETPRIMASK:
  cpsie i
  bx r14
.thumb_func
.global __SETFAULTMASK
__SETFAULTMASK:
  cpsid f
  bx r14
.thumb_func
.global __RESETFAULTMASK
__RESETFAULTMASK:
  cpsie f
  bx r14
.thumb_func
.global __BASEPRICONFIG
__BASEPRICONFIG:
  msr basepri, r0
  bx r14
.thumb_func
.global __GetBASEPRI
__GetBASEPRI:
  mrs r0, basepri_max
  bx r14
.thumb_func
