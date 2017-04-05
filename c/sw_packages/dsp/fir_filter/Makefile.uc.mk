
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_FIR_FILTER)



#INCLUDE_DIR =  


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = fir_filter.c

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        SRC += fir_arm_cortex_m_fpu.c
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
        ifdef CONFIG_FIR_FILTER_IS_SPEED_CRITICAL
            SPEED_CRITICAL_FILES += fir_arm_cortex_m_fpu.c
            SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
        endif
    endif
else
endif

VPATH = src





ifdef CONFIG_FIR_FILTER_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += fir_filter.c
endif


include $(COMMON_CC)