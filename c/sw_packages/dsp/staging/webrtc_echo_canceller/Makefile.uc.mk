 
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WEBRTC_AEC)

ifdef CONFIG_INCLUDE_WEBRTC_AEC
    GLOBAL_LIBS += libstdc++.a
endif

INCLUDE_DIR +=$(EXTERNAL_SOURCE_ROOT_DIR)/webrtc

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
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/modules/audio_processing
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/base
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/modules/audio_processing/aec
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/modules/audio_processing/utility
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/common_audio
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/common_audio/signal_processing
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/common_audio/resampler
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/webrtc/webrtc/system_wrappers/source

ifdef CONFIG_WEBRTC_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES = $(SRC)
endif



include $(COMMON_CC)
