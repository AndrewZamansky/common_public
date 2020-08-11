/*
 *
 * file :   NAU88L24.c
 *
 *
 */

#include "_project.h"

#include "NAU88L24_api.h"
#include "NAU88L24.h"

#include "dev_management_api.h"
#include "i2c_api.h"
#include "clock_control_api.h"

#include "os_wrapper.h"

struct  NAU88L24_cmd_t {
	uint16_t  reg_addr;
	uint16_t  reg_val;
};


static struct  NAU88L24_cmd_t const init_88L24_arr[] = {
	//-- Address ------ Data ---------//
	{0x0020 , 0x0000 },
	{0x0004 , 0x0000 },
	{0x0025 , 0x0082 },
	{0x0026 , 0x0000 },
	{0x0031 , 0x0800 },
	{0x0032 , 0x0100 },
	{0x0033 , 0x0300 },
	{0x0034 , 0x0000 },
	//{	0x0034 , 0xFFFF },//attenuation for Ch0 and Ch1, both 0dB attenuation
	{0x0050 , 0x2007 },
	{0x006A , 0x0000 },
	{0x006B , 0x0005 },
	{0x0073 , 0x330C },
	//{0x0076 , 0x3140 },
	{0x007B , 0x0000 },
	//{0x0080 , 0x00A0 },
	{0x0081 , 0x0003 }
};


static void send_to_88L24(struct dev_desc_t *i2c_bus_dev, uint8_t dev_addr,
		uint16_t reg_addr, uint16_t data)
{
	struct i2c_api_master_write_t  i2c_write_struct;
	uint8_t send_i2c_data[2];
	uint8_t ret_val;

	i2c_write_struct.device_addr_7bit = dev_addr;
	i2c_write_struct.num_of_bytes_to_write = 2;
	i2c_write_struct.reg_addr = reg_addr;
	i2c_write_struct.reg_addr_size = 2;
	send_i2c_data[0] = (data >> 8) & 0xff; /* MSB first */
	send_i2c_data[1] = data & 0xff;

	i2c_write_struct.tx_data = send_i2c_data;

	ret_val = DEV_IOCTL_1_PARAMS(
			i2c_bus_dev, IOCTL_I2C_MASTER_WRITE, &i2c_write_struct);
	if ( (0 != ret_val) || (0 != i2c_write_struct.i2c_error))
	{
		CRITICAL_ERROR("write to 88L24 failed");
	}
}


#define  DMIC_CLK_DIV_4  0x0100

static void init_88L24_clocks(struct NAU88L24_config_t *cfg_hndl)
{
	struct dev_desc_t    *i2c_bus_dev;
	struct dev_desc_t *   i2s_mclk_clock;
	struct dev_desc_t *   i2s_fsclk_clock;
	uint8_t    dev_addr;
	uint16_t   reg_val;
	uint32_t   internal_MCLK;
	uint32_t   mclk_clock_rate;
	uint32_t   fsclk_clock_rate;
	uint8_t   MCLK_div;
	uint8_t   HPF_filter_field;

	i2c_bus_dev = cfg_hndl->i2c_bus_dev;
	dev_addr = cfg_hndl->dev_addr;
	i2s_mclk_clock = cfg_hndl->i2s_mclk_clock;
	i2s_fsclk_clock = cfg_hndl->i2s_fsclk_clock;

	DEV_IOCTL_1_PARAMS(i2s_mclk_clock, CLK_IOCTL_GET_FREQ, &mclk_clock_rate);
	DEV_IOCTL_1_PARAMS(i2s_fsclk_clock, CLK_IOCTL_GET_FREQ, &fsclk_clock_rate);

	internal_MCLK = 256 * fsclk_clock_rate;
	MCLK_div = 1;
	while ((mclk_clock_rate / MCLK_div) > internal_MCLK)
	{
		MCLK_div++;
		if (MCLK_div > 32)
		{
			break;
		}
	}
	reg_val = 0;
	switch (MCLK_div)
	{
	case 1:
		reg_val |= 0x0;
		break;
	case 2:
		reg_val |= 0x2;
		break;
	case 3:
		reg_val |= 0x7;
		break;
	case 4:
		reg_val |= 0x3;
		break;
	case 6:
		reg_val |= 0xa;
		break;
	case 8:
		reg_val |= 0x4;
		break;
	case 12:
		reg_val |= 0xb;
		break;
	case 16:
		reg_val |= 0x5;
		break;
	case 24:
		reg_val |= 0xc;
		break;
	case 32:
		reg_val |= 0x6;
		break;
	default:
		CRITICAL_ERROR("unsupported MCLK divider");
	}
	reg_val |= DMIC_CLK_DIV_4;
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0003, reg_val);

	HPF_filter_field = 0;
	switch (fsclk_clock_rate)
	{
	case 8000:
		HPF_filter_field = 5;
		break;
	case 12000:
		HPF_filter_field = 4;
		break;
	case 16000:
		HPF_filter_field = 3;
		break;
	case 24000:
		HPF_filter_field = 2;
		break;
	case 32000:
		HPF_filter_field = 1;
		break;
	case 48000:
		HPF_filter_field = 0;
		break;
	default:
		CRITICAL_ERROR("unsupported FSCLK for HPF filter");
	}
	reg_val = 0x3; // OSR = 256
	reg_val |= HPF_filter_field << 5;
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0024, reg_val);

}


#define  ENABLE_VMID        0x0040
#define  VMID_TIE_OFF_125k  0x0020
#define  MUTE_PGA_L         0x4000
#define  MUTE_PGA_R         0x0040

#define  POWERUP_PGA_L    0x1000
#define  POWERUP_PGA_R    0x2000

#define  POWERUP_MAIN_DRIVER    0x0003
#define  POWERUP_OUTPUT_DRIVER  0x000C

#define  FEPGA_MODE_AUX_MONO_HSMIC_L  0x0001
#define  FEPGA_MODE_MICP_DMIC_L       0x0002
#define  FEPGA_MODE_AUX_MONO_HSMIC_R  0x0010
#define  FEPGA_MODE_MICP_DMIC_R       0x0020

#define  DISCHRG   0x0800

#define  POWER_ON_ADC_L    0x0040
#define  POWER_ON_ADC_R    0x0080

#define  ENABLE_BIAS                0x1000
#define  VMID_PRE_CHARGE_DISABLE    0x2000
#define  ENABLE_AUTO_SHORT_CIR_DETECT    0x0040

#define  ENABLE_CHARGE_PUMP       0x0020
#define  HEADPHONES_POWER_DOWN    0x0300

#define  ADC_MUX_CH_POS  9

static void set_routing(struct NAU88L24_config_t *cfg_hndl)
{
	struct dev_desc_t    *i2c_bus_dev;
	uint8_t    input_type;
	uint8_t    output_type;
	uint8_t    dev_addr;
	uint16_t  val_0x76;
	uint16_t  val_0x7f;
	uint16_t  val_0x77;
	uint16_t  val;

	i2c_bus_dev = cfg_hndl->i2c_bus_dev;
	dev_addr = cfg_hndl->dev_addr;
	input_type = cfg_hndl->input_type;
	output_type = cfg_hndl->output_type;

	val_0x76 = 0;
	val_0x77 = 0;
	val_0x7f = 0;
	switch (input_type)
	{
	case NAU88L24_Input_type_none:
		// do nothing
		break;
	case NAU88L24_Input_type_stereo_aux:
		CRITICAL_ERROR("fix single ended input . reg 0x79");

		val_0x7f |= (POWERUP_PGA_L + POWERUP_PGA_R);
		val_0x77 |= (FEPGA_MODE_AUX_MONO_HSMIC_L + FEPGA_MODE_AUX_MONO_HSMIC_R);
		val = 0x0100; // 0db gain
		send_to_88L24(
				i2c_bus_dev, dev_addr, 0x002d, val + (0 << ADC_MUX_CH_POS));
		send_to_88L24(
				i2c_bus_dev, dev_addr, 0x002e, val + (1 << ADC_MUX_CH_POS));
		send_to_88L24(
				i2c_bus_dev, dev_addr, 0x002f, val + (2 << ADC_MUX_CH_POS));
		send_to_88L24(
				i2c_bus_dev, dev_addr, 0x0030, val + (3 << ADC_MUX_CH_POS));
		break;
	case NAU88L24_Input_type_DMIC:
		val_0x77 |= (FEPGA_MODE_MICP_DMIC_L + FEPGA_MODE_MICP_DMIC_R);
		send_to_88L24(
				i2c_bus_dev, dev_addr, 0x0074, 0x4106);
		break;
	default :
		CRITICAL_ERROR("unsupported or unimplemented input");
		break;
	}

	switch (output_type)
	{
	case NAU88L24_Output_type_headset:
		val_0x76 |= (ENABLE_BIAS + VMID_PRE_CHARGE_DISABLE);
		val_0x7f |= POWERUP_MAIN_DRIVER + POWERUP_OUTPUT_DRIVER;
		break;
	default :
		CRITICAL_ERROR("unsupported or unimplemented input");
		break;
	}
	val_0x76 |= ENABLE_AUTO_SHORT_CIR_DETECT;

	send_to_88L24(i2c_bus_dev, dev_addr, 0x0077, val_0x77);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0078, 0x3c00);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0076, DISCHRG);
	os_delay_ms(1);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0076, val_0x76);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0080,
			(HEADPHONES_POWER_DOWN + ENABLE_CHARGE_PUMP));
	send_to_88L24(i2c_bus_dev, dev_addr, 0x007f, val_0x7f);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0078, 0x00);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0080, ENABLE_CHARGE_PUMP);


}


#define ENABLE_DMIC_POS  6

static void enable_inputs_and_outputs(struct NAU88L24_config_t *cfg_hndl)
{
	uint8_t    num_of_digital_mics;
	uint16_t  reg0x01_val;
	struct dev_desc_t    *i2c_bus_dev;
	uint8_t    dev_addr;

	i2c_bus_dev = cfg_hndl->i2c_bus_dev;
	dev_addr = cfg_hndl->dev_addr;
	num_of_digital_mics = cfg_hndl->num_of_digital_mics;

	reg0x01_val = 0x3f;
	if (NAU88L24_Input_type_DMIC == cfg_hndl->input_type)
	{
		if ((0 == num_of_digital_mics) || (4 < num_of_digital_mics))
		{
			CRITICAL_ERROR("wrong number of digital mics");
		}
		reg0x01_val |= (1 << ENABLE_DMIC_POS);
		if (1 < num_of_digital_mics)
		{
			reg0x01_val |= (1 << (ENABLE_DMIC_POS + 1));
		}
		if (2 < num_of_digital_mics)
		{
			reg0x01_val |= (1 << (ENABLE_DMIC_POS + 2));
		}
		if (3 < num_of_digital_mics)
		{
			reg0x01_val |= (1 << (ENABLE_DMIC_POS + 3));
		}
	}
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0001, 0x003f);
}


static void init_88L24(struct NAU88L24_config_t *cfg_hndl)
{
	struct dev_desc_t    *i2c_bus_dev;
	uint8_t    dev_addr;
	uint16_t i;
	uint16_t reg0x1c;
	uint16_t val;

	i2c_bus_dev = cfg_hndl->i2c_bus_dev;
	dev_addr = cfg_hndl->dev_addr;

	// datasheet recommends to write 0x0000 twice uppon resetting
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0000, 0x0001);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0000, 0x0001);

	send_to_88L24(
			i2c_bus_dev, dev_addr, 0x0066, (ENABLE_VMID + VMID_TIE_OFF_125k));
	os_delay_ms(2);// should be at least 2ms


	send_to_88L24(
			i2c_bus_dev, dev_addr, 0x0067, (MUTE_PGA_L + MUTE_PGA_R));

	set_routing(cfg_hndl);

	for(i = 0;
			i < (sizeof(init_88L24_arr) / sizeof(struct  NAU88L24_cmd_t)); i++)
	{
		struct  NAU88L24_cmd_t const *cmd_88L24;

		cmd_88L24 = &init_88L24_arr[i];
		send_to_88L24(i2c_bus_dev, dev_addr,
					cmd_88L24->reg_addr, cmd_88L24->reg_val);
	}


	os_delay_ms(10);// pop reducing delay
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0067, 0x00);

	val = 0x20 + POWER_ON_ADC_L + POWER_ON_ADC_R;
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0072, val);

	init_88L24_clocks(cfg_hndl);

	reg0x1c = 0x0043; // PCM B mode
	if (16 == cfg_hndl->pcm_data_width)
	{
		reg0x1c |= 0x0000;
	}
	else if (32 == cfg_hndl->pcm_data_width)
	{
		reg0x1c |= 0x000c;
	}
	else
	{
		CRITICAL_ERROR("unsupported pcm data width");
	}
	send_to_88L24(i2c_bus_dev, dev_addr, 0x001c, reg0x1c);

	// start clocks and ADC
	enable_inputs_and_outputs(cfg_hndl);
	send_to_88L24(i2c_bus_dev, dev_addr, 0x0002, 0xf334);
}

/**
 * uart_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t NAU88L24_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct NAU88L24_config_t *cfg_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		init_88L24(cfg_hndl);
		break;

	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME                      NAU88L24
#define	MODULE_IOCTL_FUNCTION            NAU88L24_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct NAU88L24_config_t
#include "add_module.h"
