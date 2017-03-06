/*
 * file : ESP8266.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ESP8266_H
#define _ESP8266_H


/***************   typedefs    *******************/


typedef struct {
	pdev_descriptor_t   timer_dev;
	pdev_descriptor_t   uart_rx_dev;
	pdev_descriptor_t   uart_tx_dev;
} esp8266_instance_t;


#endif /* */
