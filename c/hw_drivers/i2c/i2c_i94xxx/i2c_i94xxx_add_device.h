/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "i2c_i94xxx_api.h"
#include "src/i2c_i94xxx.h"


#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct i2c_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE struct i2c_i94xxx_runtime_t


#ifndef I2C_I94XXX_DT_BASE_ADDRESS
	#error "I2C_I94XXX_DT_BASE_ADDRESS should be defined"
#endif

#ifndef I2C_I94XXX_DT_MODE
	#error "I2C_I94XXX_DT_BASE_ADDRESS should be defined"
#endif

#ifdef I2C_I94XXX_DT_TX_CALLBACK_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
						I2C_I94XXX_DT_TX_CALLBACK_PDEV) ;
	#define POINTER_TO_TX_CALLBACK_PDEV        \
			P_TO_STATIC_DEVICE_INST(I2C_I94XXX_DT_TX_CALLBACK_PDEV)
#else
	#warning "I2C_I94XXX_DT_TX_CALLBACK_PDEV not defined in device tree"
	#define POINTER_TO_TX_CALLBACK_PDEV        NULL
#endif

#ifdef I2C_I94XXX_DT_RX_CALLBACK_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
						I2C_I94XXX_DT_RX_CALLBACK_PDEV) ;
	#define POINTER_TO_RX_CALLBACK_PDEV        \
	P_TO_STATIC_DEVICE_INST(I2C_I94XXX_DT_RX_CALLBACK_PDEV)
#else
	#warning "I2C_I94XXX_DT_RX_CALLBACK_PDEV not defined in device tree"
	#define POINTER_TO_RX_CALLBACK_PDEV        NULL
#endif

#ifndef I2C_I94XXX_DT_BAUD_RATE
	#error "I2C_I94XXX_DT_BAUD_RATE should be defined"
#endif

#ifndef I2C_I94XXX_DT_SRC_CLOCK_PDEV
	#error "I2C_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
#endif
#define POINTER_TO_SRC_CLOCK_PDEV     \
		P_TO_STATIC_DEVICE_INST(I2C_I94XXX_DT_SRC_CLOCK_PDEV)

#ifndef I2C_I94XXX_DT_SCL_PINOUT
	#error "I2C_I94XXX_DT_SCL_PINOUT should be defined"
#endif

#ifndef I2C_I94XXX_DT_SDA_PINOUT
	#error "I2C_I94XXX_DT_SDA_PINOUT should be defined"
#endif

#if (I2C_I94XXX_DT_MODE == I2C_I94XXX_API_SLAVE_MODE)
	#ifndef I2C_I94XXX_SLAVE_ADDRESS
		#error "I2C_I94XXX_SLAVE_ADDRESS should be defined"
	#endif
	#define  _I2C_I94XXX_DT_SLAVE_ADDR  I2C_I94XXX_SLAVE_ADDRESS
#else
	#define  _I2C_I94XXX_DT_SLAVE_ADDR  0x00
#endif

#define STATIC_DEV_DATA_STRUCT           \
	{                                    \
		POINTER_TO_TX_CALLBACK_PDEV ,    \
		POINTER_TO_RX_CALLBACK_PDEV ,    \
		POINTER_TO_SRC_CLOCK_PDEV   ,    \
		I2C_I94XXX_DT_BASE_ADDRESS ,     \
		I2C_I94XXX_DT_BAUD_RATE,         \
		I2C_I94XXX_DT_MODE,              \
		I2C_I94XXX_DT_SCL_PINOUT,        \
		I2C_I94XXX_DT_SDA_PINOUT,        \
		_I2C_I94XXX_DT_SLAVE_ADDR,       \
	}


#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef I2C_I94XXX_DT_MODE
#undef POINTER_TO_SRC_CLOCK_PDEV
#undef POINTER_TO_TX_CALLBACK_PDEV
#undef POINTER_TO_RX_CALLBACK_PDEV
#undef I2C_I94XXX_DT_I2C_NUMBER
#undef I2C_I94XXX_DT_CALLBACK_PDEV
#undef I2C_I94XXX_DT_BAUD_RATE
#undef I2C_I94XXX_DT_SRC_CLOCK_PDEV
#undef I2C_I94XXX_SLAVE_ADDRESS
#undef I2C_I94XXX_DT_SCL_PINOUT
#undef I2C_I94XXX_DT_SDA_PINOUT
#undef _I2C_I94XXX_DT_SLAVE_ADDR
