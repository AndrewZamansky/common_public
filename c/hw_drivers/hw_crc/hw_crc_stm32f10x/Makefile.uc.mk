
ifeq ($(sort $(CONFIG_STM32F10X)),y)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_HW_CRC)),YES) 	 
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = hw_crc_stm32f10x.c

VPATH = src

SRC += stm32f10x_crc.c
VPATH +=  | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/src

include $(COMMON_CC)
