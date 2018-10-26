/*
 *
 * file :   band_splitter_butterworth_6db_gain.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"


#include "band_splitter_butterworth_6db_gain_api.h"
#include "band_splitter_butterworth_6db_gain.h"

#include "auto_init_api.h"

#include "string.h"

#define DEBUG
#include "biquad_filter_api.h"


/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char band_splitter_butterworth_6db_gain_module_name[] =
										"band_splitter_butterworth_6db_gain";


/**********   external variables    **************/



/***********   local variables    **************/



/**
 * band_splitter_butterworth_6db_gain_dsp()
 *
 * return:
 */
void band_splitter_butterworth_6db_gain_dsp(struct dsp_module_inst_t * adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] ,
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	struct band_splitter_butterworth_6db_gain_instance_t *handle;
	struct dsp_chain_t *filter_1_pole_dsp_chain;
	struct dsp_chain_t *filter_2_poles_dsp_chain;
	real_t *apCh1In ;
	real_t *apCh1Out  ;
	real_t *apCh2Out  ;
	uint16_t i;
	size_t in_data_len ;
	size_t out1_data_len ;
	size_t out2_data_len ;

	handle =
		(struct band_splitter_butterworth_6db_gain_instance_t *)adsp->handle;

	filter_1_pole_dsp_chain = handle->filter_1_pole_dsp_chain;
	filter_2_poles_dsp_chain = handle->filter_2_poles_dsp_chain;

	dsp_get_buffer_from_pad(in_pads[0], &apCh1In, &in_data_len);
	dsp_get_buffer_from_pad(&out_pads[0], &apCh1Out, &out1_data_len);
	dsp_get_buffer_from_pad(&out_pads[1], &apCh2Out, &out2_data_len);

	dsp_management_api_set_chain_input_buffer(filter_1_pole_dsp_chain,
			DSP_INPUT_PAD_0, (uint8_t *)apCh1In, in_data_len);

	dsp_management_api_set_chain_output_buffer(filter_1_pole_dsp_chain,
			DSP_OUTPUT_PAD_0, (uint8_t *)apCh1Out, out1_data_len);

	dsp_management_api_process_chain(filter_1_pole_dsp_chain );



	dsp_management_api_set_chain_input_buffer(filter_2_poles_dsp_chain,
			DSP_INPUT_PAD_0, (uint8_t *)apCh1In, in_data_len);

	dsp_management_api_set_chain_output_buffer(filter_2_poles_dsp_chain,
			DSP_OUTPUT_PAD_0, (uint8_t *)apCh2Out, out2_data_len);

	dsp_management_api_process_chain(filter_2_poles_dsp_chain );
#if 1


	for ( i = 0; i<out1_data_len; i++)
	{
		real_t tmp;

		tmp = *apCh1Out + *apCh2Out;
		*apCh1Out = *apCh1Out - *apCh2Out;
		apCh1Out++;
		*apCh2Out++ = tmp;
	}
#else
	for ( i = 0; i<out1_data_len; i++)
	{
		real_t tmp;
		tmp = *apCh1Out ;
		*apCh1Out++ = tmp;
		*apCh2Out++ = tmp;
	}
#endif
}


char p_filter_all_pass_1_pole[] = "p_filter_all_pass_1_pole";

static struct static_dsp_component chain_1_pole[] =
{
	{"chain_inputs", CHAIN_INPUTS_DSPT, { } },

	{p_filter_all_pass_1_pole,
			BIQUAD_FILTER_DSPT, {{"chain_inputs", 0}}},

	{"chain_outputs", CHAIN_OUTPUTS_DSPT,
			{ {p_filter_all_pass_1_pole, 0} } },
};

char p_filter_all_pass_2_poles[] = "p_filter_all_pass_2_poles";

static struct static_dsp_component chain_2_poles[] =
{
	{"chain_inputs", CHAIN_INPUTS_DSPT, { } },

	{p_filter_all_pass_2_poles,
			BIQUAD_FILTER_DSPT, {{"chain_inputs", 0}}},

	{"chain_outputs", CHAIN_OUTPUTS_DSPT,
			{ {p_filter_all_pass_2_poles, 0} } },
};


static 	void init_filters_dsp_chains(
		struct band_splitter_butterworth_6db_gain_instance_t *handle)
{
	struct dsp_chain_t *filter_1_pole_dsp_chain;
	struct dsp_chain_t *filter_2_poles_dsp_chain;

	filter_1_pole_dsp_chain =
			DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(chain_1_pole);
	handle->filter_1_pole_dsp_chain =  filter_1_pole_dsp_chain;

	dsp_management_api_ioctl_1_params(
			filter_1_pole_dsp_chain, p_filter_all_pass_1_pole,
			IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS , (void*) 1 );


	filter_2_poles_dsp_chain =
			DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(chain_2_poles);
	handle->filter_2_poles_dsp_chain =  filter_2_poles_dsp_chain;

	dsp_management_api_ioctl_1_params(
			filter_2_poles_dsp_chain, p_filter_all_pass_2_poles,
			IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS , (void*) 1 );

}


static 	void set_freq(
		struct band_splitter_butterworth_6db_gain_instance_t *handle,
		real_t freq)
{
	struct biquad_filter_api_band_set_t band_set;
	struct biquad_filter_api_band_set_params_t  *p_band_set_params;
	struct dsp_chain_t *filter_1_pole_dsp_chain;
	struct dsp_chain_t *filter_2_poles_dsp_chain;

	filter_1_pole_dsp_chain = handle->filter_1_pole_dsp_chain;
	filter_2_poles_dsp_chain = handle->filter_2_poles_dsp_chain;


	p_band_set_params = &band_set.band_set_params;
	p_band_set_params->bypass = 0;
	p_band_set_params->Fc = freq;
	p_band_set_params->filter_mode = BIQUADS_ALL_PASS_BUTTERWORTH_1_POLE;
	band_set.band_num = 0;
	dsp_management_api_ioctl_1_params(
			filter_1_pole_dsp_chain, p_filter_all_pass_1_pole,
			IOCTL_BIQUAD_FILTER_SET_BAND, &band_set );
	p_band_set_params->filter_mode = BIQUADS_ALL_PASS_BUTTERWORTH_2_POLES;
	band_set.band_num = 0;
	dsp_management_api_ioctl_1_params(
			filter_2_poles_dsp_chain, p_filter_all_pass_2_poles,
			IOCTL_BIQUAD_FILTER_SET_BAND, &band_set );

}



/**
 * band_splitter_butterworth_6db_gain_ioctl()
 *
 * return:
 */
uint8_t band_splitter_butterworth_6db_gain_ioctl(struct dsp_module_inst_t * adsp,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct band_splitter_butterworth_6db_gain_instance_t *handle;
	struct band_splitter_butterworth_6db_gain_api_set_params_t *set_params;
	real_t freq;

	handle =
		(struct band_splitter_butterworth_6db_gain_instance_t *)adsp->handle;
	set_params = &handle->set_params;



	switch(aIoctl_num)
	{

	case IOCTL_DSP_INIT :
		set_params->fc = 20000.0f;
		init_filters_dsp_chains(handle);
		break;

	case IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_SET_FC :
		freq = *(float*)aIoctl_param1;
		set_params->fc = freq;
		set_freq(handle, freq);
		break;

	case IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_GET_PARAMS :
		memcpy(aIoctl_param1, set_params,
			sizeof(struct band_splitter_butterworth_6db_gain_api_set_params_t));
		break;
	default :
		return 1;
	}
	return 0;
}



/**
 * band_splitter_butterworth_6db_gain_init()
 *
 * return:
 */
void  band_splitter_butterworth_6db_gain_init(void)
{
	DSP_REGISTER_NEW_MODULE("band_splitter_butterworth_6db_gain",
			band_splitter_butterworth_6db_gain_ioctl ,
			band_splitter_butterworth_6db_gain_dsp ,
			struct band_splitter_butterworth_6db_gain_instance_t);
}

AUTO_INIT_FUNCTION(band_splitter_butterworth_6db_gain_init);
