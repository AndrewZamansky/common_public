
#ifndef _I2S_splitter_API_H_
#define _I2S_splitter_API_H_


enum I2S_SPLITTER_API_ioctl_e {
	IOCTL_PCM_SPLITTER_SET_PARAMS = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
};


enum pcm_splitter_channel_justification_e {
	PCM_SPLITTER_CHANNEL_BITS_ARE_LEFT_JUSTIFIED,
	PCM_SPLITTER_CHANNEL_BITS_ARE_RIGHT_JUSTIFIED,
};

struct pcm_splitter_api_set_params_t {
	uint8_t frame_size_bytes; /*frame contains N subframes*/
	uint8_t subframe_size_bytes; /*subframes contain M bits of channel*/
	uint8_t channel_size_bits; /**/
	uint8_t channel_justification; /**/
};

#ifdef __cplusplus
extern "C" {
#endif

extern char pcm_splitter_module_name[] ;
#define PCM_SPLITTER_DSPT	pcm_splitter_module_name

#ifdef __cplusplus
}
#endif

#endif
