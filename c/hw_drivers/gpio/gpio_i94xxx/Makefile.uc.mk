ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_GPIO)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_i94xxx.c 
VPATH = src

SRC += gpio.c
VPATH += | $(I94XXX_SRC_DIR)


include $(COMMON_CC)
