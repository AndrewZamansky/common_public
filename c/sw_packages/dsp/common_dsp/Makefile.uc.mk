
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_COMMON_DSP) $(CONFIG_AUTO_INCLUDE_COMMON_DSP)

#INCLUDE_DIR =

#DEFINES =

#CFLAGS =

#ASMFLAGS =


ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)

    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    endif

endif

SRC = biquads.c
SRC += math_functions.c
VPATH = src

ifndef CONFIG_INCLUDE_CORTEX_M_FPU
    DEFINES = ARM_MATH_CM4=1  __FPU_PRESENT=1
    SRC += arm_biquad_cascade_df2T_f32.c
    SRC += arm_biquad_cascade_df2T_init_f32.c
    SRC += arm_abs_f32.c
    VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/FilteringFunctions
    VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/BasicMathFunctions
endif


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
    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_f32.c
    SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_init_f32.c
    SPEED_CRITICAL_FILES += arm_abs_f32.c
endif


include $(COMMON_CC)
