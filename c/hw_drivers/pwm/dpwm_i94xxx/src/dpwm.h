#ifndef DPWM_H
#define DPWM_H

#include "ISD94XXXSeries.h"

#define DPWM_FIFO_SIZE (16)

typedef enum 
{
	RC_DPWM_OK = 0,
	RC_DPWM_OUT_OF_RANGE = 0x01,
	RC_DPWM_NOT_INITIALIZED = 0x02,
	RC_DPWM_NOT_SUPPORTED = 0x03,
}RC_DPWM_DRV;


typedef enum
{
	DPWM_CHAN_LEFT 	= 	0,
	DPWM_CHAN_RIGHT = 	1,
	DPWM_CHAN_SUB		=		2,
	NUM_OF_DPWM_CHANNELS
}DPWM_CHAN_E;


typedef enum 						//					LFET									RIGHT									SUB
{												//			PLUS 		MINUS					PLUS		MINUS					PLUS		MINUS		
	DPWM_IO_ON_GPA = 0,		//			GPA05		GPA04					GPA10		GPA09					GPA14		GPA13
	DPWM_IO_ON_GPC,				//			GPC13		GPC12					GPC11		GPC10  				GPC15		GPC14
	DPWM_IO_ON_GPD,				//			GPD01		GPD00					GPD06		GPD05					GPD09		GPD08
	NUM_OF_DPWM_IO_PORTS
}DPWM_IO_PORT_E;

typedef enum
{
	DPWM_WIDTH_24BIT_MSB = 0,	// PDMA MSB 24bits PWDATA[31:8]
	DPWM_WIDTH_16BIT = 1,			// PDMA 16 bits PWDATA[15:0] 
	DPWM_WIDTH_8BIT = 2,			// PDMA 8bits PWDATA[7:0]
	DPWM_WIDTH_24BIT_LSB = 3,	// PDMA 24bits PWDATA[23:0]
}DPWM_FIFO_WORD_WIDTH;

typedef enum
{
	DPWM_FORMAT_FIXED,
	DPWM_FORMAT_FLOATING_PNT,
}DPWM_DATA_FORMAT_E;

typedef enum
{
	DPWM_CLK_512_TIMES_FS = 0,
	DPWM_CLK_500_TIMES_FS = 1,
}DPWM_CLK_TYPE_E;

typedef enum
{
	DPWM_FILTER_SPLITER_BOTH_OFF 	= 0, //DPWM->CTL[22:21] = 00
	DPWM_FILTER_ONLY							= 1, //DPWM->CTL[22:21] = 01
	DPWM_SPLITTER_ONLY						= 2, //DPWM->CTL[22:21] = 10
	DPWM_FILTER_AND_SPLITTER_ON		= 3, //DPWM->CTL[22:21] = 11
}DPWM_FILTER_SPLITER_MODE;

typedef struct 
{
	uint32_t 	NumOfStages;
	void 			*pCoeffs;	
}DPWM_STAGING_T;

/* enable module and driver (TC8226 does not have an actual driver)*/
#define DPWM_ENABLE()				(DPWM->CTL |= DPWM_CTL_DPWMEN_Msk | DPWM_CTL_DWPMDRVEN_Msk)
#define DPWM_DISABLE()			(DPWM->CTL &= ~ (DPWM_CTL_DPWMEN_Msk | DPWM_CTL_DWPMDRVEN_Msk))

#define DPWM_DMA_ENABLE() 	(DPWM->DMACTL |= DPWM_DMACTL_DMAEN_Msk)
#define DPWM_DMA_DISABLE() 	(DPWM->DMACTL &= !DPWM_DMACTL_DMAEN_Msk)

#define DPWM_FIFO_FULL()		((DPWM->STS & DPWM_STS_FULL_Msk) == DPWM_STS_FULL_Msk)
#define DPWM_FIFO_EMPTY()		((DPWM->STS & DPWM_STS_EMPTY_Msk) == DPWM_STS_EMPTY_Msk)


int32_t DPWM_MuxPins(DPWM_CHAN_E chan, DPWM_IO_PORT_E ioPort);
int32_t	DPWM_Open(uint32_t samplingRateHz, DPWM_DATA_FORMAT_E format, DPWM_FIFO_WORD_WIDTH width);
void DPWM_Close( void );

int32_t DPWM_InitFilterSplitter(DPWM_FILTER_SPLITER_MODE mode, DPWM_DATA_FORMAT_E format, DPWM_STAGING_T *pFilterCoeffs, DPWM_STAGING_T *pSplitterCoeffs);
int32_t DPWM_UpdateSplitterCoeffs(DPWM_STAGING_T *pCoeff, DPWM_DATA_FORMAT_E format);
int32_t DPWM_UpdateFilterCoeffs(DPWM_STAGING_T *pCoeff, DPWM_DATA_FORMAT_E format);
int32_t DPWM_SetFifoInterruptThreshold(uint32_t threshold);
void 		DPWM_InterruptEnable(int enable);


#endif //DPWM_H
