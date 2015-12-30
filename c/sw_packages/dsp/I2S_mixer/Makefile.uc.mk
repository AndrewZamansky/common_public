
ifeq ($(findstring YES,$(CONFIG_INCLUDE_I2S_MIXER)),YES) 
    _INCLUDE_COMMON_DSP :=YES	 
    _INCLUDE_DSP_MANAGMENT :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include



#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_I2S_MIXER)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += I2S_mixer.c
endif

SRC = I2S_mixer.c

VPATH = src


include $(COMMON_CC)
