
ifeq ($(findstring YES,$(CONFIG_INCLUDE_EQUALIZER)),YES) 
    _INCLUDE_COMMON_DSP :=YES	 
    _INCLUDE_DSP_MANAGMENT :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR =  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include

INCLUDE_DIR +=$(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter
#INCLUDE_DIR += $(SW_PACKAGES_ROOT_DIR)/dsp/ffmpeg


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_EQUALIZER)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += equalizer.c
endif

SRC = equalizer.c


VPATH = src


include $(COMMON_CC)
