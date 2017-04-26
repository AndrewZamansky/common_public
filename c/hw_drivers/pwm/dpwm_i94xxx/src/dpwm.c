#include <string.h> //for memset
#include "ISD94XXXSeries.h"
#include "sys.h"
#include "clk.h"
#include "dpwm.h"

//#define PRINT_FROM_DRIVER 1

#if PRINT_FROM_DRIVER
	#include <stdio.h>
	#define DRV_PRINT printf
#else
	#define DRV_PRINT(...)
#endif	

// Creates a mask of a range of bits (field)
#define FIELD(firstBit, lastBit)    ((((1UL << ((lastBit) - (firstBit) + 1UL)) - 1UL)) << (firstBit))

// Creates a mask of a single bit
#define BIT(bit)                        (1UL << (bit))

// Write a variable's specific bits (field) with a value
#define SET_FIELD(var, firstBit, lastBit, data) (var = (((var) & ~(FIELD((firstBit), (lastBit))) ) | ((data) << (firstBit))))

// Write a variable's specific single bit with 1 or 0
#define SET_BIT(var, bitNum, data)  (SET_FIELD((var), (bitNum), (bitNum), (data)))

// Reads a variable's specific bits (field)
#define GET_FIELD(var, firstBit, lastBit)   (( (var) & FIELD((firstBit), (lastBit))) >> (firstBit))

// Read a variable's specific bit
#define GET_BIT(var, bitNum)        (GET_FIELD((var), (bitNum), (bitNum)) & 1UL)


#define ABS(x) (((x)>0) ? (x) : -(x))

/* 
	number of coefficients per filtering stage is 5:
	as following: b0, b1, b2, a1, a2
	in this methodology a0 is assumed to be 1
*/
#define NUM_OF_COEFFS_PER_STAGE	5

typedef struct
{
	DPWM_FILTER_SPLITER_MODE SplitterFilterMode;
	uint32_t FilterStages;
	uint32_t SplitterStages;
	int FilterInitDone;
	int SplitterInitDone;
}DPWM_RUNTIME_T;

static DPWM_RUNTIME_T RuntimeParams = {DPWM_FILTER_SPLITER_BOTH_OFF, 0, 0};

int32_t DPWM_MuxPins(DPWM_CHAN_E chan, DPWM_IO_PORT_E ioPort)
{
		
	switch (chan)
  {
  	case DPWM_CHAN_LEFT:
			if (ioPort == DPWM_IO_ON_GPA)
			{
				_SYS_GPA_MFPL_PA5(SYS_GPA_MFPL_PA5MFP_DPWM_PLUS_L);
				_SYS_GPA_MFPL_PA4(SYS_GPA_MFPL_PA4MFP_DPWM_MINUS_L);
			}
			else if (ioPort == DPWM_IO_ON_GPC)
			{
				_SYS_GPC_MFPH_PC13(SYS_GPC_MFPH_PC13MFP_DPWM_PLUS_L);
				_SYS_GPC_MFPH_PC12(SYS_GPC_MFPH_PC12MFP_DPWM_MINUS_L);
			}
			else if (ioPort == DPWM_IO_ON_GPD)
			{
				_SYS_GPD_MFPL_PD1(SYS_GPD_MFPL_PD1MFP_DPWM_PLUS_L);
				_SYS_GPD_MFPL_PD0(SYS_GPD_MFPL_PD0MFP_DPWM_MINUS_L);
			}
  		break;
  	case DPWM_CHAN_RIGHT:
			if (ioPort == DPWM_IO_ON_GPA)
			{
				_SYS_GPA_MFPH_PA10(SYS_GPA_MFPH_PA10MFP_DPWM_PLUS_R);
				_SYS_GPA_MFPH_PA9(SYS_GPA_MFPH_PA9MFP_DPWM_MINUS_R);
			}
			else if (ioPort == DPWM_IO_ON_GPC)
			{
				_SYS_GPC_MFPH_PC11(SYS_GPC_MFPH_PC11MFP_DPWN_PLUS_R);
				_SYS_GPC_MFPH_PC10(SYS_GPC_MFPH_PC10MFP_DPWN_MINUS_R);
			}
			else if (ioPort == DPWM_IO_ON_GPD)
			{
				_SYS_GPD_MFPL_PD6(SYS_GPD_MFPL_PD6MFP_DPWN_PLUS_R);
				_SYS_GPD_MFPL_PD5(SYS_GPD_MFPL_PD5MFP_DPWN_MINUS_R);
			}			
  		break;
	  case DPWM_CHAN_SUB:
			if (ioPort == DPWM_IO_ON_GPA)
			{
//				_SYS_GPA_MFPH_PA14(SYS_GPA_MFPH_PA14MFP_DPWM_PLUS_S);
//				_SYS_GPA_MFPH_PA13(SYS_GPA_MFPH_PA13MFP_DPWM_MINUS_S);
			}
			else if (ioPort == DPWM_IO_ON_GPC)
			{
				_SYS_GPC_MFPH_PC15(SYS_GPC_MFPH_PC15MFP_DPWM_PLUS_S);
				_SYS_GPC_MFPH_PC14(SYS_GPC_MFPH_PC14MFP_DPWM_MINUS_S);
			}
			else if (ioPort == DPWM_IO_ON_GPD)
			{
				_SYS_GPD_MFPH_PD9(SYS_GPD_MFPH_PD9MFP_DPWM_PLUS_S);
				_SYS_GPD_MFPH_PD8(SYS_GPD_MFPH_PD8MFP_DPWM_MINUS_S);
			}
  		break;		
  	default:
			return RC_DPWM_OUT_OF_RANGE;
  }

	return RC_DPWM_OK;
}

uint32_t DPWM_GetSourceClk( void )
{
	uint32_t clkSel, retVal;
	
	clkSel = (CLK->CLKSEL2 & CLK_CLKSEL2_DPWMSEL_Msk);
	
	switch (clkSel)
	{
		case CLK_CLKSEL2_DPWMSEL_HXT:
			retVal = __HXT;
		break;
		
		case CLK_CLKSEL2_DPWMSEL_PLL:
			retVal = CLK_GetPLLClockFreq();
		break;
		
		case CLK_CLKSEL2_DPWMSEL_PCLK0:
			retVal = CLK_GetPCLK0Freq();
		break;
		
		case CLK_CLKSEL2_DPWMSEL_HIRC:
			retVal = __HIRC;
		break;
		
		default:
			return 0;
	}
	
	return retVal;
}



int32_t	DPWM_Open(uint32_t samplingRateHz, DPWM_DATA_FORMAT_E format, DPWM_FIFO_WORD_WIDTH width)
{
	uint32_t zohDiv, clkDiv, k, actualSamplingRate, moduleClk, targetClk;
	DPWM_CLK_TYPE_E clkType;
	memset(&RuntimeParams,0x00,sizeof(RuntimeParams));
	
	//CLK_EnableModuleClock(DPWM_MODULE);
	
	/* Reset Module */
	//SYS_ResetModule(DPWM_RST);
	
	
	/* get clock type according to module clock */
	moduleClk = DPWM_GetSourceClk();
	
	/* calculate dividers */
	if ( (moduleClk&0x3FF) == 0) // reference clock is a mulltiple of 1024
	{
		clkType = DPWM_CLK_512_TIMES_FS;
		k = 128;
		//k = 64; // fixing dpwm switching freq issue
		targetClk = (samplingRateHz > 48000) ? (48 * 1024 * 1000) : (24 * 1024 * 1000);
		//targetClk = (samplingRateHz > 48000) ? (48 * 1024 * 1000) : (12 * 1024 * 1000);
		
	}
	else
	{
		clkType = DPWM_CLK_500_TIMES_FS;
		k = 125;
		targetClk = (samplingRateHz > 48000) ? (48 * 1000 * 1000) : (24 * 1000 * 1000);
	}
	
	clkDiv = moduleClk / targetClk - 1;
	
	zohDiv = (moduleClk / (clkDiv + 1)) / (samplingRateHz * k);
	
	if (zohDiv < 4)
	{
		DRV_PRINT("WARNING! ZOH_DIV < 4 (%u)\n",zohDiv);
		actualSamplingRate = 0;
		
	}
	
	
	{
		actualSamplingRate = moduleClk / (clkDiv + 1) / zohDiv / k;
	}

	
	/* configure clock according to required SR and module clock */
	
		
	if (actualSamplingRate == 0)
	{
		return RC_DPWM_OUT_OF_RANGE;
	}
	
	DPWM->ZOHDIV = (zohDiv << DPWM_ZOHDIV_ZOHDIV_Pos) | (clkDiv << DPWM_ZOHDIV_DPWM_CLK_DIV_Pos);
	SET_BIT(DPWM->CTL, DPWM_CTL_DPWM_CLKSET_Pos, clkType);

	
	/* set data format and width*/
	SET_BIT(DPWM->CTL, DPWM_CTL_FLT_EN_Pos, format);
	if (format == DPWM_FORMAT_FIXED)
	{
		SET_FIELD(DPWM->CTL, DPWM_CTL_FIFOWIDTH_Pos, DPWM_CTL_FIFOWIDTH_Pos + 1, width);
	}
	else
	{
		SET_BIT(DPWM->CTL, DPWM_CTL_FLT_EN_Pos, 1);
	}
	
		
	/*enable module and driver (TC8226 does not have an actual driver)*/
	DPWM_ENABLE();
	
	DRV_PRINT("DPWM_Open -> actualSamplingRate = %u\n", actualSamplingRate);
	
	return RC_DPWM_OK;
}

void DPWM_BIQ_ConfigureCoeffs(uint32_t firstStageIdx, uint32_t numOfStages, void *pCoeffs, DPWM_DATA_FORMAT_E format)
{
	int i;
	uint32_t offset = firstStageIdx*NUM_OF_COEFFS_PER_STAGE;
	uint32_t *coeffs = (uint32_t *)pCoeffs;	
	volatile uint32_t *regs = (volatile uint32_t *)&DPWM->COEFF_0;
	
	// write the value of the COEFF_CTL explicitly to avoid readback issue in TC8236
	DPWM->COEFF_CTL = (format << DPWM_COEFF_CTL_COEFF_FLTEN_Pos) | DPWM_COEFF_CTL_PRGCOEFF_Msk;
	
	for (i = 0; i < numOfStages * NUM_OF_COEFFS_PER_STAGE; ++i)
	{
			regs[offset + i] = coeffs[i];
	}
	
	// write the value of the COEFF_CTL explicitly to avoid readback issue in TC8236
	DPWM->COEFF_CTL = (format << DPWM_COEFF_CTL_COEFF_FLTEN_Pos);
}

int32_t DPWM_InitFilterSplitter(DPWM_FILTER_SPLITER_MODE mode, DPWM_DATA_FORMAT_E format, DPWM_STAGING_T *pFilterCoeffs, DPWM_STAGING_T *pSplitterCoeffs)
{
	/* First Disable both filter and splitter, and set format */
	SET_FIELD(DPWM->CTL, DPWM_CTL_BIQ_ON_Pos, DPWM_CTL_SPLT_ON_Pos, DPWM_FILTER_SPLITER_BOTH_OFF);
	SET_BIT(DPWM->COEFF_CTL, DPWM_COEFF_CTL_COEFF_FLTEN_Pos, format);
	
	switch (mode)
  {
  	case DPWM_FILTER_SPLITER_BOTH_OFF: // both BIQ and splitter are disabled
			RuntimeParams.SplitterFilterMode = mode;
			RuntimeParams.FilterStages = 0;
			RuntimeParams.FilterInitDone = FALSE;
			
			RuntimeParams.SplitterStages = 0;		
			RuntimeParams.SplitterInitDone = FALSE;
  		break;
  	
		case DPWM_FILTER_ONLY: // only BIQ filter is enabled
			/* check input validity */
			if ((pFilterCoeffs->NumOfStages > 10) || (pFilterCoeffs->pCoeffs == 0))
				return RC_DPWM_OUT_OF_RANGE;
			SET_FIELD(DPWM->CTL, DPWM_CTL_BIQ_BAND_NUM_Pos, DPWM_CTL_BIQ_BAND_NUM_Pos + 3, pFilterCoeffs->NumOfStages);
			DPWM_BIQ_ConfigureCoeffs(0, pFilterCoeffs->NumOfStages, pFilterCoeffs->pCoeffs, format);
			
			// update runtime params
			RuntimeParams.SplitterFilterMode = mode;
			
			RuntimeParams.FilterStages = pFilterCoeffs->NumOfStages;
			RuntimeParams.FilterInitDone = TRUE;
			
			RuntimeParams.SplitterStages = 0;		
			RuntimeParams.SplitterInitDone = FALSE;
			
  		break;
	
		case DPWM_SPLITTER_ONLY:
			/* check input validity */
			if ((pSplitterCoeffs->NumOfStages > 10) || (pSplitterCoeffs->pCoeffs == 0))
				return RC_DPWM_OUT_OF_RANGE;
			SET_FIELD(DPWM->CTL, DPWM_CTL_BIQ_BAND_NUM_Pos, DPWM_CTL_BIQ_BAND_NUM_Pos + 3, pSplitterCoeffs->NumOfStages);
			DPWM_BIQ_ConfigureCoeffs(0, pSplitterCoeffs->NumOfStages, pSplitterCoeffs->pCoeffs, format);
			// update runtime params
			RuntimeParams.SplitterFilterMode = mode;
			
			RuntimeParams.FilterStages = 0;
			RuntimeParams.FilterInitDone = FALSE;
			
			RuntimeParams.SplitterStages = pSplitterCoeffs->NumOfStages;		
			RuntimeParams.SplitterInitDone = TRUE;
			break;
		
		case DPWM_FILTER_AND_SPLITTER_ON:
  		
			if ((pSplitterCoeffs->NumOfStages + pFilterCoeffs->NumOfStages > 10) || 
					(pSplitterCoeffs->pCoeffs == 0) || 
					(pFilterCoeffs->pCoeffs == 0))
			{
				DRV_PRINT("DPWM_InitFilterSplitter: DPWM_OUT_OF_RANGE\n");
				return RC_DPWM_OUT_OF_RANGE;
			}
			
			SET_FIELD(DPWM->CTL, DPWM_CTL_BIQ_BAND_NUM_Pos, DPWM_CTL_BIQ_BAND_NUM_Pos + 3, pSplitterCoeffs->NumOfStages + pFilterCoeffs->NumOfStages);
			DPWM_BIQ_ConfigureCoeffs(0, pFilterCoeffs->NumOfStages, pFilterCoeffs->pCoeffs, format);
			DPWM_BIQ_ConfigureCoeffs(pFilterCoeffs->NumOfStages, pSplitterCoeffs->NumOfStages, pSplitterCoeffs->pCoeffs, format);
			
						// update runtime params
			RuntimeParams.SplitterFilterMode = mode;
			
			RuntimeParams.FilterStages = pFilterCoeffs->NumOfStages;
			RuntimeParams.FilterInitDone = TRUE;
			
			RuntimeParams.SplitterStages = pSplitterCoeffs->NumOfStages;		
			RuntimeParams.SplitterInitDone = TRUE;
			break;
		
		default:
			return RC_DPWM_OUT_OF_RANGE;
  }
	
	SET_FIELD(DPWM->CTL, DPWM_CTL_BIQ_ON_Pos, DPWM_CTL_SPLT_ON_Pos, mode);
	return RC_DPWM_OK;
}

int32_t DPWM_UpdateFilterCoeffs(DPWM_STAGING_T *pCoeff, DPWM_DATA_FORMAT_E format)
{
	
	/* input validation */
	if (!RuntimeParams.FilterInitDone || (pCoeff->pCoeffs == 0))
	{
		return RC_DPWM_NOT_INITIALIZED;
	}

	if (pCoeff->NumOfStages != RuntimeParams.FilterStages)
	{
		return RC_DPWM_NOT_SUPPORTED;
	}

	DPWM_BIQ_ConfigureCoeffs(0, pCoeff->NumOfStages, pCoeff->pCoeffs, format);
	
	return RC_DPWM_OK;
}

int32_t DPWM_UpdateSplitterCoeffs(DPWM_STAGING_T *pCoeff, DPWM_DATA_FORMAT_E format)
{
	uint32_t stageOffset;
	
	stageOffset = (RuntimeParams.FilterInitDone) ? RuntimeParams.FilterStages : 0;
	
	/* input validation */
	if (!RuntimeParams.SplitterInitDone || (pCoeff->pCoeffs == 0))
	{
		return RC_DPWM_NOT_INITIALIZED;
	}

	if (pCoeff->NumOfStages != RuntimeParams.SplitterStages)
	{
		return RC_DPWM_NOT_SUPPORTED;
	}

	DPWM_BIQ_ConfigureCoeffs(stageOffset, pCoeff->NumOfStages, pCoeff->pCoeffs, format);
	
	return RC_DPWM_OK;
}

int32_t DPWM_SetFifoInterruptThreshold(uint32_t threshold)
{
	if (threshold >= DPWM_FIFO_SIZE)
	{
		return RC_DPWM_OUT_OF_RANGE;
	}
	
	SET_FIELD(DPWM->CTL, DPWM_CTL_RXTH_Pos, DPWM_CTL_RXTH_Pos + 4, threshold);
	return RC_DPWM_OK;
}


void DPWM_InterruptEnable(int enable)
{
	SET_BIT(DPWM->CTL, DPWM_CTL_RXTHIE_Pos,enable);
	if (enable)
	{
		NVIC_EnableIRQ(DPWM_IRQn);
	}
	else
	{
		NVIC_DisableIRQ(DPWM_IRQn);
	}
}


void DPWM_Close( void )
{
	DPWM_DISABLE();
	NVIC_DisableIRQ(DPWM_IRQn);
	memset(&RuntimeParams,0x00,sizeof(RuntimeParams));
	/* Reset Module */
	SYS->IPRST2 |= SYS_IPRST2_DPWMRST_Msk;
	SYS->IPRST2 &= ~SYS_IPRST2_DPWMRST_Msk;
	
}
