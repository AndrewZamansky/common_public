
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_JUCE)

ifdef CONFIG_INCLUDE_JUCE

    JUCE_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/JUCE
    ifeq ("$(wildcard $(JUCE_PATH))","")
        $(info   )
        $(info --- JUCE path $(JUCE_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/julianstorer/JUCE  )
        $(info --- make sure that .git directory is located in $(JUCE_PATH)/  after unpacking   )
        $(error )
    endif

    ifeq ($(MAKECMDGOALS),all_after_makefile_generated)
        ifndef JUCE_GIT_TEST_ALREADY_PERFORMED
            JUCE_GIT_TEST_ALREADY_PERFORMED:=1
            #test if current commit and branch of uboot git is the same as required by application
            CURR_GIT_REPOSITORY_DIR :=$(JUCE_PATH)
            CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_JUCE_GIT_COMMIT_HASH
            include $(MAKEFILES_ROOT_DIR)/_common_include_functions/git_prebuild_repo_check.mk
        endif
    endif
    
    CURR_JUCE_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %juce/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(JUCE_PATH))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_JUCE_COMPONENT_LOCATION)/JuceLibraryCode)
endif
#DEFINES =


#ASMFLAGS =

#INCLUDE_DIR =

ifdef    CONFIG_JUCE_VST_PLUGIN
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
ifdef    CONFIG_JUCE_STANDALONE_APPLICATION
    SRC += juce_audio_utils.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_utils
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DBG_NAME :=.dbg
else
    DBG_NAME :=
endif

ifdef CONFIG_MSVC_COMPILER_32
    ARCH_NAME :=x86_32
else ifdef CONFIG_MSVC_COMPILER_64
    ARCH_NAME :=x86_64
endif

ifdef    CONFIG_JUCE_VST_PLUGIN
    SRC += juce_PluginUtilities.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/utility
    SRC += juce_VST_Wrapper.cpp
    VPATH += | $(JUCE_PATH)/modules/juce_audio_plugin_client/VST
    
    ifdef CONFIG_USE_SAVI_HOST_VST_PLUGIN_TESTER
       ifeq ($(wildcard $(REDEFINE_SAVIHOST_VST_TESTER_DIR)),)
           $(info !--- $(WORKSPACE_ROOT_DIR)/workspace_config.mk/REDEFINE_SAVIHOST_VST_TESTER_DIR = $(REDEFINE_SAVIHOST_VST_TESTER_DIR))
           $(info !--- $(REDEFINE_SAVIHOST_VST_TESTER_DIR) not found)
           $(info !--- redefine $(WORKSPACE_ROOT_DIR)/workspace_config.mk/REDEFINE_SAVIHOST_VST_TESTER_DIR to valid saviHost VST tester directory)
           $(error ---) 
       endif
    endif
    
    VST_TESTER :=$(OUT_DIR)\$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    SAVIHOST_DIR :=$(REDEFINE_SAVIHOST_VST_TESTER_DIR)
    #if VST tester dont exists then copy it tou output directory
    ifeq ($(wildcard $(VST_TESTER)),)
        ifdef CONFIG_MSVC_COMPILER_32
            SAVIHOST :=$(SAVIHOST_DIR)\x32\savihost.exe
        else ifdef CONFIG_MSVC_COMPILER_64
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
