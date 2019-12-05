
#ifndef _DPWM_I94XXX_API_H_
#define _DPWM_I94XXX_API_H_

#include "dev_management_api.h"

enum DPWM_I94XXX_API_ioctl_e {
	DPWM_I94XXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DPWM_I94XXX_DEVICE_STOP,
	DPWM_I94XXX_GET_ROOT_CLK_DEV,
	DPWM_I94XXX_GET_SAMPLE_RATE_HZ
};

struct dpwm_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t  sample_rate_hz;
	uint32_t  left_ch_p;
	uint32_t  left_ch_n;
	uint32_t  right_ch_p;
	uint32_t  right_ch_n;
	uint32_t  sub_ch_p;
	uint32_t  sub_ch_n;
};
SET_CONFIG_TYPE(dpwm_i94xxx, struct dpwm_i94xxx_cfg_t);

#endif
