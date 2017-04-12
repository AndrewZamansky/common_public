
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_STANDARD_COMPRESSOR_WITH_LOG)

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  





ifdef CONFIG_CORTEX_M4
#    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
#        SRC += standard_compressor_wLog_arm_cortex_m_fpu.c
#        GLOBAL_LIBS += libarm_cortexM4lf_math.a
#        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
#        ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
#            SPEED_CRITICAL_FILES += standard_compressor_wLog_arm_cortex_m_fpu.c
#            SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
#        endif
#    else
        SRC += standard_compressor_wLog.c
        ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
            SPEED_CRITICAL_FILES += standard_compressor_wLog.c
        endif
#    endif
else
    SRC += standard_compressor_wLog.c
    ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += standard_compressor_wLog.c
    endif
endif


VPATH = src


include $(COMMON_CC)
