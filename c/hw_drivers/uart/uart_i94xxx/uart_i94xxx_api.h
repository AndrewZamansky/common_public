
#ifndef _UART_I94XXX_API_H_
#define _UART_I94XXX_API_H_

#include "I94100.h"

/*****************  defines  **************/

struct uart_i94xxx_cfg_t {
	struct dev_desc_t *   callback_tx_dev;
	struct dev_desc_t *   callback_rx_dev;
	struct dev_desc_t *   src_clock;
	uint32_t baud_rate;
	uint32_t tx_pin;
	uint32_t rx_pin;
};

/**********  define API  types ************/



/**********  define API  functions  ************/

#endif
