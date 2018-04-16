ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_SPI)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC += spi_i94xxx.c
VPATH += src

SRC += spi.c
VPATH += | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
