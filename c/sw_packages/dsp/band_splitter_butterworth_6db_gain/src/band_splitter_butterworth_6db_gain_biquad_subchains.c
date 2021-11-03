/*
 *
 * file :   band_splitter_butterworth_6db_gain.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"
#include "band_splitter_butterworth_6db_gain_api.h"
#include "band_splitter_butterworth_6db_gain.h"

#include "biquad_filter_api.h"


char band_splitter_butterworth_6db_filter_all_pass_1_pole[] =
				"band_splitter_butterworth_6db_filter_all_pass_1_pole";

/*
 *  MSVC C++ compiler does not support compound literals used in
 *  SET_INPUTS() macro. so separate .c file is used
 */
static struct static_dsp_component_t chain_1_pole[] = {
{ "chain_inputs", CHAIN_INPUTS_DSPT ,
	SET_INPUTS(NO_INPUTS) },
{ band_splitter_butterworth_6db_filter_all_pass_1_pole, BIQUAD_FILTER_DSPT ,
	SET_INPUTS( IN0("chain_inputs", 0)) },
{"chain_outputs", CHAIN_OUTPUTS_DSPT,
	SET_INPUTS( IN0(band_splitter_butterworth_6db_filter_all_pass_1_pole, 0)) },
};


char band_splitter_butterworth_6db_filter_all_pass_2_poles[] =
			"band_splitter_butterworth_6db_filter_all_pass_2_poles";

/*
 *  MSVC C++ compiler does not support compound literals used in
 *  SET_INPUTS() macro. so separate .c file is used
 */
static struct static_dsp_component_t chain_2_poles[] = {
{ "chain_inputs", CHAIN_INPUTS_DSPT ,
	SET_INPUTS(NO_INPUTS) },
{ band_splitter_butterworth_6db_filter_all_pass_2_poles, BIQUAD_FILTER_DSPT ,
	SET_INPUTS( IN0("chain_inputs", 0)) },
{"chain_outputs", CHAIN_OUTPUTS_DSPT,
	SET_INPUTS( IN0(band_splitter_butterworth_6db_filter_all_pass_2_poles, 0))},
};


void band_splitter_butterworth_6db_gain_init_subchains(
		struct band_splitter_butterworth_6db_gain_instance_t *handle)
{
	chain_handle_t filter_1_pole_dsp_chain;
	chain_handle_t filter_2_poles_dsp_chain;

	filter_1_pole_dsp_chain =
			DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(chain_1_pole);
	handle->filter_1_pole_dsp_chain =  filter_1_pole_dsp_chain;

	DSP_IOCTL(filter_1_pole_dsp_chain,
			band_splitter_butterworth_6db_filter_all_pass_1_pole,
			IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS , (void*) 1 );

	filter_2_poles_dsp_chain =
			DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(chain_2_poles);
	handle->filter_2_poles_dsp_chain =  filter_2_poles_dsp_chain;

	DSP_IOCTL(filter_2_poles_dsp_chain,
			band_splitter_butterworth_6db_filter_all_pass_2_poles,
			IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS , (void*) 1 );
}
