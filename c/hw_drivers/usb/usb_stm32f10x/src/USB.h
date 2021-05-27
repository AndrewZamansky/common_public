/*
 * USB.h
 *
 */

#ifndef _USB_H
#define _USB_H


struct USB_Instance_t {
	USB_API_Chip_t chip;
	uint8_t do_resume;
//	SystemTick_API_Timer_Handle_t  usb_stabilize_timer;
};

extern void 	USB_STM32F103x_Init(void);

#endif
