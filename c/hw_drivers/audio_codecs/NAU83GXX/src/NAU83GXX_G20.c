/*
 *  NAU83GXX_G20.c
 */

static const struct NAU83GXX_reg_s cmd_83G20a_init[] = {
//-- 2-Byte Address -- 2-Byte Data -----//
	{0x0000, {0x00, 0x01} }, // needs TWO writes (any value), not
	{0x0000, {0x00, 0x01} }, // like described in datasheet (one write)
	{0x0002, {0x00, 0x01} }, // Latch I2C address on GPIO's
	{0x0004, {0x07, 0x00} }, // Block MCLK
	{0x001A, {0x00, 0x10} }, // Stall DSP
	{0x000E, {0x00, 0x00} }, // Default I2S setup
//-------
//	{0x000C, {0x0c, 0x24} }, // ADCOUT channels
	{0x0014, {0x01, 0x62} }, // Set I-sense digital gain to +12.25dB vgain = 1 (?)
	{0x0076, {0x40, 0x00} }, // Set I-sense PGA output stage Bias
	{0x0077, {0x07, 0x80} }, // Set I-sense PGA Bias Current to minimum
	{0x007F, {0x00, 0x30} }, // Set I-sense PGA to +12dB gain
	{0x0028, {0x80, 0x01} }, // Set normal I2S with 64x ADC OSR
	{0x0029, {0x00, 0x80} }, // Set normal 64x DAC OSR
	{0x0007, {0x00, 0x95} }, // Enable SAR ADC with 4usec sample time, 70kOhm and 0.5 usec compare cycle
	{0x002C, {0x20, 0xE2} }, // Set ALC gain limits
	{0x002D, {0x53, 0x50} }, // Set ALC settings
	{0x002E, {0xC6, 0xE0} }, // Enable ALC & set threshold & mode
	{0x002F, {0x00, 0xFF} }, // Set segment threshold
	{0x0040, {0x08, 0x01} }, // Set target sample rate
	{0x0004, {0x07, 0x07} }, // Enable DAC & ADC's
	{0x0030, {0x7d, 0x00} }, // Set Isense Temperature Coefficient
	{0x0033, {0xbd, 0xc0} }, // Set Isense TC low pas filter
	{0x0018, {0x80, 0x00} }, // Set TC EN
	{0x0073, {0x00, 0x08} }, // Set DAC reference
	{0x0063, {0x00, 0x30} }, // Set DAC reference capacitors
	{0x0064, {0x0c, 0x1b} }, // Set slew rate
	{0x006B, {0x00, 0x70} }, // Disable UVLOP comparator and set threshold
	{0x0031, {0x00, 0x48} }, // Set UVLOP attack, release & hold time, disable UVLOP gain limiter
	{0x0032, {0x00, 0x00} }, // Set UVLOP minimum gain to no change
	{0x0060, {0x00, 0x00} }, // VMID resistor selection
	{0x0068, {0x06, 0x40} }, // VREF Bandgap buffer ON, I/V Sense Ref Buffer Setting -1.5dB
	{0x0001, {0x00, 0x00} }, // Soft Reset
	{0x0001, {0x00, 0x00} }, // Soft Reset
#if BYTES_PER_PCM_CHANNEL == 2
//	{0x000D, {0x00, 0x02} }, // I2S Port Word Length = 16bit
#endif
	{0x0061, {0x55, 0x55} }, // Enable Bias, DAC & ADC's with clock gating
	{0x0062, {0x00, 0x14} }, // Enable Class-D & disable fast reference power up with clock gating
	{0x0003, {0x08, 0xE0} }, // Set MCLK, ADC & DAC clock dividers
	{0x0068, {0x06, 0x4F} }, // VREF Bandgap buffer ON, I/V Sense Ref Buffer Setting -1.5dB
	{0x0004, {0x00, 0x67} }, //  // DSP_CLK = (MCLK/2)*16 ; get MCLK from MCLK_PIN
//	{0x0001, {0x00, 0x00} }, // Soft Reset
//	{0x0001, {0x00, 0x00} }, // Soft Reset
//	{0xDDDD, {   0,   63} }, // Firmware Delay
//	{0x001A, {0x00, 0x20} }  //   enable DSP output
};

uint16_t cmd_83G20a_init_size = sizeof(cmd_83G20a_init);

static uint8_t G10_G20_set_pcm_ctl(struct NAU83GXX_config_t *config_handle,
								struct dev_desc_t * i2c_dev, uint8_t dev_addr);
static void G10_G20_enable_shutdown_interrupt(
				struct NAU83GXX_config_t *config_handle,
				struct dev_desc_t * i2c_dev, uint8_t dev_addr);


static uint8_t init_G20_regs(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t dev_addr;
	struct dev_desc_t * i2c_dev;
	uint8_t rc;

	dev_addr = runtime_handle->dev_addr;
	i2c_dev = config_handle->i2c_dev;

	rc = NAU83GXX_send_register_array(
			i2c_dev, dev_addr, cmd_83G20a_init, cmd_83G20a_init_size);
	if (rc) return rc;

	rc = G10_G20_set_pcm_ctl(config_handle, i2c_dev, dev_addr);
	if (rc) return rc;

	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000); // soft reset
	if (rc) return rc;

	if (config_handle->enable_recovery)
	{
		G10_G20_enable_shutdown_interrupt(config_handle, i2c_dev, dev_addr);
	}

	// write 2 times to 0x00 for states reset
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000);
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000);
	if (rc) return rc;
	os_delay_ms(63);
	return nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x1A, 0x0020);
}
