
ifeq ($(findstring YES,$(_INCLUDE_USB)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32_USB-FS-Device_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = usb.c 
VPATH = src 

SRC += usb_hal.c
SRC += usb_desc.c
SRC += usb_istr.c
SRC += usb_prop.c
SRC += usb_pwr.c
VPATH +=  | src/USB_STM32F103x





SRC += otgd_fs_cal.c 
SRC += otgd_fs_dev.c 
SRC += otgd_fs_int.c 
SRC += otgd_fs_pcd.c 
SRC += usb_core.c 
SRC += usb_init.c 
SRC += usb_int.c 
SRC += usb_mem.c 
SRC += usb_regs.c 
SRC += usb_sil.c 
VPATH +=  | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32_USB-FS-Device_Driver/src

include $(COMMON_CC)
