/*
 * file : PCM_MIXER.h
 *
 *
 */

#ifndef _PCM_MIXER_H
#define _PCM_MIXER_H

#include "PCM_mixer_api.h"


struct pcm_mixer_instance_t {
	struct pcm_mixer_api_set_params_t  set_params;
	uint8_t num_of_channels;
	real_t max_out_val;
	uint8_t enable_test_clipping;
};


#endif
