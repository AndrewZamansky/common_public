/*
 * file : PCM_SPLITTER.h
 *
 *
 */

#ifndef _PCM_SPLITTER_H
#define _PCM_SPLITTER_H


#include "pcm_splitter_api.h"


struct pcm_splitter_instance_t {
	struct pcm_splitter_api_set_params_t  set_params;
	real_t normalizer;
	uint8_t num_of_channels;
};


#endif
