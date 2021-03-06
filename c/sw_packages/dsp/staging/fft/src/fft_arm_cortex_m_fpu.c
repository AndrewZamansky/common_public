/*
 *
 * file :   fft.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors_api.h"

#include "dsp_management_api.h"


#include "fft_api.h"
#include "fft.h"
#include "common_dsp_api.h"

#include "math.h"

#include "cpu_config.h"
#include "arm_math.h"


#define FFT_SIZE	I2S_BUFF_LEN/2
/*
 * Function:        fft_dsp
 */
void fft_dsp(struct dsp_module_inst_t *adsp, size_t data_len,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{

//	FFT_Instance_t *handle;
	float *apCh1In;
	float *apCh1Out;
	arm_cfft_radix4_instance_f32 S;    /* ARM CFFT module */


//	handle = adsp->handle;
	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	 arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);
	 arm_cfft_radix4_f32(&S, apCh1In);
	 arm_cmplx_mag_f32(apCh1In, apCh1Out, FFT_SIZE);
}




/*
 * Function:        fft_ioctl
 */
uint8_t fft_ioctl(struct dsp_module_inst_t *adsp,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct FFT_Instance_t *handle;

	handle = adsp->handle;
	switch(aIoctl_num)
	{


		case IOCTL_DSP_INIT :

			break;
		default :
			return 1;
	}
	return 0;
}



/*
 * Function:        FFT_API_Init_Dev_Descriptor
 *
 */
uint8_t  fft_api_init_dsp_descriptor(struct dsp_module_inst_t aDspDescriptor)
{
	struct FFT_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (struct FFT_Instance_t *)os_safe_malloc(sizeof(FFT_Instance_t));
	errors_api_check_if_malloc_succeed(pInstance);

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = fft_ioctl;
	aDspDescriptor->dsp_func = fft_dsp;


	return 0 ;

}
