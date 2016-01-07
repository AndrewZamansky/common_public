/*
 *
 * file :   biquads.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "common_dsp_config.h"
//#include "dev_managment_api.h" // for device manager defines and typedefs
//#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_common_dsp_prerequirements_check.h" // should be after {equalizer_config.h,dev_managment_api.h}


#include "common_dsp_api.h"


/********  defines *********************/


/********  types  *********************/

typedef struct
{
	void *pFilterParams;
}biquads_cascading_filter_t;

typedef struct
{
	uint8_t numOfStages;
	float *  pCoeffs;
	float *  pStates; // should be allocated numOfStages*4
}sw_biquads_params_t;


#ifdef _USE_DSP_
  #include "arm_math.h"
	#define NUM_OF_STATES_PER_STAGE	2
#else
	#define NUM_OF_STATES_PER_STAGE	4
#endif
/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

void biquads_cascading_filter(void *pFilter,float *apIn,float *apOut,size_t buff_len)
{
#ifdef _USE_DSP_
	arm_biquad_cascade_df2T_instance_f32 *filter_params;
	filter_params = ((arm_biquad_cascade_df2T_instance_f32*)(((biquads_cascading_filter_t *)pFilter)->pFilterParams));
	arm_biquad_cascade_df2T_f32(filter_params,apIn,apOut ,buff_len );
//	arm_biquad_casd_df1_inst_f32 *filter_params;
//	filter_params = ((arm_biquad_casd_df1_inst_f32*)(((biquads_cascading_filter_t *)pFilter)->pFilterParams));
//	arm_biquad_cascade_df1_f32(filter_params,apIn,apOut ,buff_len );
#else

	sw_biquads_params_t *filter_params;
	float *pState,*pStates;
	uint8_t currStage,numOfStages ;
	float *pCoeffs;
	float *currStageCoeffs;

	short count	;
	float state0,state1,state2;
	float pAi1,pAi2,pBi0,pBi1,pBi2;
	float pOutTmp;
	float *pIn;
	float *pOut;


	filter_params = (sw_biquads_params_t*)((biquads_cascading_filter_t *)pFilter)->pFilterParams;
	pCoeffs = filter_params->pCoeffs;
	numOfStages = filter_params->numOfStages;
	pStates = filter_params->pStates;

	state0=0;
	for (currStage = 0 ; currStage < numOfStages ; currStage++)
	{
		pIn=apIn;
		pOut=apOut;
		pState = &pStates[currStage * NUM_OF_STATES_PER_STAGE];
		currStageCoeffs = &pCoeffs[5*currStage];

		state1 = pState[1];
		state2 = pState[2];

		pBi0 = currStageCoeffs[0];
		pBi1 = currStageCoeffs[1];
		pBi2 = currStageCoeffs[2];
		pAi1 = currStageCoeffs[3];
		pAi2 = currStageCoeffs[4];
	//			for(ChanCount = 0 ; ChanCount < u16ChannelNum; ChanCount++)
	//		*pState  = *pAi * in	;
		for(count = 0 ; count < buff_len; count++)
		{
	#if 1
			state0  = *pIn	; // due to *pAi must be "1" always
			state0 -= pAi1 * (state1)		;
			state0 -= pAi2 * (state2)		;
			pOutTmp = pBi0 * state0;
			pOutTmp += pBi1 * (state1)	;
			pOutTmp += pBi2 * (state2)	;
	#else
			//pOutTmp = pBi0 * (*pin);
			pOutTmp = (*pIn);

	#endif

			state2 = state1	;
			state1 = state0	;
			*pOut = pOutTmp;
			pIn++;
			pOut++;
		}

		pState[0] = state0;
		pState[1] = state1;
		pState[2] = state2;

	}
#endif
}




/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, float *  	pCoeffs )
{
	void *tmp;
	int tmp1;
	biquads_cascading_filter_t *p_biquads_cascading_filter;
	p_biquads_cascading_filter=(biquads_cascading_filter_t *)malloc(sizeof(biquads_cascading_filter_t));
#ifdef _USE_DSP_
	tmp1 = sizeof(arm_biquad_cascade_df2T_instance_f32);
	tmp = malloc(tmp1);
	p_biquads_cascading_filter->pFilterParams = (arm_biquad_cascade_df2T_instance_f32*) tmp;
//	tmp = malloc(sizeof(arm_biquad_casd_df1_inst_f32));
//	p_biquads_cascading_filter->pFilterParams = (arm_biquad_casd_df1_inst_f32*) tmp;
	tmp =malloc(	NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
//	memset(tmp , 0 , NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
	arm_biquad_cascade_df2T_init_f32(p_biquads_cascading_filter->pFilterParams,num_of_stages,
			pCoeffs , (float *) tmp );
//	arm_biquad_cascade_df1_init_f32(p_biquads_cascading_filter->pFilterParams,num_of_stages,
//			pCoeffs , (float *) tmp );
#else
	p_biquads_cascading_filter->pFilterParams =
				(sw_biquads_params_t *)malloc(	sizeof(sw_biquads_params_t));
	((sw_biquads_params_t*)p_biquads_cascading_filter->pFilterParams)->numOfStages = num_of_stages;
	((sw_biquads_params_t*)p_biquads_cascading_filter->pFilterParams)->pCoeffs = pCoeffs;
	tmp = malloc(NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
	memset(tmp  , 0 , NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
	((sw_biquads_params_t*)(p_biquads_cascading_filter->pFilterParams))->pStates=tmp;
#endif

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter,uint8_t num_of_stages, float *  	pCoeffs )
{
#ifdef _USE_DSP_
	free(  (   (arm_biquad_cascade_df2T_instance_f32*)((biquads_cascading_filter_t *)pFilter)->pFilterParams)->pState  );
#else
	free(  (   (sw_biquads_params_t*)((biquads_cascading_filter_t *)pFilter)->pFilterParams)->pStates  );
#endif
	free(((biquads_cascading_filter_t *)pFilter)->pFilterParams);
	free(pFilter);
}



/*   func : biquads_calculation()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_calculation(biquads_filter_mode_t filter_mode,
		float FreqC,float QValue,float Gain_dB,
		float SamplingRate, float *pCoeffs )
{
	float norm,V,K	;
	float ftem	;
	float b0,b1,b2,a1,a2,w0;

	b0 = 1.0	;

	ftem = FreqC/SamplingRate	;
    w0 = 2 * 3.1415962 * ftem;
	K = tan(3.1415962 * ftem)	;
	ftem = (Gain_dB >= 0) ? Gain_dB : -Gain_dB	;
	V = pow(10,ftem/20)	;


	switch(filter_mode)
	{
		default :
		case BIQUADS_TRANSPARENT_MODE :
			a1 = a2 = b1 = b2 = 0	;
			break	;

		case BIQUADS_LOWPASS_MODE_1_POLE :
            a1 = -exp(-w0);
            a2 = 0;
            b0 = 1 + a1;
            b1 = 0;
            b2 = 0;
			break;

		case BIQUADS_LOWPASS_MODE_2_POLES :
			norm = 1/(1 + K/QValue + K * K)	;
			b0 = K * K * norm	;
			b1 = 2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K/QValue + K * K) * norm	;
			break;

		case BIQUADS_HIGHPASS_MODE_1_POLE :
            a1 = -exp(-w0);
            a2 = 0;
            b0 = (1 - a1) / 2;
            b1 = -b0;
            b2 = 0;
			break;

		case BIQUADS_HIGHPASS_MODE_2_POLES :
			norm = 1/(1 + K/QValue + K * K)	;
			b0 = norm	;
			b1 = -2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K/QValue + K * K) * norm	;
			break;

		case BIQUADS_BANDPASS_MODE :
			norm = 1/(1 + K/QValue + K * K)	;
			b0 = K/QValue *	norm;
			b1 = 0	;
			b2 = -b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K/QValue + K * K) * norm	;
			break;

		case BIQUADS_PEAK_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K/QValue + K * K)	;
				b0 = (1 + V/QValue * K + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - V/QValue * K + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - K/QValue + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + V/QValue * K + K * K)	;
				b0 = (1 + K/QValue + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K/QValue + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - V/QValue * K + K * K) * norm	;
			}
			break;

		case BIQUADS_LOWSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + sqrt(2) * K + K * K)	;
				b0 = (1 + sqrt(2*V) * K + V * K * K) * norm	;
				b1 = 2 * (V * K * K - 1) * norm	;
				b2 = (1 - sqrt(2*V) * K + V * K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - sqrt(2) * K + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + sqrt(2*V) * K + V * K * K)	;
				b0 = (1 + sqrt(2) * K + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - sqrt(2) * K + K * K) * norm	;
				a1 = 2 * (V * K * K - 1) * norm	;
				a2 = (1 - sqrt(2*V) * K + V * K * K) * norm	;
			}
			break;

		case BIQUADS_HIGHSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + sqrt(2) * K + K * K)	;
				b0 = (V + sqrt(2*V) * K + K * K) * norm	;
				b1 = 2 * (K * K - V) * norm	;
				b2 = (V - sqrt(2*V) * K + K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - sqrt(2) * K + K * K) * norm	;
			}
			else
			{
				norm = 1/(V + sqrt(2*V) * K + K * K)	;
				b0 = (1 + sqrt(2) * K + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - sqrt(2) * K + K * K) * norm	;
				a1 = 2 * (K * K - V) * norm	;
				a2 = (V - sqrt(2*V) * K + K * K) * norm	;
			}
			break;
	}
#ifdef _USE_DSP_
	a1 = -a1;
	a2 = -a2;
#endif
	pCoeffs[0] = b0;
	pCoeffs[1] = b1;
	pCoeffs[2] = b2;
	pCoeffs[3] = a1;
	pCoeffs[4] = a2;
}
