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



/********  defines *********************/



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
    NVIC_hal_writeRegU32( NVIC_CONFIG_START_OF_RAM + (((int)int_num + 16) << 2),(unsigned int)pIsr);// ( int_num  ) * 4
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






void  NVIC_API_Init(void)
{

	int8_t  i;



	// get number of external interrupts
	i= SCnSCB->ICTR ;
    /*
     * Disable all interrupts.
     */

    while ( i >= 0 )
	{
    	NVIC->ICER[i] = 0xffffffff;
    	NVIC->ICPR[i] = 0xffffffff;

        i--;
    }
#if (CORTEX_M_TYPE == 3) && (__CM3_REV < 0x0201)  /* core<r2p1 */
    SCB->VTOR = NVIC_CONFIG_START_OF_RAM & SCB_VTOR_TBLBASE_Msk;// set base to start of RAM
#else
    // make sure that in cortex-m3 with revision < r2p1 bit 29 is 1
    SCB->VTOR = NVIC_CONFIG_START_OF_RAM;
#endif

    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);



}



