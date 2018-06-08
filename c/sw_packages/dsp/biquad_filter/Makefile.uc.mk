
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_BIQUAD_FILTER)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)
    ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    endif
endif

#INCLUDE_DIR =  


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = biquad_filter.cpp
SRC += biquad_coefficients.cpp

USE_CORTEX_M_FPU := $(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU))
USE_CORTEX_M_FPU := $(and $(USE_CORTEX_M_FPU),$(CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT))

ifneq ($(USE_CORTEX_M_FPU),)
	SRC += biquads_arm_cortex_m_fpu.cpp
#    SRC += biquads.cpp
	ifdef CONFIG_DSP_IS_SPEED_CRITICAL
		SPEED_CRITICAL_FILES += biquads_arm_cortex_m_fpu.cpp
		SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
	endif

	#DEFINES = ARM_MATH_CM4=1  __FPU_PRESENT=1
	#SRC += biquads.c
	#SRC += arm_biquad_cascade_df2T_f32.c
	#SRC += arm_biquad_cascade_df2T_init_f32.c
	#SRC += arm_abs_f32.c
	#VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/FilteringFunctions
	#VPATH += | $(ARM_CMSIS_PATH)/CMSIS/DSP_Lib/Source/BasicMathFunctions
	#ifdef CONFIG_DSP_IS_SPEED_CRITICAL
	#	SPEED_CRITICAL_FILES += biquads.c
	#	SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_f32.c
	#	SPEED_CRITICAL_FILES += arm_biquad_cascade_df2T_init_f32.c
	#	SPEED_CRITICAL_FILES += arm_abs_f32.c
	#endif
else
    SRC += biquads_c_calc.cpp
    ifdef CONFIG_DSP_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += biquads.cpp
    endif
endif

VPATH = src





ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += biquad_filter.c
endif


include $(COMMON_CC)
