
ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_USB_DEVICE)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = usb_i94xxx.c

VPATH = src

SRC += usbd.c

VPATH += | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
