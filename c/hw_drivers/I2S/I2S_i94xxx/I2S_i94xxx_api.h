
#ifndef _I2S_I94XXX_API_H_
#define _I2S_I94XXX_API_H_

#include "dev_management_api.h"
#include "i2s.h"

/*****************  defines  **************/

#define I2S_I94XXX_API_MASTER_MODE    I2S_MASTER
#define I2S_I94XXX_API_SLAVE_MODE     I2S_SLAVE

#define I2S_I94XXX_FORMAT_I2S          I2S_FORMAT_I2S
#define I2S_I94XXX_FORMAT_MSB          I2S_FORMAT_MSB
#define I2S_I94XXX_FORMAT_LSB          I2S_FORMAT_LSB
#define I2S_I94XXX_FORMAT_PCM          I2S_FORMAT_PCM
#define I2S_I94XXX_FORMAT_PCMMSB       I2S_FORMAT_PCMMSB
#define I2S_I94XXX_FORMAT_PCMLSB       I2S_FORMAT_PCMLSB


#define I2S_I94XXX_API_AUDIO_FORMAT_STEREO     I2S_STEREO
#define I2S_I94XXX_API_AUDIO_FORMAT_MONO       I2S_MONO


/**********  define API  types ************/

enum I2S_I94XXX_API_ioctl_e {
	I2S_I94XXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	I2S_I94XXX_STOP_IOCTL,
	I2S_I94XXX_GET_MEASURED_SAMPLE_RATE,
	I2S_I94XXX_SYNC_FS_TO_DPWM_FS_RATE
};


struct I2S_i94xxx_cfg_t {
	struct dev_desc_t  *src_clock;
	uint32_t    clock_mode;
	uint32_t    sample_rate;
	uint32_t    i2s_format;
	uint32_t    mono_or_stereo;
	uint16_t    Mclock_factor_based_on_FSclock;
	uint8_t     tdm_ch_num;
	uint32_t     BCLK_pin;
	uint32_t     LRCLK_pin;
	uint32_t     DI_pin;
	uint32_t     DO_pin;
	uint32_t     MCLK_pin;
	uint8_t     num_of_bytes_in_word;

	// fix for uint8_t/uint16_t access if needed:
	uint8_t     do_reordering_for_16or8bit_channels;

};

/**********  define API  functions  ************/

#endif
