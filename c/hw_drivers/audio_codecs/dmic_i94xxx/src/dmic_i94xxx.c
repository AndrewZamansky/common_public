/*
 *
 * dmic_i94xxx.c
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "dmic_i94xxx.h"
#include "I94100.h"

#include "irq_api.h"
#include "dmic_i94xxx_api.h"
#include "clock_control_i94xxx_api.h"


/*following line add module to available module list for dynamic device tree*/
#include "dmic_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/


#define SYS_GPA_MFPL_PA0MFP_DMIC_DAT0  (0x03UL<<SYS_GPA_MFPL_PA0MFP_Pos)
#define SYS_GPA_MFPL_PA1MFP_DMIC_CLK0  (0x03UL<<SYS_GPA_MFPL_PA1MFP_Pos)
#define SYS_GPA_MFPL_PA2MFP_DMIC_DAT1  (0x03UL<<SYS_GPA_MFPL_PA2MFP_Pos)
#define SYS_GPA_MFPL_PA3MFP_DMIC_CLK1  (0x03UL<<SYS_GPA_MFPL_PA3MFP_Pos)

static void set_pinout(struct dmic_i94xxx_cfg_t *cfg_hndl)
{
	switch (cfg_hndl->dmic_data0_pin)
	{
	case DMIC_I94XXX_DMIC_DATA0_A0:
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk);
		SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA0MFP_DMIC_DAT0);
		break;
	default :
		CRITICAL_ERROR("wrong pin");
		return ;
	}
	switch (cfg_hndl->dmic_clk0_pin)
	{
	case DMIC_I94XXX_DMIC_CLK0_A1:
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA1MFP_Msk);
		SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA1MFP_DMIC_CLK0);
		break;
	default :
		CRITICAL_ERROR("wrong pin");
		return ;
	}
	switch (cfg_hndl->dmic_data1_pin)
	{
	case DMIC_I94XXX_DMIC_DATA1_A2:
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA2MFP_Msk);
		SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA2MFP_DMIC_DAT1);
		break;
	default :
		CRITICAL_ERROR("wrong pin");
		return ;
	}
	switch (cfg_hndl->dmic_clk1_pin)
	{
	case DMIC_I94XXX_DMIC_CLK1_A3:
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA3MFP_Msk);
		SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA3MFP_DMIC_CLK1);
		break;
	default :
		CRITICAL_ERROR("wrong pin");
		return ;
	}
}


static void dmic_init(struct dmic_i94xxx_cfg_t *cfg_hndl)
{
	//int dmic_irq;
	struct dev_desc_t  *src_clock;
	uint32_t src_clk_rate;
	uint32_t  Mclk_rate;
	uint32_t  sample_rate;
	uint32_t  bus_clock_rate;

	src_clock = cfg_hndl->src_clock;

	//dmic_irq = DMIC0_IRQn;

	set_pinout(cfg_hndl);

	DEV_IOCTL_1_PARAMS(i94xxx_dmic_clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
	DEV_IOCTL_0_PARAMS(i94xxx_dmic_clk_dev, CLK_IOCTL_ENABLE);

	SYS_ResetModule(DMIC_RST);

	DEV_IOCTL_1_PARAMS(i94xxx_dmic_clk_dev, CLK_IOCTL_GET_FREQ, &src_clk_rate);

	sample_rate = cfg_hndl->sample_rate;
	Mclk_rate = sample_rate * 256;
	if (src_clk_rate % Mclk_rate)
	{
		Mclk_rate = sample_rate * 500;
		if (src_clk_rate % Mclk_rate)
		{
			CRITICAL_ERROR("cannot create requested main clock");
		}
	}
	DMIC->DIV = (DMIC->DIV & ~(DMIC_DIV_MCLKDIV_Msk | DMIC_DIV_PCLKDIV_Msk));
	DMIC->DIV |= (((src_clk_rate / Mclk_rate) - 1) << DMIC_DIV_PCLKDIV_Pos);
	/* Set down sample rate 100 for quilty.
	 * (Suggest 96M used DMIC_CTL_DOWNSAMPLE_100_50 )
     * DMIC_ENABLE_DOWMSAMPLE(DMIC,DMIC_CTL_DOWNSAMPLE_100_50);
     */
	DMIC_ENABLE_DOWMSAMPLE(DMIC, DMIC_CTL_DOWNSAMPLE_64);
	bus_clock_rate = 64 * sample_rate;
	if (Mclk_rate % bus_clock_rate)
	{
		CRITICAL_ERROR("cannot create requested bus clock");
	}

	DMIC->DIV |=  (((Mclk_rate / bus_clock_rate) - 1) << DMIC_DIV_MCLKDIV_Pos);


	// Set channel's latch data falling type.
	DMIC_SET_LATCHDATA(DMIC,
			DMIC_CTL_LATCHDATA_CH01F | DMIC_CTL_LATCHDATA_CH23F);
	// Enable DMIC FIFO threshold interrupt.
	DMIC_ENABLE_FIFOTHRESHOLDINT(DMIC, 8);
}


/**
 * dmic_i94xxx_ioctl()
 *
 * return:
 */
uint8_t dmic_i94xxx_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct dmic_i94xxx_cfg_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		dmic_init(cfg_hndl);
		break;
	case DMIC_I94XXX_ENABLE_INPUT_IOCTL:
		{
			uint32_t channels;

			channels = DMIC_CTL_CH0;
			if (1 < cfg_hndl->num_of_channels)
			{
				channels =  DMIC_CTL_CH1;
			}
			if (2 < cfg_hndl->num_of_channels)
			{
				channels = DMIC_CTL_CH2;
			}
			if (3 < cfg_hndl->num_of_channels)
			{
				channels |=  DMIC_CTL_CH3;
			}
			DMIC_ENABLE_CHANNEL(DMIC, channels);
			DMIC_ENABLE_PDMA(DMIC);
		}
		break;
	case DMIC_I94XXX_DISABLE_INPUT_IOCTL:
		DMIC_DISABLE_PDMA(DMIC);
		DMIC_DISABLE_CHANNEL(DMIC,
				DMIC_CTL_CH0 | DMIC_CTL_CH1 | DMIC_CTL_CH2 | DMIC_CTL_CH3);
		break;
	default :
		return 1;
	}
	return 0;
}
