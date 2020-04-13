
#ifndef _PCM_mixer_API_H_
#define _PCM_mixer_API_H_

#include "dsp_management_api.h"

enum PCM_MIXER_API_ioctl_e {
	IOCTL_PCM_MIXER_ENABLE_TEST_CLIPPING = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_PCM_MIXER_DISABLE_TEST_CLIPPING,
	IOCTL_PCM_MIXER_GET_MAX_OUTPUT_VALUE,
	IOCTL_PCM_MIXER_SET_PARAMS
};


enum pcm_mixer_channel_justification_e {
	PCM_MIXER_CHANNEL_BITS_ARE_LEFT_JUSTIFIED,
	PCM_MIXER_CHANNEL_BITS_ARE_RIGHT_JUSTIFIED,
};

/*
 * [ ch0_0 | ch1_0 | chN_0 | ch0_1 | ch1_1 | chN_1| ... |ch0_m | ch1_m | chN_m]
 *  {      frame 0        } {      frame 1       } ...   {      frame m      }
 *                          ________/    \____________
 *            _____________/                          \_________________
 *           /                                                          \
 *          | frame_size_bytes = num_of_channels * (channel_size_bits/8) |
 *
 * [ ch0_0 | ch1_0 | chN_0 | ch0_1 | ch1_1 | chN_1| ... |ch0_m | ch1_m | chN_m]
 *                   ________/    \____________
 *    ______________/                          \_________________
 *  /                                                            \
 * |channel_size_bits = padding_bits_size + 8*subframe_size_bytes |
 *
 */
struct pcm_mixer_api_set_params_t {
	uint8_t frame_size_bytes; /*frame contains N subframes*/
	uint8_t subframe_size_bytes; /*subframes contain M bits of channel*/
	uint8_t channel_size_bits; /**/
	uint8_t channel_justification; /**/
};


#ifdef __cplusplus
extern "C" {
#endif

extern char pcm_mixer_module_name[] ;
#define PCM_MIXER_DSPT	pcm_mixer_module_name

#ifdef __cplusplus
}
#endif


#endif
