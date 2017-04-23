
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DOWNSAMPLING_BY_INT)

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



SRC = downsampling_by_int.c

ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
	SRC += downsampling_by_int_arm_cortex_m_fpu.c
	ifdef CONFIG_DOWNSAMPLING_BY_INT_IS_SPEED_CRITICAL
		SPEED_CRITICAL_FILES += downsampling_by_int_arm_cortex_m_fpu.c
		SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
	endif
else
endif

VPATH = src





ifdef CONFIG_DOWNSAMPLING_BY_INT_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += downsampling_by_int.c
endif


include $(COMMON_CC)
