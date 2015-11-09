ifeq ($(findstring nuc505,$(CONFIG_SOC_TYPE)),nuc505)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_I2S)),YES) 	 
	    DYNAMIC_COMPONENT := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif

INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/Device/Nuvoton/NUC505Series/Include

#INCLUDE_DIR = $(DRIVERS_ROOT_DIR)/Obtainable/ST/nuc505s-periphs/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = I2S_nuc505.c
ifeq ($(findstring YES,$(CONFIG_USE_SHELL)),YES) 	 
 INCLUDE_DIR +=  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
 INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include
 SRC += cmd_set_i2s_loopback.c
 DEFINES += CONFIG_USE_SHELL
endif

VPATH = src

SRC += i2s.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/src

include $(COMMON_CC)
