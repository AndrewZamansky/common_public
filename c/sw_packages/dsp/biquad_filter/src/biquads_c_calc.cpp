/*
 *
 * file :   biquads.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
extern "C" {
#include "errors_api.h"
#include "os_wrapper.h"
}
#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "biquad_filter.h"
#include "biquad_filter_api.h"


#define NUM_OF_STATES_PER_STAGE	2

struct biquads_cascading_filter_t
{
	uint8_t numOfStages;
	real_t *  pCoeffs;
	real_t *  pStates; // should be allocated numOfStages*4
};


void biquads_cascading_filter(void *pFilter,
		real_t *apIn, real_t *apOut, size_t buff_len)
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;
	real_t *pState;
	real_t *pStates;
	uint8_t currStage;
	uint8_t numOfStages ;
	real_t *pCoeffs;
	real_t *currStageCoeffs;

	uint16_t count	;
	real_t state1, state2;
	real_t pAi1, pAi2, pBi0, pBi1, pBi2;
	real_t pOutTmp;
	real_t curr_x;
	real_t tmp;


	p_biquads_cascading_filter = (struct biquads_cascading_filter_t  *)pFilter;
	pCoeffs = p_biquads_cascading_filter->pCoeffs;
	numOfStages = p_biquads_cascading_filter->numOfStages;
	pStates = p_biquads_cascading_filter->pStates;
	pOutTmp = (int16_t)0;
	for(count = 0; count < buff_len; count++)
	{
		curr_x = *apIn++;
		for (currStage = 0 ; currStage < numOfStages ; currStage++)
		{
			pState = &pStates[currStage * NUM_OF_STATES_PER_STAGE];
			currStageCoeffs = &pCoeffs[ 5 * currStage ];

			state1 = pState[0];
			state2 = pState[1];

			pBi0 = currStageCoeffs[0];
			pBi1 = currStageCoeffs[1];
			pBi2 = currStageCoeffs[2];
			pAi1 = currStageCoeffs[3];
			pAi2 = currStageCoeffs[4];

			pOutTmp = pBi0 * curr_x ;
			pOutTmp += state1;
			state1 = pBi1 * curr_x;
			tmp	=  pAi1 * pOutTmp;
			state1 -= tmp;
			state1 += state2;
			state2 = pBi2 * curr_x;
			tmp =  pAi2 * pOutTmp;
			state2 -= tmp;

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
void *biquads_alloc(uint8_t num_of_stages, real_t *pCoeffs )
{
	struct biquads_cascading_filter_t *p_biquads_cascading_filter;
	real_t *pStates;
	size_t size_of_states;

	p_biquads_cascading_filter =(struct biquads_cascading_filter_t *)malloc(
			sizeof(struct biquads_cascading_filter_t));
	errors_api_check_if_malloc_succeed(p_biquads_cascading_filter);

	size_of_states = NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(real_t);
	pStates = (real_t *)malloc(size_of_states);
	errors_api_check_if_malloc_succeed(pStates);
	memset(pStates, 0, size_of_states);

	p_biquads_cascading_filter->pStates = pStates;
	p_biquads_cascading_filter->numOfStages = num_of_stages;
	p_biquads_cascading_filter->pCoeffs = pCoeffs;

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter)
{
	struct biquads_cascading_filter_t  *p_biquads_cascading_filter;

	if (NULL == pFilter) return;

	p_biquads_cascading_filter = (struct biquads_cascading_filter_t  *)pFilter;
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
void biquads_coefficients_calculation(enum biquads_filter_mode_e filter_mode,
		real_t FreqC, real_t QValue, real_t Gain_dB,
		real_t SamplingRate, real_t *pCoeffs )
{
	biquads_coefficients_calculation_common(filter_mode,
			 FreqC, QValue, Gain_dB, SamplingRate,  pCoeffs);
}
