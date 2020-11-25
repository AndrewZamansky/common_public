/*
 * file :   upsampling_by_int_arm_cortex_m.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "fir_filter_api.h"

static char lowpass_fir_filter[] = "lowpass_fir_filter";


/*
 *  MSVC C++ compiler does not support compound literals used in
 *  SET_INPUTS() macro. so separate .c file is used
 */
static struct static_dsp_component_t fir_chain[] =
{
	{ "chain_inputs", CHAIN_INPUTS_DSPT,
			SET_INPUTS(NO_INPUTS) },
	{ lowpass_fir_filter, FIR_FILTER_DSPT,
			SET_INPUTS( IN0("chain_inputs", 0)) },
	{"chain_outputs", CHAIN_OUTPUTS_DSPT,
			SET_INPUTS( IN0(lowpass_fir_filter, 0)) },
};


chain_handle_t upsampling_by_int_init_fir_chain(
		struct fir_filter_api_set_params_t *fir_set_params)
{
	chain_handle_t fir_dsp_chain;
	fir_dsp_chain = DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(fir_chain);
	dsp_management_api_ioctl_1_params(fir_dsp_chain, lowpass_fir_filter,
					IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS, fir_set_params);
	return fir_dsp_chain;
}
