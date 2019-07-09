/*
;----------------------------------------------------------
; file : startup.s
;----------------------------------------------------------
*/

.equ USR_MODE,             0x10
.equ FIQ_MODE,             0x11
.equ IRQ_MODE,             0x12
.equ SVC_MODE,             0x13
.equ ABT_MODE,             0x17
.equ UND_MODE,             0x1B
.equ SYS_MODE,             0x1F /* available on ARM Arch 4 and later*/

.equ I_BIT    ,  0x80
.equ F_BIT    ,  0x40


/*
;----------------------------------------------------------
; --- additional vector table
;----------------------------------------------------------
*/

.section .arm_additional_table.1.start,"ax"
.global _secondary_rom_vector_table_startup_entry

_additional_startup_entry:
    ldr pc, _startup_text

.section ._arm_additional_table.2.start_with_semihosting,"ax"
_additional_startup_with_debugger_entry:
    ldr pc, _startup_text_with_debugger

.extern return_from_semihosting

_startup_text: .word do_startup
_startup_text_with_debugger: .word do_startup_with_debugger

.section ._arm_additional_table.3.semihosting_breakpoint,"ax"
.section ._secondary_ram_vector_table,"ax"
.global _secondary_ram_vector_table_semihosting_entry

_secondary_ram_vector_table_semihosting_entry:
    ldr pc, _return_from_semihosting_text
_return_from_semihosting_text: .word return_from_semihosting

/*
;----------------------------------------------------------
; --- end of additional vector table
;----------------------------------------------------------
*/




.section .text.startup , "ax"

.global do_startup
do_startup:
	cpsid aif, SVC_MODE /*disable interrupts and set mode before branch!!*/
	bl do_startup_common
 	ldr r0, =main
 	blx r0
	b .

do_startup_with_debugger:
	cpsid aif, SVC_MODE /*disable interrupts and set mode before branch!!*/
	bl do_startup_common

	/* enable semihosting */
 	ldr	  r0, =smihosting_is_active
 	mov   r1,#0x01
 	str   r1,[r0]

 	ldr r0, =main
 	blx r0
	b .


do_startup_common:
/*
;----------------------------------------------------------
;Stack initialisation
;----------------------------------------------------------
*/



	/* Initialize stack pointers for all ARM modes */


	MSR CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
	LDR sp,=__irq_stack_top__ /* set the IRQ stack pointer */

	MSR CPSR_c,#(FIQ_MODE | I_BIT | F_BIT)
	LDR sp,=__fiq_stack_top__ /* set the FIQ stack pointer */

	MSR CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
	LDR sp,=__abt_stack_top__ /* set the ABT stack pointer */

	MSR CPSR_c,#(UND_MODE | I_BIT | F_BIT)
	LDR sp,=__und_stack_top__ /* set the UND stack pointer */

	MSR CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
	LDR sp,=__sys_stack_top__ /* set the C stack pointer */

	/*last state sfould be SVC !!! */
	MSR CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
	LDR sp,=__svc_stack_top__ /* set the SVC stack pointer */

	push {lr}
	bl cpu_init_cp15
	pop {lr}

	push {lr}

	mov r0,sp
 	ldr r1,=init_after_startup
 	blx r1
 	pop {pc}

label_error:
	B .

#define CONFIG_SYS_ICACHE_OFF
/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
cpu_init_cp15:
	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	mcr p15, 0, r0, c7, c10, 4	@ DSB
	mcr p15, 0, r0, c7, c5, 4	@ ISB

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
#if defined(CODE_LOCATION_INTERNAL_ROM_FOR_ASM) || defined(CODE_LOCATION_INTERNAL_SRAM_FOR_ASM)
	orr r0,#0x00002000 @ clear bits 13 (--V-)
#else
	bic r0,r0,#0x00002000 @ clear bits 13 (--V-)
#endif
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
#ifdef CONFIG_SYS_ICACHE_OFF
	bic	r0, r0, #0x00001000	@ clear bit 12 (I) I-cache
#else
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
#endif
	mcr	p15, 0, r0, c1, c0, 0

	/* Set vector address in CP15 VBAR register */
	mov	r0, #0x0
	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR


//#ifdef CONFIG_ARM_ERRATA_742230
	mrc	p15, 0, r0, c15, c0, 1	@ read diagnostic register
	orr	r0, r0, #1 << 4		@ set bit #4
	mcr	p15, 0, r0, c15, c0, 1	@ write diagnostic register
//#endif

//#ifdef CONFIG_ARM_ERRATA_743622
	mrc	p15, 0, r0, c15, c0, 1	@ read diagnostic register
	orr	r0, r0, #1 << 6		@ set bit #6
	mcr	p15, 0, r0, c15, c0, 1	@ write diagnostic register
//#endif

//#ifdef CONFIG_ARM_ERRATA_751472
	mrc	p15, 0, r0, c15, c0, 1	@ read diagnostic register
	orr	r0, r0, #1 << 11	@ set bit #11
	mcr	p15, 0, r0, c15, c0, 1	@ write diagnostic register
//#endif

	mov	pc, lr			@ back to my caller

