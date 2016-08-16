/*
 *
 * file :   voice_3D.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_voice_3D_prerequirements_check.h" // should be after {voice_3D_config.h,dev_management_api.h}

#include "voice_3D_api.h" //place first to test that header file is self-contained
#include "voice_3D.h"
#include "common_dsp_api.h"

#include "math.h"

#ifdef CONFIG_USE_HW_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

#include "auto_init_api.h"

/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char voice_3D_module_name[] = "voice_3D";


/**********   external variables    **************/



/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        voice_3D_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void voice_3D_dsp(pdsp_descriptor apdsp, size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

	VOICE_3D_Instance_t *handle;
	float *apCh1In ,  *apCh2In;
	float *apCh1Out ,  *apCh2Out;

	float main_ch_gain;
	float second_ch_gain;

	float medium_gain;
	float side_gain;
	float _3D_gain;

	handle = apdsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;
	apCh1Out = out_pads[0].buff;
	apCh2Out = out_pads[1].buff;

	medium_gain = handle->medium_gain ;
	side_gain = handle->side_gain ;
	_3D_gain = handle->_3D_gain ;

	main_ch_gain = medium_gain + side_gain ;
	second_ch_gain = medium_gain + side_gain + _3D_gain ;

	while(data_len--)
	{
		float curr_ch_1;
		float curr_ch_2;
		float tmp;
		float tmp1;

		curr_ch_1 = *apCh1In++;
		curr_ch_2 = *apCh2In++;

		tmp = main_ch_gain * curr_ch_1;
		tmp1 = curr_ch_2 * second_ch_gain;
		tmp +=tmp1;
		*apCh1Out++ = tmp;

		tmp = main_ch_gain * curr_ch_2;
		tmp1 = curr_ch_1 * second_ch_gain;
		tmp +=tmp1;
		*apCh2Out++ = tmp;
	}

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        voice_3D_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t voice_3D_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	VOICE_3D_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{
		case IOCTL_DSP_INIT :
			handle->medium_gain = 0.5;
			handle->side_gain =  0.5;
			handle->_3D_gain = 0;
			break;
		case IOCTL_VOICE_3D_SET_MEDIUM_GAIN :
			handle->medium_gain = (*((float*)aIoctl_param1))/2;
			break;
		case IOCTL_VOICE_3D_SET_SIDE_GAIN :
			handle->side_gain = (*((float*)aIoctl_param1))/2;
			break;
		case IOCTL_VOICE_3D_SET_3D_GAIN :
			handle->_3D_gain = *((float*)aIoctl_param1);
			break;
		default :
			return 1;
	}
	return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        voice_3D_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  voice_3D_init(void)
{
	DSP_REGISTER_NEW_MODULE("voice_3D",voice_3D_ioctl , voice_3D_dsp , VOICE_3D_Instance_t);
}

AUTO_INIT_FUNCTION(voice_3D_init);
