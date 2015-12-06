
/*
 *  cmd_set_eq_band.c
 */
#include "dev_managment_config.h"
#include "src/_dev_managment_prerequirements_check.h"// should be after dev_managment_config.h
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <command.h>
#include "shell_api.h"
#include "dev_managment_api.h"
#include "dsp_managment_api.h"

#include "equalizer_api.h"
#include "common_dsp_api.h"

extern dsp_descriptor_t leftChanelEQ,rightChanelEQ;

/*
 * Subroutine:  force_output
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
int do_set_eq_band (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{



#if 0 // ffmpeg
	if(argc < 3)
	{
		SHELL_REPLY_STR("err . syntax : set_eq_band filter_name filter_params\n");
		return 1;
	}
	DSP_IOCTL_2_PARAMS(&leftChanelDsp , IOCTL_EQUALIZER_SET_BAND_BIQUADS, argv[1] , argv[2] );
#else
	set_band_biquads_t  set_band_biquads;

	if(argc < 6)
	{
		SHELL_REPLY_STR("syntax err\n");
		return 1;
	}

	set_band_biquads.band_num = atoi(argv[1]);

	if (0 == strcmp(argv[2],"transparent"))
	{
		set_band_biquads.filter_mode = BIQUADS_TRANSPARENT_MODE;
	}
	else if (0 == strcmp(argv[2],"low_pass_1_pole"))
	{
		set_band_biquads.filter_mode = BIQUADS_LOWPASS_MODE_1_POLE;
	}
	else if (0 == strcmp(argv[2],"low_pass_2_poles"))
	{
		set_band_biquads.filter_mode = BIQUADS_LOWPASS_MODE_2_POLES;
	}
	else if (0 == strcmp(argv[2],"high_pass_1_pole"))
	{
		set_band_biquads.filter_mode = BIQUADS_HIGHPASS_MODE_1_POLE;
	}
	else if (0 == strcmp(argv[2],"high_pass_2_poles"))
	{
		set_band_biquads.filter_mode = BIQUADS_HIGHPASS_MODE_2_POLES;
	}
	else if (0 == strcmp(argv[2],"peak"))
	{
		set_band_biquads.filter_mode = BIQUADS_PEAK_MODE;
	}
	else if (0 == strcmp(argv[2],"low_shelf"))
	{
		set_band_biquads.filter_mode = BIQUADS_LOWSHELF_MODE;
	}
	else if (0 == strcmp(argv[2],"high_shelf"))
	{
		set_band_biquads.filter_mode = BIQUADS_HIGHSHELF_MODE;
	}
	else
	{
		SHELL_REPLY_STR("filter mode uknown\n");
		return 1;
	}

	set_band_biquads.Fc = (float)atof(argv[3]);
//	set_band_biquads.Fc = (float)atof("1000");
	set_band_biquads.QValue = (float)atof(argv[4]);
	set_band_biquads.Gain = (float)atof(argv[5]);

	DSP_IOCTL_1_PARAMS(&leftChanelEQ , IOCTL_EQUALIZER_SET_BAND_BIQUADS, &set_band_biquads );
	DSP_IOCTL_1_PARAMS(&rightChanelEQ , IOCTL_EQUALIZER_SET_BAND_BIQUADS, &set_band_biquads );

#endif
	return 0;
}

U_BOOT_CMD(
	set_eq_band,     255,	0,	do_set_eq_band,
	"set_eq_band band_num filter_type Fc QValue Gain",
	"info   - \n"
);

