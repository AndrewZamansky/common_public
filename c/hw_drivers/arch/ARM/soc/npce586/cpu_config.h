/*
 * file : cpu_config.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H


#define ARM_MATH_CM4	1

/* Configuration of the Cortex-M# Processor and Core Peripherals */
#define __CM4_REV                 0x0201    /*!< Core Revision r2p1                               */
#define __NVIC_PRIO_BITS          4         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    1         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __FPU_PRESENT             1         /*!< FPU present or not                               */

#define INTERRUPT_LOWEST_PRIORITY    ((1<<__NVIC_PRIO_BITS)-1)

#endif /* */
