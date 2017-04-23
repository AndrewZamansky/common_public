
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_NOISE_GATE)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)
    ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    endif
endif

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = noise_gate_ioctl.c

ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
    SRC += noise_gate_arm_cortex_m_fpu.c
    ifdef CONFIG_NOISE_GATE_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += noise_gate_arm_cortex_m_fpu.c
        SPEED_CRITICAL_FILES += libarm_cortexM4lf_math.a
    endif
else
    SRC += noise_gate.c
    ifdef CONFIG_BIQUAD_FILTER_IS_SPEED_CRITICAL
        SPEED_CRITICAL_FILES += noise_gate.c
    endif
endif


VPATH = src


include $(COMMON_CC)
