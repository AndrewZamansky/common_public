/*
 *  NAU83GXX_G60.c
 */


static const struct NAU83GXX_reg_s cmd_83G60_init[] = {
	//-- 2-Byte Address -- 2-Byte Data -----//
	{0x0000, {0x00, 0x01} }, // needs TWO writes (any value), not
	{0x0000, {0x00, 0x01} }, // like described in datasheet (one write)
};

uint16_t cmd_83G60_init_size = sizeof(cmd_83G60_init);


static void run_G60_OCP_recovery(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
}


static uint8_t init_G60_regs(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t dev_addr;
	struct dev_desc_t * i2c_dev;

	dev_addr = runtime_handle->dev_addr;
	i2c_dev = config_handle->i2c_dev;

	return NAU83GXX_send_register_array(
			i2c_dev, dev_addr, cmd_83G60_init, cmd_83G60_init_size);
}
