
#ifndef _SPI_I94XXX_API_H_
#define _SPI_I94XXX_API_H_


/*****************  defines  **************/
#define SPI_I94XXX_API_BASE_ADDRESS_SPI0    (size_t)SPI0_BASE
#define SPI_I94XXX_API_BASE_ADDRESS_SPI1    (size_t)SPI1_BASE


/**********  define API  types ************/

/**********  define API  functions  ************/

typedef enum
{
	IOCTL_SPI_API_SET_CS_HIGH = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SPI_API_SET_CS_LOW,
	IOCTL_SPI_API_SET_CLK
} SPI_I94XXX_API_ioctl_t;

typedef enum
{
	SPI_I94XXX_API_CLK_PIN_A5,
	SPI_I94XXX_API_CLK_PIN_C12
} SPI_I94XXX_CLK_PINOUT_t;

typedef enum
{
	SPI_I94XXX_API_SS_PIN_A6,
	SPI_I94XXX_API_SS_PIN_C7
} SPI_I94XXX_SS_PINOUT_t;

typedef enum
{
	SPI_I94XXX_API_MISO_PIN_A4,
	SPI_I94XXX_API_MISO_PIN_C11
} SPI_I94XXX_MISO_PINOUT_t;

typedef enum
{
	SPI_I94XXX_API_MOSI_PIN_A3,
	SPI_I94XXX_API_MOSI_PIN_C10
} SPI_I94XXX_MOSI_PINOUT_t;

#endif
