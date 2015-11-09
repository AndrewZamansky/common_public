
ifeq ($(findstring YES,$(CONFIG_INCLUDE_USB_VIRTUAL_COM)),YES) 	 
	_INCLUDE_USB := YES
	_INCLUDE_UART := YES
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR = $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32F10x_StdPeriph_Driver/inc
INCLUDE_DIR += $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32_USB-FS-Device_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = usb_virtual_com_stm32f103x.c


VPATH = src

include $(COMMON_CC)
