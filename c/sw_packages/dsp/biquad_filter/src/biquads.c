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

#include "_project.h"
#include "_biquad_filter_prerequirements_check.h"
#include "common_dsp_api.h"
#include "biquad_filter.h"
#include "biquad_filter_api.h"

#define NUM_OF_STATES_PER_STAGE	2

/********  defines *********************/


/********  types  *********************/

struct biquads_cascading_filter_t
{
	uint8_t numOfStages;
	float *  pCoeffs;
	float *  pStates; // should be allocated numOfStages*4
};



/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

void biquads_cascading_filter(void *pFilter,float *apIn,float *apOut,size_t buff_len)
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;
	float *pState;
	float *pStates;
	uint8_t currStage;
	uint8_t numOfStages ;
	float *pCoeffs;
	float *currStageCoeffs;

	short count	;
	float state1,state2;
	float pAi1,pAi2,pBi0,pBi1,pBi2;
	float pOutTmp;
	float curr_x;
	float tmp;


	p_biquads_cascading_filter = pFilter;
	pCoeffs = p_biquads_cascading_filter->pCoeffs;
	numOfStages = p_biquads_cascading_filter->numOfStages;
	pStates = p_biquads_cascading_filter->pStates;
	pOutTmp=0;
	for(count = 0 ; count < buff_len; count++)
	{
		curr_x = *apIn++;
		for (currStage = 0 ; currStage < numOfStages ; currStage++)
		{
			pState = &pStates[currStage * NUM_OF_STATES_PER_STAGE];
			currStageCoeffs = &pCoeffs[5*currStage];

			state1 = pState[0];
			state2 = pState[1];

			pBi0 = currStageCoeffs[0];
			pBi1 = currStageCoeffs[1];
			pBi2 = currStageCoeffs[2];
			pAi1 = currStageCoeffs[3];
			pAi2 = currStageCoeffs[4];

			pOutTmp = pBi0 * curr_x ;
			pOutTmp +=state1;
			state1 = pBi1 * curr_x;
			tmp	=  pAi1 * pOutTmp;
			state1 -=tmp;
			state1 +=state2;
			state2 = pBi2 * curr_x;
			tmp =  pAi2 * pOutTmp;
			state2 -=tmp;

			pState[0] = state1;
			pState[1] = state2;
			curr_x = pOutTmp;
		}
		*apOut++ = pOutTmp;
	}

}




/*   func : biquads_alloc()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void *biquads_alloc(uint8_t num_of_stages, float *pCoeffs )
{
	biquads_cascading_filter_t *p_biquads_cascading_filter;
	void *pStates;
	size_t size_of_states;

	p_biquads_cascading_filter =(biquads_cascading_filter_t *)malloc(
			sizeof(biquads_cascading_filter_t));

	p_biquads_cascading_filter->pFilterParams =
				(sw_biquads_params_t *)malloc(	sizeof(sw_biquads_params_t));

	size_of_states = NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float);
	pStates = malloc(size_of_states);
	memset(pStates, 0, size_of_states);

	p_biquads_cascading_filter->pStates = pStates;
	p_biquads_cascading_filter->numOfStages = num_of_stages;
	p_biquads_cascading_filter->pCoeffs = pCoeffs;

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter,uint8_t num_of_stages, float *  	pCoeffs )
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;

	p_biquads_cascading_filter = pFilter;
	free( p_biquads_cascading_filter->pStates  );
	free(p_biquads_cascading_filter);
}



/*   func : biquads_calculation()
 *    The coefficients are stored in the array pCoeffs in the following order:
 *
 *     y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 */
void biquads_coefficients_calculation(biquads_filter_mode_t filter_mode,
		float FreqC,float QValue,float Gain_dB,
		float SamplingRate, float *pCoeffs )
{
	biquads_coefficients_calculation_common(filter_mode,
			 FreqC, QValue, Gain_dB, SamplingRate,  pCoeffs);
}
