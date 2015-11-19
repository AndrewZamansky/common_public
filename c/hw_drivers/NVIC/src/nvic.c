/*
 *
 * file :   NVIC.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "nvic_config.h"

#include "NVIC_api.h"

#if CORTEX_M_TYPE == 4
 #include "core_cm4.h"
#elif CORTEX_M_TYPE == 3
 #include "core_cm3.h"
#else
 #error unknown cortex-m type
#endif

#ifdef GCC
  #define IRQ_ATTR	__attribute__((interrupt("IRQ")))
#elif defined ARMCC
  #define IRQ_ATTR	__irq
#endif

//#include "stm32f10x.h"
//#include "core_cm3.h"
//#include "misc.h"

#define NVIC_PriorityGroup_0         ((uint32_t)0x7) /*!< 0 bits for pre-emption priority
                                                            4 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x6) /*!< 1 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x5) /*!< 2 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x4) /*!< 3 bits for pre-emption priority
                                                            1 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x3) /*!< 4 bits for pre-emption priority
                                                            0 bits for subpriority */

#define NVIC_hal_writeRegU32(addr,val)		( (*(volatile uint32_t *)(addr)) = (val) )
#define NVIC_hal_readRegU32(addr)			( *(volatile uint32_t *)(addr) )

extern unsigned int Top_Of_Stacks ;
extern unsigned int do_startup ;
//#define NVIC_hal_Stack_Top			(&Top_Of_Stacks) // defined in linker file
//#define NVIC_hal_APP_Start			(&__APP_START_ADDR__) // defined in linker file


/********  defines *********************/


#define NVIC_INTERRUPT_MAX_NUM  239

#define NVIC_VECTOR_TABLE_OFFSET      	0xE000ED08



/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        NVIC_API_RegisterInt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                   pIsr -                                                                                      */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt                                                    */
/*---------------------------------------------------------------------------------------------------------*/
int NVIC_API_RegisterInt(IRQn_Type int_num , NVIC_Isr_t pIsr)
{

	// +16 offset becouse IRQn_Type starts from -16
    NVIC_hal_writeRegU32( CONFIG_RAM_START_ADDR + (((int)int_num + 16) << 2),(unsigned int)pIsr);// ( int_num  ) * 4
    return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        NVIC_DisableInt                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int NVIC_API_DisableInt(IRQn_Type int_num)
{
	NVIC_DisableIRQ(int_num);
    NVIC_ClearPendingIRQ(int_num);

    return 0;
}


/*
 *
 */
void NVIC_API_EnableAllInt(void)
{

    __asm__ __volatile__("cpsie i\n");
}

/*
 *
 */
void NVIC_API_DisableAllInt(void)
{
	__asm__ __volatile__("cpsid i\n");
}


void IRQ_ATTR HardFault_Isr(void)
{
	while(1);
}

void IRQ_ATTR NMI_Isr(void)
{
	while(1);
}

void  NVIC_API_Init(void)
{
	int  i;


	__asm__ __volatile__("cpsid i\n");

    /*
     * Disable all interrupts.
     */
    for (i = NonMaskableInt_IRQn ; i < NVIC_INTERRUPT_MAX_NUM; i++)
	{
    	NVIC_DisableIRQ(i);
        NVIC_ClearPendingIRQ(i);
    }

    NVIC_hal_writeRegU32(NVIC_VECTOR_TABLE_OFFSET , CONFIG_RAM_START_ADDR);// set base to RAM
    NVIC_hal_writeRegU32( CONFIG_RAM_START_ADDR ,(uint32_t )&Top_Of_Stacks);
    NVIC_hal_writeRegU32( CONFIG_RAM_START_ADDR + 0x4 ,(uint32_t )&do_startup);
    NVIC_hal_writeRegU32( CONFIG_RAM_START_ADDR + 0x8 ,(uint32_t )NMI_Isr);
    NVIC_hal_writeRegU32( CONFIG_RAM_START_ADDR + 0xc ,(uint32_t )HardFault_Isr);

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);



}



