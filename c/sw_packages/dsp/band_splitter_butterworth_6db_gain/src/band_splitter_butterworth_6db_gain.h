/*
 * file : BAND_SPLITTER_BUTTERWORTH_6DB_GAIN.h
 *
 *
 */

#ifndef _BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_H
#define _BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_H

#include "band_splitter_butterworth_6db_gain_api.h"


/***************   typedefs    *******************/



struct band_splitter_butterworth_6db_gain_instance_t
{
	struct dsp_chain_t *filter_1_pole_dsp_chain;
	struct dsp_desc_t filter_all_pass_1_pole;

	struct dsp_chain_t *filter_2_poles_dsp_chain;
	struct dsp_desc_t filter_all_pass_2_poles;
	struct band_splitter_butterworth_6db_gain_api_set_params_t set_params;
};




#endif
