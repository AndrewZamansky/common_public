
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_JUCE)

ifdef CONFIG_INCLUDE_JUCE

    JUCE_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/juce-grapefruit-windows
    ifeq ("$(wildcard $(JUCE_PATH))","")
        $(info   )
        $(info --- juce-grapefruit-windows path $(JUCE_PATH) dont exists )
        $(info --- get juce-grapefruit-windows  and unpack it to $(JUCE_PATH)  )
        $(info --- make sure that modules directory is located in $(JUCE_PATH)/  after unpacking   )
        $(error )
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(JUCE_PATH))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SW_PACKAGES_ROOT_DIR)/staging/juce/JuceLibraryCode)
endif
#DEFINES =


#ASMFLAGS =

#INCLUDE_DIR = $(JUCE_PATH)/include#  $(JUCE_PATH)/arch/arm/include/asm/


SRC = juce_audio_basics.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_basics
SRC += juce_audio_devices.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_devices
SRC += juce_audio_formats.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_formats
SRC += juce_audio_processors.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_processors
SRC += juce_audio_utils.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_utils
SRC += juce_core.cpp
VPATH += | $(JUCE_PATH)/modules/juce_core
SRC += juce_cryptography.cpp
VPATH += | $(JUCE_PATH)/modules/juce_cryptography
SRC += juce_data_structures.cpp
VPATH += | $(JUCE_PATH)/modules/juce_data_structures
SRC += juce_events.cpp
VPATH += | $(JUCE_PATH)/modules/juce_events
SRC += juce_graphics.cpp
VPATH += | $(JUCE_PATH)/modules/juce_graphics
SRC += juce_gui_basics.cpp
VPATH += | $(JUCE_PATH)/modules/juce_gui_basics
SRC += juce_gui_extra.cpp
VPATH += | $(JUCE_PATH)/modules/juce_gui_extra
SRC += juce_opengl.cpp
VPATH += | $(JUCE_PATH)/modules/juce_opengl
SRC += juce_video.cpp
VPATH += | $(JUCE_PATH)/modules/juce_video

include $(COMMON_CC)
