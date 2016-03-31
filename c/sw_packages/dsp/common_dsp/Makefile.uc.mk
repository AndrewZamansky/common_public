
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_COMMON_DSP)

ifeq ($(findstring YES,$(_INCLUDE_COMMON_DSP)),YES) 	 
	INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_COMMON_DSP) y   
endif  


#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = -O3

#ASMFLAGS =  

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter
INCLUDE_DIR += $(SW_PACKAGES_ROOT_DIR)/dsp/ffmpeg

GLOBAL_LIBS += libarm_cortexM4lf_math.a
GLOBAL_LIBS_PATH += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Lib/GCC
#GLOBAL_LIBS_PATH += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Lib/G++

SRC = biquads.c
SRC += math_functions.c
VPATH = src

#DEFINES = ARM_MATH_CM4=1  __FPU_PRESENT=1  __FPU_USED=1
#INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/Device/ARM/ARMCM4/Include
#SRC += arm_biquad_cascade_df2T_f32.c
#SRC += arm_biquad_cascade_df2T_init_f32.c
#SRC += arm_abs_f32.c
#VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/DSP_Lib/Source/FilteringFunctions
#VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/DSP_Lib/Source/BasicMathFunctions

ifdef CONFIG_COMMON_DSP_IS_SPEED_CRITICAL
    _COMMON_DSP_IS_SPEED_CRITICAL:=YES
endif

ifdef ($(findstring YES,$(_COMMON_DSP_IS_SPEED_CRITICAL)),YES) 
    SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
    SPEED_CRITICAL_FILES += biquads.c
    SPEED_CRITICAL_FILES += math_functions.c
#    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_f32.c
#    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_init_f32.c
#    SPEED_CRITICAL_FILES += arm_abs_f32.c
endif




include $(COMMON_CC)
