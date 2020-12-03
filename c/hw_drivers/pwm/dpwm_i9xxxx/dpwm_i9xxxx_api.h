
#ifndef _DPWM_I9XXXX_API_H_
#define _DPWM_I9XXXX_API_H_

#include "dev_management_api.h"

#define I2S_I9XXXX_API_DATA_MODE_FLOATING_POINT  0
#define I2S_I9XXXX_API_DATA_MODE_PCM             1

enum DPWM_I9XXXX_API_ioctl_e {
	DPWM_I9XXXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DPWM_I9XXXX_DEVICE_STOP,
	DPWM_I9XXXX_GET_ROOT_CLK_DEV,
	DPWM_I9XXXX_GET_SAMPLE_RATE_HZ
};

struct dpwm_i9xxxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t  sample_rate_hz;
	uint32_t  left_ch_p;
	uint32_t  left_ch_n;
	uint32_t  right_ch_p;
	uint32_t  right_ch_n;
	uint32_t  sub_ch_p;
	uint32_t  sub_ch_n;
	uint8_t   data_mode;
};
SET_CONFIG_TYPE(dpwm_i9xxxx, struct dpwm_i9xxxx_cfg_t);

#endif
