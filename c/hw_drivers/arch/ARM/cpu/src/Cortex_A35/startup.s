// ------------------------------------------------------------
// Armv8-A MPCore EL3 AArch64 Startup Code
//
// Basic Vectors, MMU, caches and GICv2 initialization in EL3 AArch64
//
// Exits in EL3 AArch64
//
// Copyright (c) 2014-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------

#include "v8_mmu.h"
#include "v8_system.h"


    .section .text.StartUp, "ax"
    .balign 4


    .global el1_vectors
    .global el2_vectors
    .global el3_vectors

    .global InvalidateUDCaches

    .global SetPrivateIntSecurityBlock
    .global SetSPISecurityAll
    .global SetPrivateIntPriority

    .global SetIrqGroup
    .global SetBlockGroup
    .global SetSPIGroup
    .global EnableIRQ
    .global TestIRQ
    .global EnableGICD
    .global EnableGICC
    .global SetIRQPriority
    .global SetPriorityMask
    .global ClearSGI
    .global EnableSecureFIQ

    .global main_core0
    .global main_core1
    .global main_core2
    .global main_core3

 	.global __ttb0_l1
    .global __el3_stack_top__

// ------------------------------------------------------------

    .global start64
    .type start64, "function"
start64:
	bl start_common
    bl main
    b .

    .global start64_semihosting
    .type start64_semihosting, "function"
start64_semihosting:
	bl start_common
 	ldr	  x0, =smihosting_is_active
 	mov   w1,#0x01
 	str   w1,[x0]
    bl main
    b .

    .global start64_semihosting_palladium
    .type start64_semihosting_palladium, "function"
start64_semihosting_palladium:
	bl start_common
 	ldr	  x0, =smihosting_is_active
 	mov   w1,#0x01
 	str   w1,[x0]
 	ldr	  x0, =running_on_palladium
 	mov   w1,#0x01
 	str   w1,[x0]
    bl main
    b .

    .type start_common, "function"
start_common:

    //
    // program the VBARs
    //

    ldr x1, =el1_vectors
    msr VBAR_EL1, x1

    ldr x1, =el2_vectors
    msr VBAR_EL2, x1

    ldr x1, =el3_vectors
    msr VBAR_EL3, x1

	//
	//	Route aborts, FIQ and IRQ to EL3
	//
	mov x3, #(SCR_EL3_EA | SCR_EL3_FIQ | SCR_EL3_IRQ | SCR_EL3_RW)
    msr SCR_EL3, x3

    // extract the core number from MPIDR_EL1 and store it in
    // x19 (defined by the AAPCS as callee-saved), so we can re-use
    // the number later
    //

    //az in GetCPUID on x0 is used, so store x30(LR) in x1
    mov x1, x30
    bl GetCPUID
    mov x19, x0
    mov x30, x1

    //
    // Don't trap SIMD, floating point or accesses to CPACR
    //
    msr CPTR_EL3, xzr

    //
    // SCTLR_ELx may come out of reset with UNKNOWN values so we will
    // set the fields to 0 except, possibly, the endianess field(s).
    //
#ifdef __ARM_BIG_ENDIAN
    mov x0, #(SCTLR_ELx_EE | SCTLR_EL1_E0E)
#else
    mov x0, #0
#endif
    msr SCTLR_EL3, x0

    //
    // Configure ACTLR_EL[3]
    // ----------------------
    //
    // Controls acces to auxliary registers in lower levels
    //
    msr ACTLR_EL3, xzr

    //
    // configure CPUECTLR_EL1
    //
    // These bits are IMP DEF, so need to different for different
    // processors
    //
    // SMPEN - bit 6 - Enables the processor to receive cache
    //                 and TLB maintenance operations
    //
    // Note: For Cortex-A57/53 SMPEN should be set before enabling
    //       the caches and MMU, or performing any cache and TLB
    //       maintenance operations.
    //
    //       This register has a defined reset value, so we use a
    //       read-modify-write sequence to set SMPEN
    //
    mrs x0, S3_1_c15_c2_1  // Read EL1 CPU Extended Control Register
    orr x0, x0, #(1 << 6)  // Set the SMPEN bit
    msr S3_1_c15_c2_1, x0  // Write EL1 CPU Extended Control Register

    isb


    //
    // That's the last of the control settings for now
    //
    // Note: no ISB after all these changes, as registers won't be
    // accessed until after an exception return, which is itself a
    // context synchronisation event
    //

    //
    // Setup some EL3 stack space, ready for calling some subroutines, below.
    //
    // Stack space allocation is CPU-specific, so use CPU
    // number already held in x19
    //
    // 2^12 bytes per CPU for the EL3 stacks
    //
    ldr x0, =__el3_stack_top__
    sub x0, x0, x19, lsl #12
    mov sp, x0

	str   x30, [sp, #-16]! // push LR to stack

    //
    // SGI #15 is assigned to group1 - non secure interruprs
    //
    mov w0, #15
    mov w1, #1
	bl SetIrqGroup

    //
    // While we're in the Secure World, set the priority mask low enough
    // for it to be writable in the Non-Secure World
    //
    mov w0, #0x1F
    bl  SetPriorityMask

    //
    // Enable floating point
    //
    mov x0, #CPACR_EL1_FPEN
    msr CPACR_EL1, x0

    // Invalidate caches and TLBs for all stage 1
    // translations used at EL1
    //
    // Cortex-A processors automatically invalidate their caches on reset
    // (unless suppressed with the DBGL1RSTDISABLE or L2RSTDISABLE pins).
    // It is therefore not necessary for software to invalidate the caches
    // on startup, however, this is done here in case of a warm reset.
    bl  InvalidateUDCaches
    tlbi VMALLE1

#if 0 // not using MMU yet
    //
    // Set TTBR0 Base address
    //
    // The CPUs share one set of translation tables that are
    // generated by CPU0 at run-time
    //
    //
    ldr x1, =__ttb0_l1
    msr TTBR0_EL3, x1


    //
    // Set up memory attributes
    //
    // These equate to:
    //
    // 0 -> 0b01000100 = 0x00000044 = Normal, Inner/Outer Non-Cacheable
    // 1 -> 0b11111111 = 0x0000ff00 = Normal, Inner/Outer WriteBack Read/Write Allocate
    // 2 -> 0b00000100 = 0x00040000 = Device-nGnRE
    //
    mov  x1, #0xff44
    movk x1, #4, LSL #16    // equiv to: movk x1, #0x0000000000040000
    msr MAIR_EL3, x1


    //
    // Set up TCR_EL3
    //
    // We're using only TTBR0 (EPD1 = 1), and the page table entries:
    //  - are using an 8-bit ASID from TTBR0
    //  - have a 4K granularity (TG0 = 0b00)
    //  - are outer-shareable (SH0 = 0b10)
    //  - are using Inner & Outer WBWA Normal memory ([IO]RGN0 = 0b01)
    //  - map
    //      + 36 bits of VA space (T0SZ = 0x20 - (region size 2^(64-32) = 2^32))
    //      + into a 32-bit PA space (PS = 0b00)
    //
    //     28   24   20   16   12    8    4    0
    //  +----+----+----+----+----+----+----+----+
    //  |    |    |    |    |    |OOII|    |    |
    //  |R   |   R|R R |R   |    |RRRR| RT |   T|
    //  |E   |   E|E ET|E   |TTSS|GGGG| E0 |   0|
    //  |S   |   S|S SB|S  P|GGHH|NNNN| SS |   S|
    //  |1---|---0|1-0I|0--S|0000|0000|-0Z-|---Z|
    //
    //   0000 0000 0000 0001 0010 0101 0001 1100
    //
    // 0x  0    0    0    0    2    5    2    0
    //
    ldr x1, =0x2520
    msr TCR_EL3, x1
    isb


    //
    // x19 already contains the CPU number, so branch to secondary
    // code if we're not on CPU0
    //
    cbnz x19, el3_secondary
#endif
    //
    // Fall through to primary code
    //


//
// ------------------------------------------------------------
//
// EL3 - primary CPU init code
//
// This code is run on CPU0, while the other CPUs are in the
// holding pen
//

    .global el3_primary
    .type el3_primary, "function"
el3_primary:

    //
    // We're now on the primary processor, so turn GIC distributor
    // and CPU interface
    bl  EnableGICD

	//
	// Enabling secure FIQ will generate FIQ for Group 0 interrupts; otherwise IRQ will be generated
	//
    bl 	EnableSecureFIQ

	//
	// Enable GIC CPU interface
	//
    bl  EnableGICC

    //
    // Enable the MMU
    //
    mrs x1, SCTLR_EL3
//    orr x1, x1, #SCTLR_ELx_M
//    bic x1, x1, #SCTLR_ELx_A // Disable alignment fault checking.  To enable, change bic to orr
//    orr x1, x1, #SCTLR_ELx_C
//    orr x1, x1, #SCTLR_ELx_I
    msr SCTLR_EL3, x1
    isb

	// Branch to core0 main funtion
    bl init_after_startup

    ldr   x30, [sp], #16   // pop LR from stack
	ret

// ------------------------------------------------------------
// EL3 - secondary CPU init code
//
// This code is run on CPUs 1, 2, 3 etc....
// ------------------------------------------------------------

    .global el3_secondary
    .type el3_secondary, "function"
el3_secondary:

#ifdef WAKE_ON_SGI
    //
    // the primary CPU is going to use SGI 15 as a wakeup event
    // to let us know when it is OK to proceed, so prepare for
    // receiving that interrupt
    //
    // NS interrupt priorities run from 0 to 15, with 15 being
    // too low a priority to ever raise an interrupt, so let's
    // use 14
    //
    mov w0, #15
    mov w1, #(14 << 1) // we're in NS world, so adjustment is needed
    bl  SetIRQPriority

    mov w0, #15
    bl  EnableIRQ

    mov w0, #(15 << 1)
    bl  SetPriorityMask

#endif

	//
	// Enable GIC CPU interface
	//
    bl  EnableGICC

    //
    // wait for our interrupt to arrive
    //

loop_wfi:
    dsb SY      // Clear all pending data accesses
#ifdef WAKE_ON_SGI
    wfi         // Go to sleep
#else
	wfe
#endif

#ifdef WAKE_ON_SGI
    //
    // something woke us from our wait, was it the required interrupt?
    //
    mov w0, #15
    bl  TestIRQ
    cbz w0, loop_wfi

    //
    // it was - there's no need to actually take the interrupt,
    // so just clear it
    //
    mov w0, #15
    mov w1, #0        // IRQ was raised by the primary CPU
    bl  ClearSGI
#endif
    //
    // Enable the MMU and caches
    //
    mrs x1, SCTLR_EL3
    orr x1, x1, #SCTLR_ELx_M
    orr x1, x1, #SCTLR_ELx_C
    orr x1, x1, #SCTLR_ELx_I
    bic x1, x1, #SCTLR_ELx_A // Disable alignment fault checking.  To enable, change bic to orr
    msr SCTLR_EL3, x1
    isb

    //
    // Call secondary CPU main app
    // Each core is directed to its own private main function based on core ID that is stored in x19
    //
    cmp x19, #1
    beq	main_core1

    cmp x19, #2
    beq	main_core2

	// Only core 3 ahould get here so no additional verification is required
    b 	main_core3
