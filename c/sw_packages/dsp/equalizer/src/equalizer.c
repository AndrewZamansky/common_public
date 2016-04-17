/*
 *
 * file :   equalizer.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_equalizer_prerequirements_check.h"

#include "PRINTF_api.h"

#include "equalizer_api.h" //place first to test that header file is self-contained
#include "equalizer.h"


/********  defines *********************/


/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/

#define INSTANCE(hndl)	((EQUALIZER_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/


#if 0
	static float coeffs[35]={
			0.99715656, -1.9943131, 0.99715656 , -1.9943091, 0.99431717,
			1.0032988, -1.9838786, 0.98070878 ,-1.9838786,0.98400754,
			0.99144435, -1.9396375, 0.94996631 , -1.9396375, 0.94141072,
			1.0263182, -1.7937105, 0.79657972 ,-1.7990962, 0.81751221,
			1.0000000 , 0.0000000, 0.0000000 , 0.00000000, 0.00000000 ,
			0.91199619, -0.62079674, 0.23223402 , -0.62079674, 0.14423016 ,
			1.2700197 , 0.22158107, 0.22474150 , 0.50019020, 0.21615198
	};
#endif
#if 0
	static BandCoeffs_t coeffs[7]={
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0},
			{1, 0, 0 , 0, 0}
	};
#endif



#if 0
	static BandCoeffs_t coeffs[7]={
			{1, 0, 0 , 0, 0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0}
	};
#endif

#if 0
	static BandCoeffs_t coeffs[7]={
			{0.871208, -1.690729, 0.834110 , +1.690729, -0.705319},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0},
			{1,0, 0 ,0,0}
	};
#endif

#if 0
	static float coeffs[5]={
			0.5, 0, 0 , 0,0
	};
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        equalizer_dsp                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void equalizer_dsp(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS])
{
	float *apCh1In  ;
	float *apCh1Out ;

	apCh1In = in_pads[0]->buff;
	apCh1Out = out_pads[0].buff;

	biquads_cascading_filter(INSTANCE(aHandle)->pBiquadFilter , apCh1In , apCh1Out , data_len);

}

#if 0
#include "libavfilter/avfilter.h"
#include "libavfilter/internal.h"

#if 1
#include "libavutil/avassert.h"
#include "libavutil/opt.h"
#include "audio.h"
#include "avfilter.h"
#include "internal.h"

enum FilterType {
    biquad,
    equalizer,
    bass,
    treble,
    band,
    bandpass,
    bandreject,
    allpass,
    highpass,
    lowpass,
};

enum WidthType {
    NONE,
    HERTZ,
    OCTAVE,
    QFACTOR,
    SLOPE,
};

typedef struct ChanCache {
    double i1, i2;
    double o1, o2;
} ChanCache;

typedef struct {
    const AVClass *class;

    enum FilterType filter_type;
    int width_type;
    int poles;
    int csg;

    double gain;
    double frequency;
    double width;

    double a0, a1, a2;
    double b0, b1, b2;

    ChanCache *cache;

    void (*filter)(const void *ibuf, void *obuf, int len,
                   double *i1, double *i2, double *o1, double *o2,
                   double b0, double b1, double b2, double a1, double a2);
} BiquadsContext;
#endif

#define AV_LOG_BUFF_LEN  128
char av_log_buff[AV_LOG_BUFF_LEN]={0};
void av_log_ucprojects_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	int retVal;

	retVal = vsnprintf(av_log_buff,AV_LOG_BUFF_LEN-1,fmt,vl);
	if(0>=retVal)
	{
		PRINT_STR_DBG("err:av string too long \n");
		return ;
	}
	PRINT_DATA_DBG(av_log_buff,retVal);
}

#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        equalizer_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t equalizer_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t i;
	size_t num_of_bands;
	uint8_t band_num;
	BandCoeffs_t *pCoeffs;
	equalizer_api_band_set_params_t *p_band_set_params;

	switch(aIoctl_num)
	{
//#if EQUALIZER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
//		case IOCTL_GET_PARAMS_ARRAY_FUNC :
//			*(const dev_param_t**)aIoctl_param1  = EQUALIZER_Dev_Params;
//			*(uint8_t*)aIoctl_param2 = sizeof(EQUALIZER_Dev_Params)/sizeof(dev_param_t); //size
//			break;
//#endif // for EQUALIZER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0


		case IOCTL_DEVICE_START :

			pCoeffs = INSTANCE(aHandle)->pCoeffs;
			INSTANCE(aHandle)->pBiquadFilter = biquads_alloc(INSTANCE(aHandle)->num_of_bands , (float *)pCoeffs );

			break;
		case IOCTL_EQUALIZER_SET_NUM_OF_BANDS :
			num_of_bands = ((size_t)aIoctl_param1);
			INSTANCE(aHandle)->num_of_bands = num_of_bands;
			free(INSTANCE(aHandle)->pCoeffs);
			{
				int tmp=sizeof(BandCoeffs_t) * num_of_bands;
				pCoeffs=(BandCoeffs_t *)malloc(tmp);
			}
			INSTANCE(aHandle)->pCoeffs = pCoeffs;
			for(i=0 ; i<num_of_bands ; i++)
			{
				pCoeffs[i].a1 = 0;
				pCoeffs[i].a2 = 0;
				pCoeffs[i].b0 = 1;
				pCoeffs[i].b1 = 0;
				pCoeffs[i].b2 = 0;
			}

			break;
		case IOCTL_EQUALIZER_SET_BAND_BIQUADS :
#if 0
		{
			int i;
			BiquadsContext *bctx;
			AVFilterContext *ctx;

			AVFilter *filter;


			av_log_set_callback(av_log_ucprojects_callback);
			av_log(NULL, AV_LOG_ERROR,"hello from av_log\n");
 			avfilter_register_all();
 			 PRINTF_DBG( "avfilter_register_all done'\n");
			/* get a corresponding filter and open it */
 			//if (!(filter = avfilter_get_by_name("bass")))
 			if (!(filter = avfilter_get_by_name((char*)aIoctl_param1)))
 			{
 				PRINTF_DBG( "Unrecognized filter with name '%s'\n", (char*)aIoctl_param1);
			  //return 1;
			  }
			ctx = ff_filter_alloc(filter , "tmp_filter");


			filter->init(ctx); // should retuns without error


//			 avfilter_init_str(ctx , "f=1000:width_type=q:width=0.6:gain=1");
			 avfilter_init_str(ctx , (char*)aIoctl_param2);

			/* create a link for each of the input pads */
			for (i = 0; i < ctx->nb_inputs; i++)
			{
				AVFilterLink *link = av_mallocz(sizeof(AVFilterLink));
				if (!link)
				{
					//fprintf(stderr, "Unable to allocate memory for filter input link\n");
					//ret = 1;
					//goto fail;
				}
				link->type = avfilter_pad_get_type(ctx->input_pads, i);
				link->sample_rate = 48000;
				link->channels = 0 ; // not used for now
				link->format = AV_SAMPLE_FMT_FLTP ;
				ctx->inputs[i] = link;
			}
			for (i = 0; i < ctx->nb_outputs; i++)
			{
				AVFilterLink *link = av_mallocz(sizeof(AVFilterLink));
				if (!link)
				{
					//fprintf(stderr, "Unable to allocate memory for filter output link\n");
					//ret = 1;
					//goto fail;
				}
				link->type = avfilter_pad_get_type(ctx->output_pads, i);
				ctx->outputs[i] = link;
			}

		    if (filter->query_formats)
		        filter->query_formats(ctx);
		    else
		        ff_default_query_formats(ctx);


			 ctx->outputs[0]->src=ctx; // test
			filter->outputs[0].config_props(ctx->outputs[0]);
			bctx = ctx->priv;

			coeffs[0].b0 = bctx->b0;
			coeffs[1].b1 = bctx->b1;
			coeffs[2].b2 = bctx->b2;
			coeffs[3].a1 = -bctx->a1;
			coeffs[4].a2 = -bctx->a2;


			 PRINTF_DBG( "filter->outputs[0].config_props done \n");

			 ctx->outputs[0]->src=NULL;
			avfilter_free(ctx);
		}

#else
		num_of_bands = INSTANCE(aHandle)->num_of_bands;
		band_num = ((equalizer_api_band_set_t*)aIoctl_param1)->band_num ;
		if(num_of_bands > band_num )
		{
			p_band_set_params = &(((equalizer_api_band_set_t*)aIoctl_param1)->band_set_params);
			memcpy(&INSTANCE(aHandle)->band_set_params,
					p_band_set_params,sizeof(equalizer_api_band_set_params_t));
			pCoeffs = &INSTANCE(aHandle)->pCoeffs[band_num];
			biquads_calculation(
					p_band_set_params->filter_mode,
					p_band_set_params->Fc,
					p_band_set_params->QValue,
					p_band_set_params->Gain,
					48000,
					(float*)pCoeffs
					);
		}

#endif
//			PRINTF_DBG( "b0 = %f  \n",coeffs[0].b0);
//			PRINTF_DBG( "b1 = %f  \n",coeffs[0].b1);
//			PRINTF_DBG( "b2 = %f  \n",coeffs[0].b2);
//			PRINTF_DBG( "a1 = %f  \n",coeffs[0].a1);
//			PRINTF_DBG( "a2 = %f  \n",coeffs[0].a2);
			break;
		case IOCTL_EQUALIZER_GET_BAND_BIQUADS :
			p_band_set_params = &(((equalizer_api_band_set_t*)aIoctl_param1)->band_set_params);
			memcpy(p_band_set_params,
					&INSTANCE(aHandle)->band_set_params, sizeof(equalizer_api_band_set_params_t));
			break;
		default :
			return 1;
	}
	return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        EQUALIZER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  equalizer_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor)
{
	EQUALIZER_Instance_t *pInstance;

	if(NULL == aDspDescriptor) return 1;

	pInstance = (EQUALIZER_Instance_t *)malloc(sizeof(EQUALIZER_Instance_t));
	if(NULL == pInstance) return 1;

	aDspDescriptor->handle = pInstance;
	aDspDescriptor->ioctl = equalizer_ioctl;
	aDspDescriptor->dsp_func = equalizer_dsp;

	pInstance->num_of_bands =0;
	pInstance->pCoeffs=NULL;


	return 0 ;

}

