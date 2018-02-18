ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_I2C)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC += i2c_i94xxx.c

VPATH = src

SRC += i2c.c

VPATH += | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
