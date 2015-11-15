ifeq ($(findstring nuc505,$(CONFIG_SOC_TYPE)),nuc505)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_UART)),YES) 	 
	    _INCLUDE_UART := YES
	    DYNAMIC_COMPONENT := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif
  

INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/Device/Nuvoton/NUC505Series/Include

#INCLUDE_DIR = $(DRIVERS_ROOT_DIR)/Obtainable/ST/nuc505s-periphs/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = uart_nuc505.c

VPATH = src

SRC += uart.c
SRC += sys.c

VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/src

include $(COMMON_CC)
