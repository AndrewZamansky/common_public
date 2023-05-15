/*
 * file :   downsampling_by_int_arm_cortex_m.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "biquad_filter_api.h"

char biquad_filter[] = "biquad_filter";


/*
 *  MSVC C++ compiler does not support compound literals used in
 *  SET_INPUTS() macro. so separate .c file is used
 */
static struct static_dsp_component_t biquad_chain[] =
{
	{ "chain_inputs", CHAIN_INPUTS_DSPT,
			SET_INPUTS(NO_INPUTS) },
	{ biquad_filter, BIQUAD_FILTER_DSPT,
			SET_INPUTS( IN0("chain_inputs", 0)) },
	{"chain_outputs", CHAIN_OUTPUTS_DSPT,
			SET_INPUTS( IN0(biquad_filter, 0)) },
};


chain_handle_t downsampling_by_int_init_biquad_chain(size_t num_of_bands)
{
	chain_handle_t biquad_dsp_chain;
	biquad_dsp_chain = DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(biquad_chain);
	DSP_IOCTL(biquad_dsp_chain, biquad_filter,
				IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS, (void*)num_of_bands);

	return biquad_dsp_chain;
}
