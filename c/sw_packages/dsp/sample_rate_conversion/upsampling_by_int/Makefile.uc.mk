
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_UPSAMPLING_BY_INT)

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



SRC = upsampling_by_int.c

ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
    SRC += upsampling_by_int_arm_cortex_m_fpu.c
    ifdef CONFIG_DSP_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += upsampling_by_int_arm_cortex_m_fpu.c
        SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
        #in new versions of CMSIS lib path is different
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/DSP/Lib/GCC
	endif
else
endif

VPATH = src





ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += upsampling_by_int.c
endif


include $(COMMON_CC)
