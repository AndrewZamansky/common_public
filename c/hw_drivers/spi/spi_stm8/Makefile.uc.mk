ifeq ($(sort $(CONFIG_STM8S103F)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_SPI)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = spi_stm8.c

VPATH = src

SRC += stm8s_spi.c

VPATH += | $(STM8S_SRC_DIR)

include $(COMMON_CC)
