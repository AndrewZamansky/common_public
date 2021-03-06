#ifndef DT__PIN_CONTROL_STM32F10X_H_
#define _DT_PIN_CONTROL_STM32F10X_H_

#define PORT_A  0
#define PORT_B  1
#define PORT_C  2
#define PORT_D  3

#define PIN_0   0
#define PIN_1   1
#define PIN_2   2
#define PIN_3   3
#define PIN_4   4
#define PIN_5   5
#define PIN_6   6
#define PIN_7   7
#define PIN_8   8
#define PIN_9   9
#define PIN_10  10
#define PIN_11  11
#define PIN_12  12
#define PIN_13  13
#define PIN_14  14
#define PIN_15  15

#define DUMMY_AF_REMAP_REG   0x20  // in case no pin remap possible
#define PIN_POS                  4
#define PIN_MASK                 0xf0
#define PORT_POS                 8
#define PORT_MASK                0x300
#define AF_MAP_REG_POS           10
#define AF_MAP_FIELD_PLACE_POS   14
#define AF_MAP_FIELD_SIZE_POS    19
#define AF_MAP_FUNC_POS          23

#define PIN_TYPE_FLOATING_INPUT             0x4
#define PIN_TYPE_ANALOG                     0x0
#define PIN_TYPE_ALT_FUNC_PUSH_PULL_50MHZ   0xB

#define FILL_PIN_DATA(port, pin, pin_func, af_remap_reg,\
						af_field_pos, af_field_size, af_remap_fun) \
	(pin_func | (pin << PIN_POS) |            \
	(port << PORT_POS)  |                      \
	(af_remap_reg << AF_MAP_REG_POS)   |       \
	(af_field_pos << AF_MAP_FIELD_PLACE_POS) | \
	(af_field_size << AF_MAP_FIELD_SIZE_POS) | \
	(af_remap_fun << AF_MAP_FUNC_POS))


/*****************************************/
/************  PORT A ********************/
#define PIN_CONTROL_DT_STM32F10X_PIN_A00_GPIO   FILL_PIN_DATA(PORT_A, PIN_0,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)
#define PIN_CONTROL_DT_STM32F10X_PIN_A00_ADC    FILL_PIN_DATA(PORT_A, PIN_0,  \
			PIN_TYPE_ANALOG, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A01_GPIO   FILL_PIN_DATA(PORT_A, PIN_1,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A02_GPIO   FILL_PIN_DATA(PORT_A, PIN_2,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)
#define PIN_CONTROL_DT_STM32F10X_PIN_A02_ADC    FILL_PIN_DATA(PORT_A, PIN_2,  \
			PIN_TYPE_ANALOG, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A03_GPIO   FILL_PIN_DATA(PORT_A, PIN_3,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A04_GPIO   FILL_PIN_DATA(PORT_A, PIN_4,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A05_GPIO   FILL_PIN_DATA(PORT_A, PIN_5,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A06_GPIO   FILL_PIN_DATA(PORT_A, PIN_6,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A07_GPIO   FILL_PIN_DATA(PORT_A, PIN_7,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A08_GPIO   FILL_PIN_DATA(PORT_A, PIN_8,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)
#define PIN_CONTROL_DT_STM32F10X_PIN_A08_TIMER  FILL_PIN_DATA(PORT_A, PIN_8, \
			PIN_TYPE_ALT_FUNC_PUSH_PULL_50MHZ, DUMMY_AF_REMAP_REG, 0, 0, 0)
#define PIN_CONTROL_DT_STM32F10X_PIN_A08_PWM    FILL_PIN_DATA(PORT_A, PIN_8, \
			PIN_TYPE_ALT_FUNC_PUSH_PULL_50MHZ, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A09_GPIO   FILL_PIN_DATA(PORT_A, PIN_9,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A10_GPIO   FILL_PIN_DATA(PORT_A, PIN_10, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A11_GPIO   FILL_PIN_DATA(PORT_A, PIN_11, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A12_GPIO   FILL_PIN_DATA(PORT_A, PIN_12, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A13_GPIO   FILL_PIN_DATA(PORT_A, PIN_13, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A14_GPIO   FILL_PIN_DATA(PORT_A, PIN_14, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_A15_GPIO   FILL_PIN_DATA(PORT_A, PIN_15, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)



/*****************************************/
/************  PORT B ********************/
#define PIN_CONTROL_DT_STM32F10X_PIN_B00_GPIO   FILL_PIN_DATA(PORT_B, PIN_0,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B01_GPIO   FILL_PIN_DATA(PORT_B, PIN_1,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B02_GPIO   FILL_PIN_DATA(PORT_B, PIN_2,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B03_GPIO   FILL_PIN_DATA(PORT_B, PIN_3,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B04_GPIO   FILL_PIN_DATA(PORT_B, PIN_4,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B05_GPIO   FILL_PIN_DATA(PORT_B, PIN_5,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B06_GPIO   FILL_PIN_DATA(PORT_B, PIN_6,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B07_GPIO   FILL_PIN_DATA(PORT_B, PIN_7,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B08_GPIO   FILL_PIN_DATA(PORT_B, PIN_8,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B09_GPIO   FILL_PIN_DATA(PORT_B, PIN_9,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B10_GPIO   FILL_PIN_DATA(PORT_B, PIN_10, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B11_GPIO   FILL_PIN_DATA(PORT_B, PIN_11, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B12_GPIO   FILL_PIN_DATA(PORT_B, PIN_12, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B13_GPIO   FILL_PIN_DATA(PORT_B, PIN_13, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B14_GPIO   FILL_PIN_DATA(PORT_B, PIN_14, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_B15_GPIO   FILL_PIN_DATA(PORT_B, PIN_15, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)




/*****************************************/
/************  PORT C ********************/
#define PIN_CONTROL_DT_STM32F10X_PIN_C00_GPIO   FILL_PIN_DATA(PORT_C, PIN_0,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C01_GPIO   FILL_PIN_DATA(PORT_C, PIN_1,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C02_GPIO   FILL_PIN_DATA(PORT_C, PIN_2,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C03_GPIO   FILL_PIN_DATA(PORT_C, PIN_3,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C04_GPIO   FILL_PIN_DATA(PORT_C, PIN_4,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C05_GPIO   FILL_PIN_DATA(PORT_C, PIN_5,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C06_GPIO   FILL_PIN_DATA(PORT_C, PIN_6,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C07_GPIO   FILL_PIN_DATA(PORT_C, PIN_7,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C08_GPIO   FILL_PIN_DATA(PORT_C, PIN_8,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C09_GPIO   FILL_PIN_DATA(PORT_C, PIN_9,  \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C10_GPIO   FILL_PIN_DATA(PORT_C, PIN_10, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C11_GPIO   FILL_PIN_DATA(PORT_C, PIN_11, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C12_GPIO   FILL_PIN_DATA(PORT_C, PIN_12, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C13_GPIO   FILL_PIN_DATA(PORT_C, PIN_13, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C14_GPIO   FILL_PIN_DATA(PORT_C, PIN_14, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#define PIN_CONTROL_DT_STM32F10X_PIN_C15_GPIO   FILL_PIN_DATA(PORT_C, PIN_15, \
			PIN_TYPE_FLOATING_INPUT, DUMMY_AF_REMAP_REG, 0, 0, 0)

#endif
