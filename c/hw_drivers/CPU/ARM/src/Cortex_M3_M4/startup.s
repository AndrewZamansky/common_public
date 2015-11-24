/*
;----------------------------------------------------------
; file : startup.s
;----------------------------------------------------------
*/
.syntax unified

.section ._secondary_rom_vector_table,"ax"
.global _secondary_rom_vector_table
.global _secondary_rom_vector_table_startup_entry


.thumb
.thumb_func
_secondary_rom_vector_table_startup_entry:
	ldr r0, =do_startup
 	blx r0
 	nop /* alignment for 8 bytes */
 	nop
.thumb_func
_secondary_rom_vector_table_startup_with_debugger_entry:
	ldr r0, =do_startup_with_debugger
 	blx r0
 	nop /* alignment for 8 bytes */
 	nop
/*end of _secondary_rom_vector_table */

.section ._secondary_ram_vector_table,"ax"
.global _secondary_ram_vector_table_semihosting_entry

.extern return_from_semihosting

.thumb
.thumb_func
_secondary_ram_vector_table_semihosting_entry:
	ldr r0, =return_from_semihosting
 	blx r0
 	nop  /* alignment for 8 bytes */
 	nop
/*end of _secondary_ram_vector_table */
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
//	push {lr}
	STMDB sp!,{lr}
//	push {r0}
//	pop {r1}
 	ldr r0,=low_level_init
 	/*orr r0 ,r0, #0x01*/  /*remain in thumb*/
 	blx r0
 	LDMIA sp!,{lr}
// 	pop {pc}



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
