/*
 *
 *   file  :  TimerPoleg.cpp
 *
 */



/***************   includes    *******************/
#include "global_typedefs.h"
//#include "Drivers_APP_HAL/TimerPoleg/TimerPoleg_hal.h"
#include "TimerPoleg_API.h"
#include "TimerPoleg.h"


/***************   defines    *******************/

/***************   typedefs    *******************/

#define TimerPoleg_hal_writeRegU32(reg,val)  *((volatile uint32_t *)reg)=val

/**********   external variables    **************/



/***********   local variables    **************/



//---------------------------------------------------------------
//
//----------------------------------------------------------------


//----------------------------------------------------------------
void TimerPoleg_API_RestInterrupt(int TimerModule,int PortNum)
{

	TimerPoleg_hal_writeRegU32(TISR(TimerModule),0x1F);

}



uint8_t TWD_DisableTimer(int TimerModule,int PortNum)
{

	if (PortNum < 0)
	{
	//	LogError("TWD_DisableTimer: Illegal Module Number %d should 0..4\n",PortNum);
		return 1;
	}
	if (TimerModule < 1)
	{
	//	LogError("TWD_DisableTimer: Illegal Module Number %d (should be 1..3)\n",TimerModule);
		return 1;
	}


	if (TimerModule > 3)
	{
		//LogError("TWD_DisableTimer: Illegal TImer Module Number %d (should be 1..3) \n",TimerModule);
		return 1;
	}
	if (PortNum > 4)
	{
	//	LogError("TWD_DisableTimer: Illegal TImer Module Number %d (should be 1..3) \n",PortNum);
		return 1;
	}

	TimerPoleg_hal_writeRegU32(TCSR(PortNum,TimerModule), 0);
	TimerPoleg_hal_writeRegU32(TISR(3),0x1F); // clear interrupt in timer
	int IntNum = TIMER_INT_0 + (TimerModule * PortNum);

	TimerPoleg_hal_RegisterInterrupt(IntNum,NULL);

	TimerPoleg_hal_DisableInt(IntNum);
	return 0;

}


uint8_t TWD_DisableTimers()
{
	int tnum,mnum;
	for ( tnum = 1; tnum <=3; tnum++)
	{
		for ( mnum = 0; mnum <= 4; mnum++)
		{
			TWD_DisableTimer(mnum,tnum);
		}
	}


	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TWD_1_ISR                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                             */
/*                                                                                                         */
/* Returns:                                                                                           */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TWD_1_ISR()
{
	static int i=0;
	//--------------------------------------------------------
	//		Clear Pending interrupts
	//-----------------------------------------------------------

	//FreeRTOS_Tick_Handler();
	TimerPoleg_hal_writeRegU32(TISR_1,0x1F);
//	PRINTF("tick=%d\n",i++);

}


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
uint8_t TimerPoleg_API_Init_No_Start(int TimerModule, int TimerPort,
		TimerPoleg_API_TWD_MODE_t twd_mode,int TickPerSecond,
		TimerPoleg_API_CLK_SOURCE_t clk_source,Timer_Callback_t afTimerCallback)
{

	//------------------------------------------------
	//	Check Validity
	//------------------------------------------------
	uint32_t ReadReg = 0;
//	LogMessage("Timer_Init TimerModule %d TimerPort %d twd_mode %d TickPerSecond %d clk_source %d\n",TimerModule,TimerPort,twd_mode,TickPerSecond,clk_source);
	if (TimerPort < 0)
	{
	//	LogError("Timer_Init_No_Start: Illegal Module Number %d should 0..4\n",TimerPort);
		return 1;
	}
	if (TimerModule < 1)
	{
	//	LogError("Timer_Init_No_Start: Illegal Module Number %d (should be 1..3)\n",TimerModule);
		return 1;
	}


	if (TimerModule > 3)
	{
	//	LogError("Timer_Init_No_Start: Illegal TImer Module Number %d (should be 1..3) \n",TimerModule);
		return 1;
	}
	if (TimerPort > 4)
	{
	//	LogError("Timer_Init_No_Start: Illegal TImer Module Number %d (should be 1..3) \n",TimerPort);
		return 1;
	}
	//--------------------------------------------------------
	//		Disable Timer
	//-----------------------------------------------------------
	TWD_DisableTimer(TimerModule,TimerPort);

	//-----------------------------------------------------------
	//
	//------------------------------------------------------------

	ReadReg = TimerPoleg_hal_readRegU32(CLKSEL);
	//LogMessage("Read CLKDIV  0x%X\n",ReadReg);
	ReadReg &=  ~CLKSEL_TIMCKSEL_MASK;
	ReadReg |= (clk_source << 14);
	TimerPoleg_hal_writeRegU32(CLKSEL,ReadReg);

	// Set clock divider to 1
	ReadReg = TimerPoleg_hal_readRegU32(CLKDIV1);
	ReadReg &=  ~CLKDIV1_TIMCKDIV_MASK;
	ReadReg |= (0 << 21); // Set clock divider to 1
	TimerPoleg_hal_writeRegU32(CLKDIV1,ReadReg);

	//--------------------------------------------------------
	//		Clear Pending interrupts
	//-----------------------------------------------------------

	TimerPoleg_hal_writeRegU32(TISR(TimerModule),(1<<TimerPort));

	//--------------------------------------------------
	//  TICR Setting
	//--------------------------------------------------

	if (TickPerSecond <= 0)
	{
	//	LogError("Bad tick period: %d\n",TickPerSecond);
		return 1;
	}
	TimerPoleg_hal_writeRegU32(TICR(TimerPort,TimerModule),TimerPoleg_hal_InputClock() / (TickPerSecond));

	//--------------------------------------------------
	//  TCSR Setting
	//--------------------------------------------------

	ReadReg = TimerPoleg_hal_readRegU32(TCSR(TimerPort,TimerModule));
//	LogMessage("Inital TCSR %d %d is 0x%X\n",TimerModule,TimerPort,ReadReg);
	ReadReg &= ~TCSR_MODE;

	switch (twd_mode)
	{
	case TWD_MODE_ONE_SHOT:
		ReadReg |= TCSR_MODE_ONE_SHOT;
		break;
	case TWD_MODE_PERIODIC:
		ReadReg |= TCSR_MODE_PERIODIC;
		break;
	default:
			//LogError("Bad TWD mode %d\n",twd_mode);
	break;

	}
	// Make sure timer is not counting yet!
	ReadReg &= ~TCSR_CEN;
	ReadReg |= TCSR_IEN;

	//LogMessage("TCSR CFG %d %d is 0x%X\n",TimerModule,TimerPort,ReadReg);
	TimerPoleg_hal_writeRegU32(TCSR(TimerPort,TimerModule),ReadReg);

	int IntNum = TIMER_INT_0 + (TimerModule * TimerPort);
	//TimerPoleg_hal_RegisterInterrupt(IntNum,afTimerCallback);
	TimerPoleg_hal_RegisterInterrupt(IntNum,TWD_1_ISR);

	TimerPoleg_hal_EnableInt(IntNum);
	return 0;
}

uint8_t TimerPoleg_API_Start(int TimerModule, int TimerPort)
{
	uint32_t ReadReg = 0;

	// Clear all interrups. again.
	TimerPoleg_hal_writeRegU32(TISR(TimerModule),(1<<TimerPort));


	ReadReg = TimerPoleg_hal_readRegU32(TCSR(TimerPort,TimerModule));
	ReadReg |= TCSR_CEN;
	//LogMessage("TCSR Start %d %d is 0x%X\n",TimerModule,TimerPort,ReadReg);
	TimerPoleg_hal_writeRegU32(TCSR(TimerPort,TimerModule),ReadReg);

	return 0;
}



