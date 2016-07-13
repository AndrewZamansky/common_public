
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SPEEX_ECHO_CANCELLER)),YES) 	 
    _INCLUDE_COMMON_DSP :=YES	 
    _INCLUDE_DSP_MANAGEMENT :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR =$(EXTERNAL_SOURCE_ROOT_DIR)/speex/speexdsp/include
INCLUDE_DIR +=$(EXTERNAL_SOURCE_ROOT_DIR)/speex/speexdsp/include/speex
INCLUDE_DIR +=src

#DEFINES = FLOATING_POINT USE_KISS_FFT
DEFINES = FLOATING_POINT USE_ARM_M4_FFT

#CFLAGS = 

#ASMFLAGS =  




SRC = speex_echo_canceller.c
VPATH = src

SRC += mdf.c
SRC += preprocess.c
SRC += fftwrap.c
#SRC += kiss_fft.c
#SRC += kiss_fftr.c
SRC += filterbank.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/speex/speexdsp/libspeexdsp

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_SPEEX_ECHO_CANCELLER)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += speex_echo_canceller.c
    SPEED_CRITICAL_FILES += mdf.c
    SPEED_CRITICAL_FILES += preprocess.c
    SPEED_CRITICAL_FILES += fftwrap.c
#    SPEED_CRITICAL_FILES += kiss_fft.c
    SPEED_CRITICAL_FILES += filterbank.c
    _COMMON_DSP_IS_SPEED_CRITICAL := YES
endif




include $(COMMON_CC)
