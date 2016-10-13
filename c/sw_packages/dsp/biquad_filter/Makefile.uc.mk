
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_BIQUAD_FILTER)



#INCLUDE_DIR =  


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = biquad_filter.c

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        SRC += biquads_arm_cortex_m_fpu.c
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
        ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
            SPEED_CRITICAL_FILES += biquads_arm_cortex_m_fpu.c
            SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
        endif
    else
        DEFINES = ARM_MATH_CM4=1  __FPU_PRESENT=1
        SRC += biquads.c
        SRC += arm_biquad_cascade_df2T_f32.c
        SRC += arm_biquad_cascade_df2T_init_f32.c
        SRC += arm_abs_f32.c
        VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/FilteringFunctions
        VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/BasicMathFunctions
        ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
            SPEED_CRITICAL_FILES += biquads.c
            SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_f32.c
            SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_init_f32.c
            SPEED_CRITICAL_FILES += arm_abs_f32.c
        endif
    endif
else
    SRC += biquads.c
    ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += biquads.c
    endif
endif

VPATH = src





ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += biquad_filter.c
endif


include $(COMMON_CC)
