/*
 *
 * gic.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "gic.h"

#include "irq_api.h"

#include "auto_init_api.h"


/*******  cpu interface registers ****************/
#define GICC_CTLR        (GIC_CPU_INTERFACE_BASE_ADDR + 0x0000)
#define GICC_CTL_ENABLE_GRP0  0x1
#define GICC_CTL_ENABLE_GRP1  0x2

#define GICC_PMR         (GIC_CPU_INTERFACE_BASE_ADDR + 0x0004)
#define GICC_IAR         (GIC_CPU_INTERFACE_BASE_ADDR + 0x000C)
#define GICC_EOIR        (GIC_CPU_INTERFACE_BASE_ADDR + 0x0010)
#define GICC_IIDR        (GIC_CPU_INTERFACE_BASE_ADDR + 0x00FC)


/*******  distributor registers ****************/
#define GICD_CTLR            (GIC_DISTRIBUTOR_BASE_ADDR + 0x0000)
#define GICD_CTL_ENABLE_GRP0  0x1
#define GICD_CTL_ENABLE_GRP1  0x2

#define GICD_TYPER           (GIC_DISTRIBUTOR_BASE_ADDR + 0x0004)
#define GICD_IIDR            (GIC_DISTRIBUTOR_BASE_ADDR + 0x0008)
#define GICD_ISENABLER       (GIC_DISTRIBUTOR_BASE_ADDR + 0x0100)
#define GICD_ICENABLER       (GIC_DISTRIBUTOR_BASE_ADDR + 0x0180)
#define GICD_ICPENDR         (GIC_DISTRIBUTOR_BASE_ADDR + 0x0280)

#define GICD_IPRIORITYR      (GIC_DISTRIBUTOR_BASE_ADDR + 0x0400)
#define GIC_PRI_LOWEST     0xf0
#define GIC_PRI_IRQ        0xa0
#define GIC_PRI_IPI        0x90 /* IPIs must preempt normal interrupts */
#define GIC_PRI_HIGHEST    0x80 /* Higher priorities belong to Secure-World */

#define GICD_ITARGETSR       (GIC_DISTRIBUTOR_BASE_ADDR + 0x0800)
#define GICD_ICFGR           (GIC_DISTRIBUTOR_BASE_ADDR + 0x0C00)



#define GICD_TYPE_LINES 0x01f
#define GICD_TYPE_CPUS  0x0e0
#define GICD_TYPE_SEC   0x400

#define REG_WRITE_U32(addr, val)  \
			( (*(volatile uint32_t *)((size_t)addr)) = (val) )
#define REG_READ_U32(addr)       ( *(volatile uint32_t *)((size_t)addr) )


#define GIC_VERSION_MASK  0x000F0000
#define GET_GIC_VERSION()   (REG_READ_U32(GICC_IIDR) & 0x000F0000)
#define GIC_VERSION_V1    0x00010000
#define GIC_VERSION_V2    0x00020000


int GIC_EndOfService(uint32_t  int_num);

/*-----------------------------------------------------------------------
 * Interrupt Handler Table
 *----------------------------------------------------------------------
 */

struct GIC_IsrEntry_T {
    isr_t   func;
   // uint32_t       param;
};

/*-----------------------------------------------------------------------
 * Function forward declaration
 *-----------------------------------------------------------------------
 */
static struct GIC_IsrEntry_T GIC_handler_table[GIC_INTERRUPT_NUM];


static int g_init_done = 0;

#if defined(CONFIG_CODE_LOCATION_INTERNAL_SRAM)
	#define VECTOR_TABLE_BA  0xfffd0000
#else
	#define VECTOR_TABLE_BA  0x0
#endif

/* function   gic_hal_init
 *
 */
void	gic_hal_init(void ( GIC_Isr)(void))
{
#if defined(CONFIG_CORTEX_A35)
	//do nothing
#else
	// install isr  (az fiq also installed ??)
	*((volatile unsigned int *)(VECTOR_TABLE_BA + 0x18))=0xE59FF018;
	*((volatile unsigned int *)(VECTOR_TABLE_BA + 0x38))=(uint32_t)GIC_Isr;
#endif
}


/*  function   gic_hal_disable_interrupts
 *
 */
void gic_hal_disable_interrupts  (  void )
{ /* Body */
#if defined(__GNUC__)
	#if defined(CONFIG_CORTEX_A35)
		__asm__ __volatile__("msr DAIFSet, #0x7\n");
	#else
		unsigned long old,temp;
		__asm__ __volatile__("mrs %0, cpsr\n"
					 "orr %1, %0, #0xc0\n"
					 "msr cpsr_c, %1"
					 : "=r" (old), "=r" (temp)
					 :
					 : "memory");
	#endif
#elif defined(__arm)
	#if defined(CONFIG_CORTEX_A35)
		#error "TODO"
	#else
		unsigned long old,temp;
		__asm
		{
			mrs old, cpsr
			orr temp, old, #0xc0
			msr cpsr_c, temp
		};
	#endif
#endif
} /* EndBody */


/*  function   gic_hal_enable_interrupts
 *
 *
 */
void gic_hal_enable_interrupts (  void  )
{ /* Body */
	//GIC_EnableInt(USB_INTERRUPT_NUM);
#if defined(__GNUC__)
	#if defined(CONFIG_CORTEX_A35)
		__asm__ __volatile__("msr DAIFClr, #0x7\n");
	#else
		unsigned long temp;
		__asm__ __volatile__("mrs %0, cpsr\n"
					 "bic %0, %0, #0x80\n"
					 "msr cpsr_c, %0"
					 : "=r" (temp)
					 :
					 : "memory");
	#endif
#elif defined(__arm)
	#if defined(CONFIG_CORTEX_A35)
		#error "TODO"
	#else
		unsigned long temp;
		__asm
		{
			mrs temp, cpsr
			bic temp, temp, #0x80
			msr cpsr_c, temp
		};
	#endif
#endif


} /* EndBody */


/*-------------------------------------------------------------------------
 * Function:        irq_register_interrupt
 * Parameters:
 * Returns:
 *-----------------------------------------------------------------------
 */
int irq_register_interrupt(int  int_num, isr_t func )
{

	if((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
		return 1;
	}

	GIC_handler_table[int_num].func = func;

	return 0;
}




/*---------------------------------------------------------------------
 * Function:        irq_enable_interrupt
 * Parameters:
 * Returns:
 *---------------------------------------------------------------------
 */
void irq_enable_interrupt(int  int_num)
{

	volatile uint32_t* reg_address;
	uint32_t  mask;

	if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
		CRITICAL_ERROR("no such interrupt");
	}

	reg_address =
			(volatile uint32_t*)(size_t)(GICD_ISENABLER + (int_num / 32) * 4);
	mask = 1 << (int_num % 32);
	*reg_address = mask;
}


/*-----------------------------------------------------------------------
 * Function:        irq_disable_interrupt
 * Parameters:
 * Returns:         none
 *----------------------------------------------------------------------
 */
void irq_disable_interrupt(int  int_num)
{

	volatile uint32_t  * reg_address;
	uint32_t  mask;

	if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
		CRITICAL_ERROR("no such interrupt");
	}

	reg_address =
			(volatile uint32_t*)(size_t)( GICD_ICPENDR  + (int_num / 32) * 4);
	mask = 1 << (int_num % 32);
	*reg_address = mask;
	GIC_EndOfService(int_num);
}

/*-------------------------------------------------------------------------
 * Function:        GIC_EnableGroupInt
 * Parameters:
 * Returns:         none
 *-------------------------------------------------------------------------
*/
int GIC_EnableGroupInt(uint32_t  group_bit_num)
{


    return 0;
}


/*-------------------------------------------------------------------------
 * Function:        GIC_DisableGroupInt
 * Parameters:
 * Returns:         none
 *-------------------------------------------------------------------------
 */
int GIC_DisableGroupInt(uint32_t  group_bit_num)
{

	return 0;
}

/*----------------------------------------------------------------------
 * Function:        GIC_IsGroupIntEnabled
 * Parameters:
 * Returns:
 *--------------------------------------------------------------------
 */
int GIC_IsGroupIntEnabled(uint32_t  group_bit_num)
{

	return 1;

}


/** Clear a pending interrupt
    @param[in] irq IRQ number
 */
void gicd_clear_pending(uint32_t  int_num)
{
	volatile uint32_t  * reg_address;
	uint32_t  mask;

	if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
		CRITICAL_ERROR("no such interrupt");
	}

	reg_address =
			(volatile uint32_t*)(size_t)( GICD_ICPENDR  + (int_num / 32) * 4);
	mask = 1 << (int_num % 32);
	*reg_address = mask;
}

/*-------------------------------------------------------------------
 * Function:        GIC_EndOfService
 * Parameters:
 * Returns:         none
 *-------------------------------------------------------------------
 */
int GIC_EndOfService(uint32_t  int_num)
{
	//LogMessage("GIC_EndOfService\n");

	gicd_clear_pending(int_num);
	REG_WRITE_U32(GICC_EOIR, int_num);


	return 0;
}

/*-----------------------------------------------------------------------
 * Function:        GIC_ExecuteIsr
 * Parameters:
 * Returns:         if interrupt was executed return 0 , otherwise 1
 *-----------------------------------------------------------------------
 */
int  GIC_ExecuteIsr(uint32_t aIntNum)
{
	if ((aIntNum >= 32) && (aIntNum < GIC_INTERRUPT_NUM))
	{
	   /*-----------------------------------------------------------------
		* If handler installed, execute it
		*----------------------------------------------------------------
		*/
	   if (GIC_handler_table[aIntNum].func)
	   {
		   GIC_handler_table[aIntNum].func();
	   }

	   return 0;
	}

	return 1;
}


/*------------------------------------------------------------------------
 * Function:        GIC_API_ExecuteIsr
 * Parameters:
 * Returns:         none
 *-------------------------------------------------------------------------
*/
void  GIC_API_ExecuteIsr(uint32_t aIntNum)
{
	GIC_ExecuteIsr(aIntNum);
}

volatile int _dbg_cnt = 0;

/*-----------------------------------------------------------------------
 * Function:        GIC_Isr
 * Parameters:      none
 * Returns:         none
 *------------------------------------------------------------------------
 */
#if defined(CONFIG_CORTEX_A35)
void GIC_Isr(void)
#else
void __attribute__((interrupt("IRQ"))) GIC_Isr(void)
#endif
{
	/*---------------------------------------------------------------
	* Reading the highest priority unhandled interrupt
	* IPER returns the interrupt number multipled by 4 (for easier table lookup)
	*-----------------------------------------------------------------------
	*/
	_dbg_cnt++;

	gic_hal_disable_interrupts();

	if (0==g_init_done) return;
	//LogMessage("GIC_ISR");
	while (1)
	{
		uint32_t  irq_num;

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
void irq_unblock_all(void)
{
	if (0==g_init_done) return;

	gic_hal_enable_interrupts();
}

/*
 *
 */
void irq_block_all(void)
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

	/*--------------------------------------------------------------
	* clean interrupt handler function pointers table
    *--------------------------------------------------------------
    */
	for(i=0; i<GIC_INTERRUPT_NUM; i++)
	{
		GIC_handler_table[i].func = NULL;
	}


	REG_WRITE_U32(GICD_CTLR, (uint32_t )0x00);    // Disable GIC Distributor

//	gic_type = REG_READ_U32(GICD_TYPER);
//	gic_iidr = REG_READ_U32(GICD_IIDR);
 //   gic_lines = 32 * ((gic_type & GICD_TYPE_LINES) + 1);
 //   gic_cpus = 1 + ((gic_type & GICD_TYPE_CPUS) >> 5);

//	printf("GIC: %d lines, %d cpu%s%s (IID %8.8x).\n",
//		   gic_lines, gic_cpus, (gic_cpus == 1) ? "" : "s",
//		   (gic_type & GICD_TYPE_SEC) ? ", secure" : "",
//		   gic_iidr);


	/*
	 * Set all global interrupts to be level triggered, active High.
	 */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 16)
	{
		reg_address = (volatile uint32_t*)(size_t)((GICD_ICFGR) + (i * 4/16));
		*reg_address = (uint32_t )0x00;
	}


	/*
	 * Set all global interrupts to this CPU only (CPU0).
	 */
#if 0// not clear code
	cpumask = REG_READ_U32(GICD_ITARGETSR) & 0xff;
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
#else
	cpumask = 0x01010101;
#endif
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
		reg_address = (volatile uint32_t*)(size_t)(GICD_ITARGETSR + i);
		*reg_address = cpumask;
	}

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
		reg_address = (volatile uint32_t  *)(size_t)(GICD_IPRIORITYR + i);
		*reg_address = (GIC_PRI_IRQ << 24) |
				(GIC_PRI_IRQ << 16) | (GIC_PRI_IRQ << 8) | (GIC_PRI_IRQ);
	}

	/*
	 * Disable all interrupts including 0-32(SGI + PPI) .
	 */
	for (i = 0; i < GIC_INTERRUPT_NUM; i += 32)
	{
		reg_address =
				(volatile uint32_t  *)(size_t)(GICD_ICENABLER + ((i * 4) / 32));
		*reg_address = 0xFFFFFFFF;
	}

	// Enable GIC Distributor
	REG_WRITE_U32(GICD_CTLR,
			(uint32_t )(GICD_CTL_ENABLE_GRP0 | GICD_CTL_ENABLE_GRP1));


    REG_WRITE_U32(GICC_PMR, (uint32_t )0xFF);// Set Priority Mask

    // Enable GIC CPU Interface
    REG_WRITE_U32(GICC_CTLR,
    		(uint32_t )(GICC_CTL_ENABLE_GRP0 | GICC_CTL_ENABLE_GRP1));


	gic_hal_init(GIC_Isr);
	g_init_done = 1;
}


AUTO_INIT_FUNCTION(GIC_API_Init);
