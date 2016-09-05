
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_COMMON_DSP) $(CONFIG_AUTO_INCLUDE_COMMON_DSP)

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = -O3

#ASMFLAGS =  

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter
INCLUDE_DIR += $(SW_PACKAGES_ROOT_DIR)/dsp/ffmpeg

ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)

    GLOBAL_LIBS += libarm_cortexM4lf_math.a
	ARM_CMSIS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS
    ifeq ("$(wildcard $(ARM_CMSIS_PATH))","")
        $(info ARM CMSIS path $(ARM_CMSIS_PATH) dont exists )
        $(info download ARM CMSIS version 3.01 and unpack it to $(ARM_CMSIS_PATH)  )
        $(info make sure that file Version 3.01 is located in $(ARM_CMSIS_PATH)/  after unpacking   )
        $(error )
    endif

    GLOBAL_LIBS_PATH += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Lib/GCC
    #GLOBAL_LIBS_PATH += $(EXTERNAL_SOURCE_ROOT_DIR)/ARM-CMSIS/CMSIS/Lib/G++
endif

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
ifdef CONFIG_AUTO_COMMON_DSP_IS_SPEED_CRITICAL
    _COMMON_DSP_IS_SPEED_CRITICAL:=YES
endif

ifdef _COMMON_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
    SPEED_CRITICAL_FILES += biquads.c
    SPEED_CRITICAL_FILES += math_functions.c
#    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_f32.c
#    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_init_f32.c
#    SPEED_CRITICAL_FILES += arm_abs_f32.c
endif




include $(COMMON_CC)
