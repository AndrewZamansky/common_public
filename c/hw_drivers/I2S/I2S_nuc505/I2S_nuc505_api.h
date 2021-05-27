
#ifndef _I2S_NUC505_API_H_
#define _I2S_NUC505_API_H_


#define I2S_NUC505_API_MASTER_MODE		1
#define I2S_NUC505_API_SLAVE_MODE		0


enum I2S_API_ioctl_e {
	I2S_SET_OUT_VOLUME_LEVEL_DB = IOCTL_LAST_COMMON_IOCTL + 1,
	I2S_SET_PARAMS,
	I2S_GET_MEASURED_SAMPLE_RATE,
	I2S_ENABLE_OUTPUT_IOCTL

};


struct I2S_API_set_params_t
{
	uint16_t num_of_words_in_buffer_per_chenel;
	uint8_t	num_of_bytes_in_word;
};


#endif
