/*
 * file : cpu_config.h
 *
 *
 */

#ifndef _cpu_config_H
#define _cpu_config_H

// GIC (INTC for A9) first 32 interrupts are not used
#define GIC_INTERRUPT_NUM               160


#define GIC_PHYS_BASE_ADDR              0xF03FE000
#define GIC_BASE_ADDR                   GIC_PHYS_BASE_ADDR


#define GIC_CPU_INTERFACE_OFFSET   0x0100
#define GIC_DISTRIBUTOR_OFFSET     0x1000

#define GIC_CPU_INTERFACE_BASE_ADDR  (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET)
#define GIC_DISTRIBUTOR_BASE_ADDR    (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET)

#endif /* */
