INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN)

#INCLUDE_DIR

#DEFINES =

#CFLAGS =

#ASMFLAGS =

SRC = band_splitter_butterworth_6db_gain.cpp
SRC += band_splitter_butterworth_6db_gain_biquad_subchains.c

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += band_splitter_butterworth_6db_gain.cpp \
                band_splitter_butterworth_6db_gain_biquad_subchains.c
endif

VPATH = src

include $(COMMON_CC)
