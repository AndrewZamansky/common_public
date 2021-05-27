
#ifndef _Timer_API_H_
#define _Timer_API_H_


enum TimerPoleg_API_TWD_MODE_e {
	TWD_MODE_ONE_SHOT,
	TWD_MODE_PERIODIC,
	TWD_MODE_LAST,
};


enum TimerPoleg_API_CLK_SOURCE_e {
	TWD_SOURCE_PLL0 = 0,
	TWD_SOURCE_PLL1,
	TWD_SOURCE_CLKREF,
	TWD_SOURCE_PLL2_DIV2,
	TWD_LAST
};

typedef void (*Timer_Callback_t)(void);


uint8_t TimerPoleg_API_Init_No_Start(int TimerModule, int TimerPort,
		enum TimerPoleg_API_TWD_MODE_e twd_mode, int TickPerSecond,
		TimerPoleg_API_CLK_SOURCE_e clk_source,Timer_Callback_t afTimerCallback);
uint8_t TimerPoleg_API_Start(int TimerModule, int TimerPort);
void TimerPoleg_API_RestInterrupt(int TimerModule,int PortNum);

#endif
