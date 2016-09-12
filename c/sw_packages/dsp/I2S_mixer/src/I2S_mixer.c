/*
 *
 * file :   I2S_mixer.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_I2S_mixer_prerequirements_check.h"

#include "I2S_mixer_api.h" //place first to test that header file is self-contained
#include "I2S_mixer.h"
#include "common_dsp_api.h"

#include "auto_init_api.h"

/********  defines *********************/
#if (2==NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fff
	typedef int16_t	buffer_type_t	;
#endif
#if (4==NUM_OF_BYTES_PER_AUDIO_WORD)
	#define	FLOAT_NORMALIZER	0x7fffffff
	typedef int32_t	buffer_type_t	;
#endif

#ifndef ABS
#define ABS(x)  (x > 0 ? x : -x)
#endif
/********  types  *********************/

/********  externals *********************/


/********  exported variables *********************/

char I2S_mixer_module_name[] = "I2S_mixer";


/**********   external variables    **************/



/***********   local variables    **************/
float g_max_out_val = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_mixer_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void I2S_mixer_dsp(pdsp_descriptor aDspDescriptor , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t  out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In ,  *apCh2In;
	float normalizer ;
	float tmp1, tmp2;

	buffer_type_t *pTxBuf;
	pTxBuf = (buffer_type_t*)out_pads[0].buff;

	apCh1In = in_pads[0]->buff;
	apCh2In = in_pads[1]->buff;

	normalizer = FLOAT_NORMALIZER;
	for( ; data_len ;data_len--)
	{
#if 1
		tmp1 = ABS(*apCh1In);
		tmp2 = ABS(*apCh2In);
		if (tmp1 >= g_max_out_val)
		{
			g_max_out_val = tmp1;
		}

		if (tmp2>= g_max_out_val)
		{
			g_max_out_val = tmp2;
		}
#endif
		*apCh1In = *apCh1In * normalizer;
		*pTxBuf = (buffer_type_t)(*apCh1In++)		;// pTxBuf[2*i]
//					*pTxBuf = *pTxBuf & 0x00ffffff;
		pTxBuf++;

		*apCh2In = *apCh2In * normalizer;
		*pTxBuf = (buffer_type_t)(*apCh2In++); // pTxBuf[2*i + 1]
//					*pTxBuf = *pTxBuf & 0x00ffffff;
		pTxBuf++;
	}

}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        I2S_mixer_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t I2S_mixer_ioctl(pdsp_descriptor aDspDescriptor ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{

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
/* Function:         I2S_mixer_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void  I2S_mixer_init(void)
{
	DSP_REGISTER_NEW_MODULE("I2S_mixer",I2S_mixer_ioctl , I2S_mixer_dsp , I2S_MIXER_Instance_t);
}

AUTO_INIT_FUNCTION(I2S_mixer_init);
