// ------------------------------------------------------------
// Armv8-A Vector tables
//
// Copyright (c) 2014-2016 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
// ------------------------------------------------------------
    .section ._arm_vector_table, "ax"
    .balign 4
    .global start64
    .global start64_semihosting
    .global start64_semihosting_palladium


// ------------------------------------------------------------

    .global _startup
    .type rom_start, "function"
_startup:
    bl start64

    .global _startup_semihosting
    .type rom_start_semihosting, "function"
_startup_semihosting:
    bl start64_semihosting

    .global _startup_semihosting_palladium
    .type rom_start_semihosting_palladium, "function"
_startup_semihosting_palladium:
    bl start64_semihosting_palladium


    .global el1_vectors
    .global el2_vectors
    .global el3_vectors
    .global c0sync1
    .global GIC_Isr
    .global fiqHandler
    .global irqFirstLevelHandler
    .global fiqFirstLevelHandler

    .section  ._arm_vector_table.EL1VECTORS, "ax"
    .align 11

//
// Current EL with SP0
//
el1_vectors:
c0sync1: B c0sync1

    .balign 0x80
c0irq1: B irqFirstLevelHandler

    .balign 0x80
c0fiq1: B fiqFirstLevelHandler

    .balign 0x80
c0serr1: B c0serr1

//
// Current EL with SPx
//
    .balign 0x80
cxsync1: B cxsync1

    .balign 0x80
cxirq1: B irqFirstLevelHandler

    .balign 0x80
cxfiq1: B fiqFirstLevelHandler

    .balign 0x80
cxserr1: B cxserr1

//
// Lower EL using AArch64
//
    .balign 0x80
l64sync1: B l64sync1

    .balign 0x80
l64irq1: B irqFirstLevelHandler

    .balign 0x80
l64fiq1: B fiqFirstLevelHandler

    .balign 0x80
l64serr1: B l64serr1

//
// Lower EL using AArch32
//
    .balign 0x80
l32sync1: B l32sync1

    .balign 0x80
l32irq1: B irqFirstLevelHandler

    .balign 0x80
l32fiq1: B fiqFirstLevelHandler

    .balign 0x80
l32serr1: B l32serr1

//----------------------------------------------------------------

    .section  ._arm_vector_table.EL2VECTORS, "ax"
    .align 11

//
// Current EL with SP0
//
el2_vectors:
c0sync2: B c0sync2

    .balign 0x80
c0irq2: B irqFirstLevelHandler

    .balign 0x80
c0fiq2: B fiqFirstLevelHandler

    .balign 0x80
c0serr2: B c0serr2

//
// Current EL with SPx
//
    .balign 0x80
cxsync2: B cxsync2

    .balign 0x80
cxirq2: B irqFirstLevelHandler

    .balign 0x80
cxfiq2: B fiqFirstLevelHandler

    .balign 0x80
cxserr2: B cxserr2

//
// Lower EL using AArch64
//
    .balign 0x80
l64sync2: B l64sync2

    .balign 0x80
l64irq2: B irqFirstLevelHandler

    .balign 0x80
l64fiq2: B fiqFirstLevelHandler

    .balign 0x80
l64serr2: B l64serr2

//
// Lower EL using AArch32
//
    .balign 0x80
l32sync2: B l32sync2

    .balign 0x80
l32irq2: B irqFirstLevelHandler

    .balign 0x80
l32fiq2: B fiqFirstLevelHandler

    .balign 0x80
l32serr2: B l32serr2

//----------------------------------------------------------------

    .section  ._arm_vector_table.EL3VECTORS, "ax"
    .align 11

//
// Current EL with SP0
//
el3_vectors:
c0sync3: B c0sync3

    .balign 0x80
c0irq3: B irqFirstLevelHandler

    .balign 0x80
c0fiq3: B fiqFirstLevelHandler

    .balign 0x80
c0serr3: B c0serr3

//
// Current EL with SPx
//
    .balign 0x80
cxsync3: B cxsync3

    .balign 0x80
cxirq3: B irqFirstLevelHandler

    .balign 0x80
cxfiq3: B fiqFirstLevelHandler

    .balign 0x80
cxserr3: B cxserr3

//
// Lower EL using AArch64
//
    .balign 0x80
l64sync3: B l64sync3

    .balign 0x80
l64irq3: B irqFirstLevelHandler

    .balign 0x80
l64fiq3: B fiqFirstLevelHandler

    .balign 0x80
l64serr3: B l64serr3

//
// Lower EL using AArch32
//
    .balign 0x80
l32sync3: B l32sync3

    .balign 0x80
l32irq3: B irqFirstLevelHandler

    .balign 0x80
l32fiq3: B fiqFirstLevelHandler

    .balign 0x80
l32serr3: B l32serr3


    .section .text.InterruptHandlers, "ax"
    .balign 4

    .type irqFirstLevelHandler, "function"
irqFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  STP      x18, x19, [sp, #-16]!
  STP      x16, x17, [sp, #-16]!
  STP      x14, x15, [sp, #-16]!
  STP      x12, x13, [sp, #-16]!
  STP      x10, x11, [sp, #-16]!
  STP      x8, x9, [sp, #-16]!
  STP      x6, x7, [sp, #-16]!
  STP      x4, x5, [sp, #-16]!
  STP      x2, x3, [sp, #-16]!
  STP      x0, x1, [sp, #-16]!

  BL       GIC_Isr

  LDP      x0, x1, [sp], #16
  LDP      x2, x3, [sp], #16
  LDP      x4, x5, [sp], #16
  LDP      x6, x7, [sp], #16
  LDP      x8, x9, [sp], #16
  LDP      x10, x11, [sp], #16
  LDP      x12, x13, [sp], #16
  LDP      x14, x15, [sp], #16
  LDP      x16, x17, [sp], #16
  LDP      x18, x19, [sp], #16
  LDP      x29, x30, [sp], #16
  ERET

    .type fiqFirstLevelHandler, "function"
fiqFirstLevelHandler:
  STP      x29, x30, [sp, #-16]!
  STP      x18, x19, [sp, #-16]!
  STP      x16, x17, [sp, #-16]!
  STP      x14, x15, [sp, #-16]!
  STP      x12, x13, [sp, #-16]!
  STP      x10, x11, [sp, #-16]!
  STP      x8, x9, [sp, #-16]!
  STP      x6, x7, [sp, #-16]!
  STP      x4, x5, [sp, #-16]!
  STP      x2, x3, [sp, #-16]!
  STP      x0, x1, [sp, #-16]!

  BL       fiqHandler

  LDP      x0, x1, [sp], #16
  LDP      x2, x3, [sp], #16
  LDP      x4, x5, [sp], #16
  LDP      x6, x7, [sp], #16
  LDP      x8, x9, [sp], #16
  LDP      x10, x11, [sp], #16
  LDP      x12, x13, [sp], #16
  LDP      x14, x15, [sp], #16
  LDP      x16, x17, [sp], #16
  LDP      x18, x19, [sp], #16
  LDP      x29, x30, [sp], #16
  ERET

