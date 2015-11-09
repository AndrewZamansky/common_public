
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SPI_FLASH_prev)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


INCLUDE_DIR = $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = SPI_FLASH.c 
SRC += SPI_FLASH_stm32f103xx.c

ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 
 SRC += cmd_spi_format.c
endif

VPATH = src

SRC += stm32f10x_spi.c
VPATH +=  | $(DRIVERS_ROOT_DIR)/Obtainable/ST/STM32F10x_StdPeriph_Driver/src

include $(COMMON_CC)
