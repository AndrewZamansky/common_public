
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_FIR_FILTER)

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



SRC = fir_filter.c

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        SRC += fir_arm_cortex_m_fpu.c
        ifdef CONFIG_DSP_IS_SPEED_CRITICAL
            SPEED_CRITICAL_FILES += fir_arm_cortex_m_fpu.c
            SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
            #in new versions of CMSIS lib path is different
            GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/DSP/Lib/GCC
        endif
    endif
else
endif

VPATH = src





ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += fir_filter.c
endif


include $(COMMON_CC)
