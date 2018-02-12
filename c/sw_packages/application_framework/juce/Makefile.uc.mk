

ifeq ($(sort $(CONFIG_XTENSA_FPGA_SIM)),y)

    JUCE_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/JUCE
    ifeq ("$(wildcard $(JUCE_PATH))","")
        $(info   )
        $(info --- JUCE path $(JUCE_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/julianstorer/JUCE  )
        $(info --- make sure that .git directory is located in $(JUCE_PATH)/  after unpacking   )
        $(error )
    endif

    #test if current commit and branch of uboot git is the same as required by application
    CURR_GIT_REPO_DIR :=$(JUCE_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_JUCE_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    CURR_JUCE_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %juce/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(JUCE_PATH))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_JUCE_COMPONENT_LOCATION)/JuceLibraryCode)
    INCLUDE_THIS_COMPONENT := y

endif


#DEFINES =


#ASMFLAGS =

#INCLUDE_DIR =

ifeq ($(sort $(CONFIG_JUCE_VST_PLUGIN)),y)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/VST3_SDK)
endif

SRC = juce_audio_basics.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_basics
SRC += juce_audio_devices.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_devices
SRC += juce_audio_formats.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_formats
SRC += juce_audio_processors.cpp
VPATH += | $(JUCE_PATH)/modules/juce_audio_processors
ifeq ($(sort $(CONFIG_JUCE_STANDALONE_APPLICATION)),y)
    SRC += juce_audio_utils.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_utils
endif

ifeq ($(sort $(CONFIG_COMPILE_FOR_DEBUG)),y)
    DBG_NAME :=.dbg
else
    DBG_NAME :=
endif

ifeq ($(sort $(CONFIG_MSVC_COMPILER_32)),y)
    ARCH_NAME :=x86_32
else ifeq ($(sort $(CONFIG_MSVC_COMPILER_64)),y)
    ARCH_NAME :=x86_64
endif

ifeq ($(sort $(CONFIG_JUCE_VST_PLUGIN)),y)
    SRC += juce_PluginUtilities.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/utility
    SRC += juce_VST_Wrapper.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/VST
    
    ifeq ($(sort $(CONFIG_USE_SAVI_HOST_VST_PLUGIN_TESTER)),y)
       ifeq ($(wildcard $(REDEFINE_SAVIHOST_VST_TESTER_DIR)),)
           $(info !--- $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/REDEFINE_SAVIHOST_VST_TESTER_DIR = $(REDEFINE_SAVIHOST_VST_TESTER_DIR))
           $(info !--- $(REDEFINE_SAVIHOST_VST_TESTER_DIR) not found)
           $(info !--- redefine $(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk/REDEFINE_SAVIHOST_VST_TESTER_DIR to valid saviHost VST tester directory)
           $(error ---) 
       endif
    endif
    
    VST_TESTER :=$(OUT_DIR)\$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    SAVIHOST_DIR :=$(REDEFINE_SAVIHOST_VST_TESTER_DIR)
    #if VST tester dont exists then copy it tou output directory
    ifeq ($(wildcard $(VST_TESTER)),)
        ifeq ($(sort $(CONFIG_MSVC_COMPILER_32)),y)
            SAVIHOST :=$(SAVIHOST_DIR)\x32\savihost.exe
        else ifeq ($(sort $(CONFIG_MSVC_COMPILER_64)),y)
            SAVIHOST :=$(SAVIHOST_DIR)\x64\savihost.exe
        endif
        SAVIHOST := $(subst /,\,$(SAVIHOST))
        DUMMY:=$(shell $(CP) $(SAVIHOST) $(VST_TESTER))
    endif


endif
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
