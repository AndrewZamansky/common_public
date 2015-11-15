
ifeq ($(findstring YES,$(CONFIG_INCLUDE_USB_MASS_STORAGE)),YES) 	 
	_INCLUDE_USB := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR = $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32F10x_StdPeriph_Driver/inc
INCLUDE_DIR += $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32_USB-FS-Device_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



# mass storage
SRC = usb_bot.c
SRC += usb_scsi.c
SRC += memory.c
SRC += scsi_data.c
SRC += USB_Mass_Storage.c

VPATH = src


include $(COMMON_CC)
