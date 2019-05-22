
#ifndef _SPI_I94XXX_API_H_
#define _SPI_I94XXX_API_H_


/*****************  defines  **************/
#define SPI_I94XXX_API_BASE_ADDRESS_SPI0    (size_t)SPI0_BASE
#define SPI_I94XXX_API_BASE_ADDRESS_SPI1    (size_t)SPI1_BASE


/**********  define API  types ************/

/**********  define API  functions  ************/

enum SPI_I94XXX_API_ioctl_e {
	IOCTL_SPI_API_SET_CS_HIGH = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SPI_API_SET_CS_LOW,
	IOCTL_SPI_API_SET_CLK
};


#endif
