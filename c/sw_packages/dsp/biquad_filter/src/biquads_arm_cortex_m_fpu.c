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
#include "cpu_config.h"
#include "arm_math.h"
#include "biquad_filter_api.h"

#define NUM_OF_STATES_PER_STAGE	2

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



/********  externals *********************/


/********  local defs *********************/


/**********   external variables    **************/



/***********   local variables    **************/

void biquads_cascading_filter(void *pFilter,float *apIn,float *apOut,size_t buff_len)
{
	arm_biquad_cascade_df2T_instance_f32 *filter_params;
	filter_params = ((arm_biquad_cascade_df2T_instance_f32*)(((biquads_cascading_filter_t *)pFilter)->pFilterParams));
	arm_biquad_cascade_df2T_f32(filter_params,apIn,apOut ,buff_len );
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
	biquads_cascading_filter_t *p_biquads_cascading_filter;
	p_biquads_cascading_filter=(biquads_cascading_filter_t *)malloc(sizeof(biquads_cascading_filter_t));
	{
		arm_biquad_cascade_df2T_instance_f32* p_filter_instance;
		p_filter_instance =(arm_biquad_cascade_df2T_instance_f32*) malloc(sizeof(arm_biquad_cascade_df2T_instance_f32));
		p_biquads_cascading_filter->pFilterParams = p_filter_instance;
		tmp =malloc(	NUM_OF_STATES_PER_STAGE * num_of_stages * sizeof(float));
		arm_biquad_cascade_df2T_init_f32(p_filter_instance ,num_of_stages,
				pCoeffs , (float *) tmp );
	}

	return p_biquads_cascading_filter;
}


/*  func : biquads_free()
 */
void biquads_free(void *pFilter,uint8_t num_of_stages, float *  	pCoeffs )
{
	free(  (   (arm_biquad_cascade_df2T_instance_f32*)((biquads_cascading_filter_t *)pFilter)->pFilterParams)->pState  );
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
	float ftem , G_abs	;
	float b0,b1,b2,a1,a2,w0;
	float K_div_Q;

	b0 = 1.0	;

	ftem = FreqC/SamplingRate	;
    w0 = 2 * 3.1415962 * ftem;
	K = tan(3.1415962 * ftem)	;
	G_abs = (Gain_dB >= 0) ? Gain_dB : -Gain_dB	;
	V = pow(10,G_abs/20)	;

	K_div_Q = K/QValue;

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
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = K * K * norm	;
			b1 = 2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_HIGHPASS_MODE_1_POLE :
            a1 = -exp(-w0);
            a2 = 0;
            b0 = (1 - a1) / 2;
            b1 = -b0;
            b2 = 0;
			break;

		case BIQUADS_HIGHPASS_MODE_2_POLES :
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = norm	;
			b1 = -2 * b0	;
			b2 = b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_BANDPASS_MODE :
			norm = 1/(1 + K_div_Q + K * K)	;
			b0 = K_div_Q *	norm;
			b1 = 0	;
			b2 = -b0	;
			a1 = 2 * (K * K - 1) * norm	;
			a2 = (1 - K_div_Q + K * K) * norm	;
			break;

		case BIQUADS_PEAK_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (1 + V/QValue * K + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - V/QValue * K + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + V/QValue * K + K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = b1	;
				a2 = (1 - V/QValue * K + K * K) * norm	;
			}
			break;

		case BIQUADS_LOWSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (1 + sqrt(V) * K_div_Q + V * K * K) * norm	;
				b1 = 2 * (V * K * K - 1) * norm	;
				b2 = (1 - sqrt(V) * K_div_Q + V * K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(1 + sqrt(V) * K_div_Q + V * K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = 2 * (V * K * K - 1) * norm	;
				a2 = (1 - sqrt(V) * K_div_Q + V * K * K) * norm	;
			}
			break;

		case BIQUADS_HIGHSHELF_MODE :
			if(Gain_dB >= 0)
			{
				norm = 1/(1 + K_div_Q + K * K)	;
				b0 = (V + sqrt(V) * K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - V) * norm	;
				b2 = (V - sqrt(V) * K_div_Q + K * K) * norm	;
				a1 = 2 * (K * K - 1) * norm	;
				a2 = (1 - K_div_Q + K * K) * norm	;
			}
			else
			{
				norm = 1/(V + sqrt(V) * K_div_Q + K * K)	;
				b0 = (1 + K_div_Q + K * K) * norm	;
				b1 = 2 * (K * K - 1) * norm	;
				b2 = (1 - K_div_Q + K * K) * norm	;
				a1 = 2 * (K * K - V) * norm	;
				a2 = (V - sqrt(V) * K_div_Q + K * K) * norm	;
			}
			break;
	}
	a1 = -a1;
	a2 = -a2;

	pCoeffs[0] = b0;
	pCoeffs[1] = b1;
	pCoeffs[2] = b2;
	pCoeffs[3] = a1;
	pCoeffs[4] = a2;
}
