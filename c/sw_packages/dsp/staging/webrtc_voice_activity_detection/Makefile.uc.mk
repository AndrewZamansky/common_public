 
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WEBRTC_VAD)

ifdef CONFIG_INCLUDE_WEBRTC_VAD
    WEBRTC_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/webrtc
    ifeq ("$(wildcard $(WEBRTC_PATH))","")
        $(info   )
        $(info --- webrtc path $(WEBRTC_PATH) dont exists )
        $(info --- get webrtc ported to uCProjects from Andrew Zamansky and unpack it to $(WEBRTC_PATH)  )
        $(info --- make sure that infra directory is located in $(WEBRTC_PATH)/  after unpacking   )
        $(info ---    )
        $(error )
    endif
endif

INCLUDE_DIR +=$(WEBRTC_PATH)

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
VPATH += | $(WEBRTC_PATH)/webrtc/common_audio/vad

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
VPATH += | $(WEBRTC_PATH)/webrtc/common_audio/signal_processing

ifdef CONFIG_WEBRTC_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES +=  webrtc_voice_activity_detection.c

    SPEED_CRITICAL_FILES += webrtc_vad.c
    SPEED_CRITICAL_FILES += vad_core.c
    SPEED_CRITICAL_FILES += vad_filterbank.c
    SPEED_CRITICAL_FILES += vad_gmm.c
    SPEED_CRITICAL_FILES += vad_sp.c

    SPEED_CRITICAL_FILES += spl_init.c
    SPEED_CRITICAL_FILES += min_max_operations.c
    SPEED_CRITICAL_FILES += vector_scaling_operations.c
    SPEED_CRITICAL_FILES += cross_correlation.c
    SPEED_CRITICAL_FILES += downsample_fast.c
    SPEED_CRITICAL_FILES += division_operations.c
    SPEED_CRITICAL_FILES += resample_48khz.c
    SPEED_CRITICAL_FILES += spl_inl.c
    SPEED_CRITICAL_FILES += resample_fractional.c
    SPEED_CRITICAL_FILES += resample_by_2_internal.c
    SPEED_CRITICAL_FILES += energy.c
    SPEED_CRITICAL_FILES += get_scaling_square.c
endif



include $(COMMON_CC)
