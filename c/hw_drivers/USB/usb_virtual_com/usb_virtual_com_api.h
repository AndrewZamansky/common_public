
#ifndef _USB_Virtual_COM_API_H_
#define _USB_Virtual_COM_API_H_

/*****************  defines  **************/


/**********  define API  types ************/




/**********  define API  functions  ************/
uint8_t  usb_virtual_com_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#else
#pragma message( "warning : this header file had already been included" )
#endif
