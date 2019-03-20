/*
 *
 * file :   rms.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "rms_api.h"
#include "rms.h"

#include "string.h"
#include "stdlib.h"

#include "auto_init_api.h"



/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char rms_module_name[] = "rms";


/**********   external variables    **************/



/***********   local variables    **************/

#define  ALPHA   0.00004f
#define  ONE_MINUS_ALPHA   (1.0f - ALPHA)

/**
 * rms_dsp()
 *
 * return:
 */
void rms_dsp(struct dsp_module_inst_t *adsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	struct rms_instance_t *handle;
	real_t *apCh1In;
	real_t *apCh1Out;
	size_t in_data_len1 ;
	size_t out_data_len1 ;
	real_t prev_rms_sq;
	real_t curr_x1;

	handle = (struct rms_instance_t *)adsp->handle;

	dsp_get_buffer_from_pad(in_pads[0], &apCh1In, &in_data_len1);
	dsp_get_buffer_from_pad(&out_pads[0], &apCh1Out, &out_data_len1);

	if (in_data_len1 != out_data_len1 )
	{
		CRITICAL_ERROR("bad buffers sizes");
	}

	prev_rms_sq = handle->prev_rms_sq;

	while( in_data_len1-- )
	{

		curr_x1 = *apCh1In++;

		curr_x1 *= curr_x1;
		curr_x1 *=  ALPHA;
		prev_rms_sq *= ONE_MINUS_ALPHA;
		prev_rms_sq +=curr_x1;

		*apCh1Out++ = prev_rms_sq;
	}
	handle->prev_rms_sq = prev_rms_sq;

}



/**
 * rms_ioctl()
 *
 * return:
 */
uint8_t rms_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct rms_instance_t *handle;

	handle = (struct rms_instance_t *)adsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->prev_rms_sq = 0;
			break;
		default :
			return 1;
	}

	return 0;
}




/**
 * rms_init()
 *
 * return:
 */
void  rms_init(void)
{
	DSP_REGISTER_NEW_MODULE("rms",
			rms_ioctl, rms_dsp, struct rms_instance_t);
}

AUTO_INIT_FUNCTION(rms_init);
