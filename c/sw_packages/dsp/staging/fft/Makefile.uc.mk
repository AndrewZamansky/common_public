
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_FFT)


#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




SRC = fft.c

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += fft_arm_cortex_m_fpu.c
endif

VPATH = src


include $(COMMON_CC)
