/*
 * file : DSP_MANAGEMENT.h
 *
 *
 */

#ifndef _DSP_MANAGEMENT_H
#define _DSP_MANAGEMENT_H

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

enum DSP_CHAIN_STATE_e {
	DSP_CHAIN_STATE__UNKNOWN = 0,
	DSP_CHAIN_STATE__INITIALIZING,
	DSP_CHAIN_STATE__READY
};

struct dsp_chain_t {
	struct dsp_module_inst_t *module_inst_arr;
	size_t            num_of_modules;

	// chain_in_pads is of out_pads_t type because it contain real buffers
	// that were passed to chain
	struct out_pads_t      chain_in_pads;

	// chain_out_pads is of in_pads_t type because it will contain pointers
	// to pads with processed buffers that will be copied to output buffers
	struct in_pads_t       chain_out_pads;

	DSP_CHAIN_STATE_e       chain_state;
};

#endif
