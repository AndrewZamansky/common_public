ifeq ($(sort $(CONFIG_PIC32MX575)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_GPIO)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_pic32mx575.c 
VPATH = src



include $(COMMON_CC)
