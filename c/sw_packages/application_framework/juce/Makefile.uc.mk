
ifeq ($(sort $(CONFIG_INCLUDE_JUCE)),y)

    # !!IMPORTANT!! : should be used before any 'include' statement in makefile:
    CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

    JUCE_GIT :=https://github.com/julianstorer/JUCE
    JUCE_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/JUCE
    ifeq ("$(wildcard $(JUCE_PATH))","")
        $(info   )
        $(info !--- JUCE path $(JUCE_PATH) does not exist)
        $(info !--- get repo from andew zamansky or from $(JUCE_GIT))
        $(info !--- make sure that .git directory is)
        $(info !--- located in $(JUCE_PATH)/  after unpacking)
        $(error )
    endif

    # test if current commit and branch of JUCE git is
    # the same as required by application
    CURR_GIT_REPO_DIR :=$(JUCE_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_JUCE_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(JUCE_PATH)/modules)
    DUMMY := $(call \
         ADD_TO_GLOBAL_INCLUDE_PATH, $(CURR_COMPONENT_DIR)/JuceLibraryCode)
    INCLUDE_THIS_COMPONENT := y

endif


DUMMY := $(call ADD_TO_GLOBAL_DEFINES,JUCE_APP_CONFIG_HEADER="AppConfig.h")

#DEFINES +=


#ASMFLAGS =

#INCLUDE_DIR =

ifeq ($(sort $(CONFIG_JUCE_VST_PLUGIN)),y)
    DUMMY := $(call \
           ADD_TO_GLOBAL_INCLUDE_PATH, $(EXTERNAL_SOURCE_ROOT_DIR)/VST3_SDK)
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

ifeq ($(sort $(CONFIG_MSC_TARGET_ARCH_X86)),y)
    ARCH_NAME :=x86_32
else ifeq ($(sort $(CONFIG_MSC_TARGET_ARCH_X64)),y)
    ARCH_NAME :=x86_64
endif

ifeq ($(sort $(CONFIG_JUCE_VST_PLUGIN)),y)
    SRC += juce_PluginUtilities.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/utility
    SRC += juce_VST_Wrapper.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/VST
    
    ifeq ($(sort $(CONFIG_USE_SAVI_HOST_VST_PLUGIN_TESTER)),y)
       ifeq ($(wildcard $(REDEFINE_SAVIHOST_VST_TESTER_DIR)),)
           WORKSPACE_CFG_MK :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
           SAVIHOST_DIR_VAR :=\
                  $(WORKSPACE_CFG_MK)/REDEFINE_SAVIHOST_VST_TESTER_DIR
           $(info\
               err: $(SAVIHOST_DIR_VAR) = $(REDEFINE_SAVIHOST_VST_TESTER_DIR))
           $(info ---: $(REDEFINE_SAVIHOST_VST_TESTER_DIR) not found)
           $(info ---: redefine $(SAVIHOST_DIR_VAR) to)
           $(info ---: valid saviHost VST tester directory)
           $(call exit,1)
       endif
    endif
    
    VST_TESTER :=$(OUT_DIR)\$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    SAVIHOST_DIR :=$(REDEFINE_SAVIHOST_VST_TESTER_DIR)
    #if VST tester dont exists then copy it tou output directory
    ifeq ($(wildcard $(VST_TESTER)),)
        ifeq ($(sort $(CONFIG_MSC_TARGET_ARCH_X86)),y)
            SAVIHOST :=$(SAVIHOST_DIR)\x32\savihost.exe
        else ifeq ($(sort $(CONFIG_MSC_TARGET_ARCH_X64)),y)
            SAVIHOST :=$(SAVIHOST_DIR)\x64\savihost.exe
        endif
        SAVIHOST := $(call fix_path_if_in_windows,$(SAVIHOST))
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
