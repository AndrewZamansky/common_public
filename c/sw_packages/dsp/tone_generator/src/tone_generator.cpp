/*
 *
 * file :   tone_generator.c
 *
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "tone_generator_api.h"
#include "tone_generator.h"

#include "math.h"
#if (defined(CONFIG_ARM) && \
		defined(CONFIG_CORTEX_M4) && defined(CONFIG_INCLUDE_CORTEX_M_FPU))
	#ifndef ARM_MATH_CM4
		#define ARM_MATH_CM4
	#endif
	#ifndef __FPU_PRESENT
		#define __FPU_PRESENT  1
	#endif
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-compare"
	#include "arm_math.h"
	#pragma GCC diagnostic pop
	#define SIN arm_sin_f32
#else
	#include "math.h"
	#define SIN sin
#endif

#include "auto_init_api.h"


#define  PI_mult_2  6.28318530718f


char tone_generator_module_name[] = "tone_generator";


volatile uint32_t __stop = 0xffffffff;
/**
 * tone_generator_dsp()
 *
 * return:
 */
static void tone_generator_dsp(struct dsp_module_inst_t *adsp)
{
	struct TONE_GENERATOR_Instance_t *handle;
	real_t *apCh1Out;
	real_t *apCh2Out;
	size_t i ;
	size_t output_buff_size ;
	real_t volume_lin;
	real_t curr_time;
	real_t curr_time_delta;
	real_t prev_val;
	uint8_t buff_is_zero_buffer;

	handle = (struct TONE_GENERATOR_Instance_t *)adsp->handle;
	output_buff_size = handle->output_buff_size;
	if (0 == output_buff_size )
	{
		CRITICAL_ERROR("tone generator: bad output buffer size");
	}

	dsp_get_output_buffer_from_pad(adsp, 0, &apCh1Out, output_buff_size);
	dsp_get_output_buffer_from_pad(adsp, 1, &apCh2Out, output_buff_size);

	volume_lin = handle->volume_lin;
	curr_time = handle->curr_time;
	curr_time_delta = handle->curr_time_delta;

	prev_val = handle->prev_val;
	output_buff_size = output_buff_size / sizeof(real_t);
	for(i = 0; i < output_buff_size; i++)
	{
		real_t cur_val;

		cur_val = SIN(curr_time);
		cur_val *= volume_lin;
		*apCh1Out++ = cur_val ;
		*apCh2Out++ = cur_val ;
		curr_time += curr_time_delta;
		if ((prev_val < 0) && (cur_val >= 0))
		{
			curr_time = curr_time_delta;
		}
		prev_val = cur_val;
	}
	handle->prev_val = prev_val;
	handle->curr_time = curr_time;
}


static void update_params(struct TONE_GENERATOR_Instance_t *handle)
{
	uint32_t sample_rate_hz;
	real_t freq_hz_real;

	freq_hz_real = (float)handle->freq_hz;
	sample_rate_hz = handle->sample_rate_hz;
	handle->curr_time_delta =
			(freq_hz_real * PI_mult_2) / (float)sample_rate_hz;
	handle->curr_time = 0;
}


/**
 * tone_generator_ioctl()
 *
 * return:
 */
static uint8_t tone_generator_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct TONE_GENERATOR_Instance_t *handle;

	handle = (struct TONE_GENERATOR_Instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->freq_hz = 1000;
			handle->sample_rate_hz = 48000;
			handle->volume_lin = 1.0f;
			handle->curr_time = 0;
			handle->curr_time_delta = 0;
			handle->prev_val = 0;
			handle->output_buff_size = 0;
			break;
		case IOCTL_TONE_GENERATOR_SET_FREQUENCY :
			handle->freq_hz = (*((uint32_t*)aIoctl_param1));
			update_params(handle);
			break;
		case IOCTL_TONE_GENERATOR_SET_SAMPLE_RATE :
			handle->sample_rate_hz = *((uint32_t*)aIoctl_param1);
			update_params(handle);
			break;
		case IOCTL_TONE_GENERATOR_SET_VOLUME_LIN :
			handle->volume_lin = (*((float*)aIoctl_param1));
			break;
		case IOCTL_TONE_GENERATOR_SET_OUTPUT_BUFFER_SIZE:
			handle->output_buff_size = (*((size_t*)aIoctl_param1));
			break;
		default :
			return 1;
	}
	return 0;
}


/**
 * tone_generator_init()
 *
 * return:
 */
extern "C" void  tone_generator_init(void)
{
	DSP_REGISTER_NEW_MODULE("tone_generator", tone_generator_ioctl,
				tone_generator_dsp,
				NULL, dsp_management_default_mute,
				DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE,
				struct TONE_GENERATOR_Instance_t);
}

AUTO_INIT_FUNCTION(tone_generator_init);
