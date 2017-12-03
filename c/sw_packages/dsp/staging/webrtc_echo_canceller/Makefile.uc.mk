 
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WEBRTC_AEC)

ifdef CONFIG_INCLUDE_WEBRTC_AEC
    WEBRTC_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/webrtc
    ifeq ("$(wildcard $(WEBRTC_PATH))","")
        $(info   )
        $(info --- webrtc path $(WEBRTC_PATH) dont exists )
        $(info --- get webrtc ported to uCProjects from Andrew Zamansky and unpack it to $(WEBRTC_PATH)  )
        $(info --- make sure that infra directory is located in $(WEBRTC_PATH)/  after unpacking   )
        $(info ---    )
        $(error )
    endif

    ifeq ("","$(filter $(WEBRTC_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(WEBRTC_PATH)
    endif

    GLOBAL_LIBS += libstdc++.a
endif

INCLUDE_DIR +=$(WEBRTC_PATH)

#DEFINES = FLOATING_POINT USE_KISS_FFT
DEFINES = WEBRTC_POSIX pthread_t=int pthread_mutex_t=int pthread_cond_t=int 
DEFINES += pthread_mutexattr_t=int M_PI=3.14159265359f
DEFINES += 'pthread_mutexattr_init(...)='
DEFINES += 'pthread_mutexattr_settype(...)='
DEFINES += 'pthread_mutex_init(...)='
DEFINES += 'pthread_mutexattr_destroy(...)='
DEFINES += 'pthread_mutex_destroy(...)='
DEFINES += 'pthread_mutex_lock(...)='
DEFINES += 'pthread_mutex_trylock(...)=1'
DEFINES += 'pthread_mutex_unlock(...)='
DEFINES += 'nanosleep(...)='
DEFINES += WEBRTC_AEC_DEBUG_DUMP=0

CFLAGS = -std=c++11  -fpermissive

#ASMFLAGS =  




SRC =  webrtc_echo_canceller.cc
VPATH = src

SRC += audio_processing_impl.cc
SRC += echo_cancellation_impl.cc
SRC += echo_cancellation.cc
SRC += echo_cancellation.cc
SRC += aec_core.cc
SRC += aec_rdft.cc
SRC += block_mean_calculator.cc
SRC += delay_estimator_wrapper.cc
SRC += ring_buffer.c
SRC += randomization_functions.c
SRC += metrics_default.cc
SRC += delay_estimator.cc
SRC += aec_resampler.cc
SRC += audio_buffer.cc
SRC += audio_util.cc
SRC += channel_buffer.cc
SRC += push_sinc_resampler.cc
SRC += push_resampler.cc
SRC += sinc_resampler.cc
SRC += splitting_filter.cc
SRC += splitting_filter.c
SRC += aligned_malloc.cc
SRC += three_band_filter_bank.cc
SRC += sparse_fir_filter.cc
VPATH += | $(WEBRTC_PATH)/webrtc/modules/audio_processing
VPATH += | $(WEBRTC_PATH)/webrtc/base
VPATH += | $(WEBRTC_PATH)/webrtc/modules/audio_processing/aec
VPATH += | $(WEBRTC_PATH)/webrtc/modules/audio_processing/utility
VPATH += | $(WEBRTC_PATH)/webrtc/common_audio
VPATH += | $(WEBRTC_PATH)/webrtc/common_audio/signal_processing
VPATH += | $(WEBRTC_PATH)/webrtc/common_audio/resampler
VPATH += | $(WEBRTC_PATH)/webrtc/system_wrappers/source

ifdef CONFIG_WEBRTC_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES +=  webrtc_echo_canceller.cc

    SPEED_CRITICAL_FILES += audio_processing_impl.cc
    SPEED_CRITICAL_FILES += echo_cancellation_impl.cc
    SPEED_CRITICAL_FILES += echo_cancellation.cc
    SPEED_CRITICAL_FILES += echo_cancellation.cc
    SPEED_CRITICAL_FILES += aec_core.cc
    SPEED_CRITICAL_FILES += aec_rdft.cc
    SPEED_CRITICAL_FILES += block_mean_calculator.cc
    SPEED_CRITICAL_FILES += delay_estimator_wrapper.cc
    SPEED_CRITICAL_FILES += ring_buffer.c
    SPEED_CRITICAL_FILES += randomization_functions.c
    SPEED_CRITICAL_FILES += metrics_default.cc
    SPEED_CRITICAL_FILES += delay_estimator.cc
    SPEED_CRITICAL_FILES += aec_resampler.cc
    SPEED_CRITICAL_FILES += audio_buffer.cc
    SPEED_CRITICAL_FILES += audio_util.cc
    SPEED_CRITICAL_FILES += channel_buffer.cc
    SPEED_CRITICAL_FILES += push_sinc_resampler.cc
    SPEED_CRITICAL_FILES += push_resampler.cc
    SPEED_CRITICAL_FILES += sinc_resampler.cc
    SPEED_CRITICAL_FILES += splitting_filter.cc
    SPEED_CRITICAL_FILES += splitting_filter.c
    SPEED_CRITICAL_FILES += aligned_malloc.cc
    SPEED_CRITICAL_FILES += three_band_filter_bank.cc
    SPEED_CRITICAL_FILES += sparse_fir_filter.ccendif
endif


include $(COMMON_CC)
