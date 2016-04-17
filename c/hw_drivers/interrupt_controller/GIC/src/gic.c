/*
 *
 * gic.c
 *
 *
 *
 *
 *
 */


#include "Drivers_APP_HAL/GIC/gic_hal.h"
#include "gic.h"
#include "gic_API.h"

#define GIC_CPU_INTERFACE_OFFSET   0x0100
#define GIC_DISTRIBUTOR_OFFSET     0x1000

#define GICC_CTLR              (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0000)
#define GICC_PMR               (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0004)
#define GICC_IAR               (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x000C)
#define GICC_EOIR              (GIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0010)


#define GICD_CTLR              (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0000)
#define GICD_TYPER             (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0004)
#define GICD_IIDR              (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0008)
#define GICD_ISENABLER         (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0100)
#define GICD_ICENABLER         (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0180)
#define GICD_IPRIORITYR        (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0400)
#define GICD_ITARGETSR         (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0800)
#define GICD_ICFGR             (GIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0C00)

#define GIC_PRI_LOWEST     0xf0
#define GIC_PRI_IRQ        0xa0
#define GIC_PRI_IPI        0x90 /* IPIs must preempt normal interrupts */
#define GIC_PRI_HIGHEST    0x80 /* Higher priorities belong to Secure-World */

#define GICC_CTL_EOI (0x1 << 9)
#define GICD_CTL_ENABLE 0x1
#define GICC_CTL_ENABLE 0x1

#define GICD_TYPE_LINES 0x01f
#define GICD_TYPE_CPUS  0x0e0
#define GICD_TYPE_SEC   0x400

#define REG_WRITE_U32(addr,val)		( (*(volatile unsigned int *)(addr)) = (val) )
#define REG_READ_U32(addr)			( *(volatile unsigned int *)(addr) )
#define REG_WRITE_U16(addr,val)		( (*(volatile unsigned short *)(addr)) = (val) )
#define REG_READ_U16(addr)			( *(volatile unsigned short *)(addr) )
#define REG_WRITE_U8(addr,val)		( (*(volatile unsigned char *)(addr)) = (val) )
#define REG_READ_U8(addr)			( *(volatile unsigned char *)(addr) )


int GIC_EndOfService(uint32_t  int_num);
/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Group related defines                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define GIC_GROUP_INTERRUPT_NONE            0xFF


/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler Table                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

typedef struct
{
    GIC_Isr_T   func;
   // uint32_t       param;
}   GIC_IsrEntry_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Function forward declaration                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
static GIC_IsrEntry_T GIC_handler_table[GIC_INTERRUPT_NUM];


static int g_init_done=0;


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_RegisterHandler                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func -                                                                                 */
/*                  int_num -                                                                              */
/*                  param -                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs interrup registration                                            */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_API_RegisterHandler(uint32_t  int_num, GIC_Isr_T func )
{

    if((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
    {
        return 1;
    }

    GIC_handler_table[int_num].func = func;

    return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_EnableInt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_API_EnableInt(uint32_t  int_num)
{

	volatile uint32_t  * reg_address;
    uint32_t  mask = 1 << (int_num % 32);

    if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
    {
        return 1;
    }


	reg_address = (volatile uint32_t  *)(GICD_ISENABLER +( int_num/32 ) * 4);
    *reg_address = mask;


    return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_DisableInt                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_API_DisableInt(uint32_t  int_num)
{

	volatile uint32_t  * reg_address;
    uint32_t  mask = 1 << (int_num % 32);


    if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
    {
        return 1;
    }

	reg_address = (volatile uint32_t  *)( GICD_ICENABLER  + ( int_num/32 ) * 4);
    *reg_address = mask;
	GIC_EndOfService(int_num);

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_EnableGroupInt                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the GIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt in group                                           */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_EnableGroupInt(uint32_t  group_bit_num)
{


    return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_DisableGroupInt                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt in group                                          */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_DisableGroupInt(uint32_t  group_bit_num)
{

    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_IsGroupIntEnabled                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns true is group_bit_num is set                                      */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_IsGroupIntEnabled(uint32_t  group_bit_num)
{

    return 1;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_EndOfService                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine acknowledges that IRQ is served                                           */
/*---------------------------------------------------------------------------------------------------------*/
int GIC_EndOfService(uint32_t  int_num)
{
	//LogMessage("GIC_EndOfService\n");

	REG_WRITE_U32(GICC_EOIR, int_num);


    return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_ExecuteIsr                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         if interrupt was executed return 0 , otherwise 1                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine acknowledges that IRQ is served                                           */
/*---------------------------------------------------------------------------------------------------------*/
int  GIC_ExecuteIsr(uint32_t aIntNum)
{
	if ((aIntNum >= 32) && (aIntNum < GIC_INTERRUPT_NUM))
	{
       /*-------------------------------------------------------------------------------------------------*/
       /* If handler installed, execute it                                                                */
       /*-------------------------------------------------------------------------------------------------*/
       if (GIC_handler_table[aIntNum].func)
       {
           GIC_handler_table[aIntNum].func();
       }

       return 0;
	}

	return 1;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_API_ExecuteIsr                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine acknowledges that IRQ is served                                           */
/*---------------------------------------------------------------------------------------------------------*/
void  GIC_API_ExecuteIsr(uint32_t aIntNum)
{
	GIC_ExecuteIsr(aIntNum);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GIC_Isr                                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is the main IRQ handler                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void __attribute__((interrupt("IRQ"))) GIC_Isr(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reading the highest priority unhandled interrupt                                                    */
    /* IPER returns the interrupt number multipled by 4 (for easier table lookup)                          */
    /*-----------------------------------------------------------------------------------------------------*/
    uint32_t  irq_num;


    gic_hal_disable_interrupts();

    if (0==g_init_done) return;
	//LogMessage("GIC_ISR");
    while (1)
	{
        irq_num = REG_READ_U32(GICC_IAR);
		irq_num &= (uint32_t )0x03FF;


		if ( 0== GIC_ExecuteIsr(irq_num))
		{
		   GIC_EndOfService(irq_num);
		   continue;
		}

		break;
    }

    gic_hal_enable_interrupts();


}


/*
 *
 */
void GIC_API_EnableAllInt(void)
{
    if (0==g_init_done) return;

	gic_hal_enable_interrupts();
}

/*
 *
 */
void GIC_API_DisableAllInt(void)
{
	gic_hal_disable_interrupts();
}

void  GIC_API_Init(void)
{
    uint32_t  i;
	volatile uint32_t  * reg_address;
	uint32_t  cpumask;
//	uint32_t  gic_type;

	gic_hal_disable_interrupts();
	g_init_done=0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* clean interrupt handler function pointers table                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    for(i=0; i<GIC_INTERRUPT_NUM; i++)
    {
        GIC_handler_table[i].func = NULL;
    }

    cpumask = REG_READ_U32(GICD_ITARGETSR) & 0xff;
    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16;

    REG_WRITE_U32(GICD_CTLR, (uint32_t )0x00);    // Disable GIC Distributor

//    gic_type = REG_READ_U32(GICD_TYPER);
//	gic_iidr = REG_READ_U32(GICD_IIDR);
 //   gic_lines = 32 * ((gic_type & GICD_TYPE_LINES) + 1);
 //   gic_cpus = 1 + ((gic_type & GICD_TYPE_CPUS) >> 5);

//    printf("GIC: %d lines, %d cpu%s%s (IID %8.8x).\n",
//           gic_lines, gic_cpus, (gic_cpus == 1) ? "" : "s",
//           (gic_type & GICD_TYPE_SEC) ? ", secure" : "",
//           gic_iidr);


    /*
     * Set all global interrupts to be level triggered, active High.
     */
    for (i = 32; i < GIC_INTERRUPT_NUM; i += 16)
	{
	    reg_address = (volatile uint32_t  *)((GICD_ICFGR) + (i * 4/16));
        *reg_address = (uint32_t )0x00;
    }
    /*
     * Set all global interrupts to this CPU only (CPU0).
     */
    for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
	    reg_address = (volatile uint32_t  *)((GICD_ITARGETSR) + (i * 4/4));
        *reg_address = cpumask;
    }

    /*
     * Set priority on all interrupts.
     */
    for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
	    reg_address = (volatile uint32_t  *)((GICD_IPRIORITYR) + (i * 4/4));
        *reg_address =  (GIC_PRI_IRQ<<24) | (GIC_PRI_IRQ<<16) | (GIC_PRI_IRQ<<8) | (GIC_PRI_IRQ);
    }

    /*
     * Disable all interrupts.
     */
    for (i = 32; i < GIC_INTERRUPT_NUM; i += 32)
	{
	    reg_address = (volatile uint32_t  *)((GICD_ICENABLER) + (i * 4/32));
        *reg_address = 0xFFFFFFFF;
    }


    REG_WRITE_U32(GICD_CTLR, (uint32_t )GICD_CTL_ENABLE);                   // Enable GIC Distributor

    REG_WRITE_U32(GICD_ICENABLER, 0xFFFFFFFF);                           // Disable all SGI + PPI

    REG_WRITE_U32(GICC_PMR, (uint32_t )0xFF);                               // Set Priority Mask

    REG_WRITE_U32(GICC_CTLR, (uint32_t )GICC_CTL_ENABLE | GICC_CTL_EOI);    // Enable GIC CPU Interface



	gic_hal_init(GIC_Isr);

}


void  GIC_API_Open(void)
{
	g_init_done=1;
}

