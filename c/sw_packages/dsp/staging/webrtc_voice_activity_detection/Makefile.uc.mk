 
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WEBRTC_VAD)

ifdef CONFIG_INCLUDE_WEBRTC_VAD
    #GLOBAL_LIBS += libstdc++.a
endif

INCLUDE_DIR +=$(EXTERNAL_SOURCE_ROOT_DIR)/webrtc

#DEFINES = FLOATING_POINT USE_KISS_FFT
DEFINES = 
#CFLAGS = -std=c++11  -fpermissive

#ASMFLAGS =  




SRC =  webrtc_voice_activity_detection.c
VPATH = src

SRC += webrtc_vad.c
SRC += vad_core.c
SRC += vad_filterbank.c
SRC += vad_gmm.c
SRC += vad_sp.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/common_audio/vad

SRC += spl_init.c
SRC += min_max_operations.c
SRC += vector_scaling_operations.c
SRC += cross_correlation.c
SRC += downsample_fast.c
SRC += division_operations.c
SRC += resample_48khz.c
SRC += spl_inl.c
SRC += resample_fractional.c
SRC += resample_by_2_internal.c
SRC += energy.c
SRC += get_scaling_square.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/common_audio/signal_processing

ifdef CONFIG_WEBRTC_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES = $(SRC)
endif



include $(COMMON_CC)
