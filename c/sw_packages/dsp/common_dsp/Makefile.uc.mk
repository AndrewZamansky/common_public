
ifeq ($(findstring YES,$(_INCLUDE_COMMON_DSP)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter
INCLUDE_DIR += $(SW_PACKAGES_ROOT_DIR)/dsp/ffmpeg

GLOBAL_LIBS += libarm_cortexM4lf_math.a
GLOBAL_LIBS_PATH += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Lib/GCC

SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a

SPEED_CRITICAL_FILES += biquads.c


SRC = biquads.c

VPATH = src


include $(COMMON_CC)
