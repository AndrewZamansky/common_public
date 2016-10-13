/*
 *
 * file :   fft.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_fft_prerequirements_check.h" // should be after {fft_config.h,dev_management_api.h}

#include "fft_api.h" //place first to test that header file is self-contained
#include "fft.h"
#include "common_dsp_api.h"

#include "math.h"

#include "cpu_config.h"
#include "arm_math.h"


/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/



/***********   local variables    **************/

#define FFT_SIZE	I2S_BUFF_LEN/2
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        fft_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void fft_dsp(pdsp_descriptor apdsp, size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

//	FFT_Instance_t *handle;
	float *apCh1In;
	float *apCh1Out;
	arm_cfft_radix4_instance_f32 S;    /* ARM CFFT module */


//	handle = apdsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	 arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);
	 arm_cfft_radix4_f32(&S, apCh1In);
	 arm_cmplx_mag_f32(apCh1In, apCh1Out, FFT_SIZE);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        fft_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t fft_ioctl(pdsp_descriptor apdsp ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	FFT_Instance_t *handle;

	handle = apdsp->handle;
	switch(aIoctl_num)
	{


		case IOCTL_DSP_INIT :

			break;
		default :
			return 1;
	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FFT_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  fft_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	FFT_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (FFT_Instance_t *)malloc(sizeof(FFT_Instance_t));
	if(NULL == pInstance) return 1;

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = fft_ioctl;
	aDspDescriptor->dsp_func = fft_dsp;


	return 0 ;

}
