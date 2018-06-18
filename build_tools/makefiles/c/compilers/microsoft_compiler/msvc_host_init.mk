MSVC_BIN_DIR :=

#######   test for existence of microsoft compiler   ###########
   # MSVC_BIN_DIR will contain directry of microsoft compiler
SEARCHED_TOOL:=cl.exe
SEARCHED_DIR_VARIABLE:=MSVC_BIN_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_VISUAL_STUDIO_DIR
TEST_FILE_IN_SEARCHED_DIR:=cl.exe
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
############ end of tool existence test #####

MSVC_BIN_DIR := $(subst /,\,$(MSVC_BIN_DIR))

SHELL_OUT :=$(shell "$(MSVC_BIN_DIR)\cl.exe" 2>&1)
ifneq ($(findstring Version 18,$(SHELL_OUT)),)
    ADDITIONAL_PATH :=$(MSVC_BIN_DIR)
    VS_VERSION :=2013
    MSVC_VERSION :=18
else ifneq ($(findstring Version 19.14,$(SHELL_OUT)),)
    ADDITIONAL_PATH :=$(MSVC_BIN_DIR)
    ifdef CONFIG_MSC_TARGET_ARCH_X64
        ADDITIONAL_PATH +=;$(MSVC_BIN_DIR)\..\x64
    endif
    VS_VERSION :=2017
    MSVC_VERSION :=19.14
else ifneq ($(findstring Version 19.00,$(SHELL_OUT)),)
    ADDITIONAL_PATH :=$(MSVC_BIN_DIR)
    VS_VERSION :=2015
    MSVC_VERSION :=19.00
else
    ADDITIONAL_PATH :=$(MSVC_BIN_DIR)\..\..\Common7\IDE
    SHEL_CMD :=set "PATH=$(ADDITIONAL_PATH)" &
    SHEL_CMD +=$(MSVC_BIN_DIR)\cl.exe" 2>&1
    SHELL_OUT :=$(shell $(SHEL_CMD))
    ifneq ($(findstring Version 17,$(SHELL_OUT)),)
        MSVC_SET_ADDITIONAL_PATHS :=set "PATH=$(ADDITIONAL_PATH)" &
        VS_VERSION :=2012
        MSVC_VERSION :=17
    else
        $(info err: unsupported version of visual studio)
        $(info err: install one of following versions : VS2012, VS2013, VS2015, VS2017(community))
        $(call exit,1)
    endif
endif
MSVC_SET_ADDITIONAL_PATHS :=set "PATH=$(ADDITIONAL_PATH)" &

$(info ---- Visual studio version :  $(VS_VERSION))

ifeq ($(VS_VERSION),2017)
    GLOBAL_CFLAGS += /I"$(MSVC_BIN_DIR)/../../../include"
else
    GLOBAL_CFLAGS += /I"$(MSVC_BIN_DIR)/../include"
endif


ifdef CONFIG_USE_WINDOWS_KITS

    ifeq ($(VS_VERSION),2012)
        WDK_DIR :=C:\Program Files (x86)\WINDOWS KITS\8.0
    endif

    ifeq ($(VS_VERSION),2013)
        WDK_DIR :=C:\Program Files (x86)\WINDOWS KITS\8.1
    endif

    ifeq ($(VS_VERSION),2015)
        WDK_DIR :=C:\Program Files (x86)\WINDOWS KITS\10
    endif
    ifeq ($(VS_VERSION),2017)
        WDK_DIR :=C:\Program Files (x86)\WINDOWS KITS\10
    endif

    TEST_WINDOWS_KIT_DIR :=$(subst $(SPACE),\$(SPACE),$(WDK_DIR))
    ifeq ("$(wildcard $(TEST_WINDOWS_KIT_DIR))","")
        $(info err: $(TEST_WINDOWS_KIT_DIR) does not exists )
        $(info ---: window kit $(WINDOWS_KIT_VERSION) not found )
        $(info ---: download/install windows kit $(WINDOWS_KIT_VERSION) to default location)
        $(call exit,1)
    endif

    $(info ---- WDK :  $(WDK_DIR))

    ifeq ($(VS_VERSION),2012)
        # cannot use $(call ADD_TO_GLOBAL_INCLUDE_PATH) because
        # of spaces in folder name
        GLOBAL_CFLAGS += /I"$(WDK_DIR)\INCLUDE\UM"
        GLOBAL_CFLAGS += /I"$(WDK_DIR)\INCLUDE\SHARED"
    endif

    ifeq ($(VS_VERSION),2013)
        # cannot use $(call ADD_TO_GLOBAL_INCLUDE_PATH) because
        # of spaces in folder name
        GLOBAL_CFLAGS += /I"$(WDK_DIR)\INCLUDE\UM"
        GLOBAL_CFLAGS += /I"$(WDK_DIR)\INCLUDE\SHARED"
    endif

    ifneq ($(filter 2015 2017,$(VS_VERSION)),)# if 2015 or 2017

        # must be sorted from newer to older order
        SUPPORTED_WDK := 10.0.17134.0
        SUPPORTED_WDK += 10.0.10586.0
        SUPPORTED_WDK += 10.0.10240.0
        SUPPORTED_WDK += 10.0.10150.0

        WDK_DIR_FOR_SHELL :=$(subst $(SPACE),\$(SPACE),$(WDK_DIR))\Include
        _WDK_FOLDERS :=$(patsubst %,$(WDK_DIR_FOR_SHELL)\\%,$(SUPPORTED_WDK))
        FOUND_WDK_DIR_LIST :=$(wildcard $(_WDK_FOLDERS))
        FOUND_WDK_LIST :=$(subst $(WDK_DIR)\Include\,,$(FOUND_WDK_DIR_LIST))
        WDK_10_VERSION := $(firstword $(FOUND_WDK_LIST))
        WDK_10_DIR :=$(WDK_DIR)\Include\$(WDK_10_VERSION)

        ifeq ($(WDK_10_VERSION),)
             $(info err: no supported WDK 10 version found.)
             $(info ---: supported versions are $(SUPPORTED_WDK))
             $(call exit,1)
        endif

        # cannot use $(call ADD_TO_GLOBAL_INCLUDE_PATH) because
        # of spaces in folder name
        GLOBAL_CFLAGS += /I"$(WDK_10_DIR)\ucrt"
        GLOBAL_CFLAGS += /I"$(WDK_10_DIR)\um"
        GLOBAL_CFLAGS += /I"$(WDK_10_DIR)\shared"
        $(info ---- WDK vaersion :  $(WDK_10_VERSION))
    endif

endif



#{{{{{{{{   GLOBAL_CFLAGS PREPARATIONS   {{{{{{{{

GLOBAL_CFLAGS += /Zi
GLOBAL_CFLAGS += /MP /GS /analyze- /W4 /Zc:wchar_t /Gm-
GLOBAL_CFLAGS += /Fd"$(OUT_DIR)\\" /fp:precise
GLOBAL_CFLAGS += /errorReport:prompt /WX- /Zc:forScope /GR /Gd /Oy-
GLOBAL_CFLAGS += /EHsc#/EHsc
GLOBAL_CFLAGS += /nologo /Fp"$(OUT_DIR)\out.pch"
ifneq ($(VS_VERSION),2012)
    GLOBAL_CFLAGS += /FS
endif
#GLOBAL_CFLAGS += /Fa"Debug\\"#folder should be on local disk

ifdef CONFIG_MSVC_CRT_LIBRARIES_LINKED_DINAMICALLY
    CRT_LIBRARIES_OPTION := /MD
else
    CRT_LIBRARIES_OPTION := /MT
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , DEBUG)
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , _DEBUG)
    # following can cause executables not to open .
    # for example some PC cubase is not openning VST (dll) pluging
    # when compile with debug runtime libraries (maybe debug runtime 
    # libraries are missing , TO CHECK !!)
    CRT_LIBRARIES_OPTION :=$(CRT_LIBRARIES_OPTION)d
else
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , NDEBUG)
    GLOBAL_CFLAGS += /GL
endif
GLOBAL_CFLAGS += $(CRT_LIBRARIES_OPTION)

ifdef CONFIG_MSVC_OPTIMISE_NONE
    GLOBAL_CFLAGS += /Od
else ifdef CONFIG_MSVC_OPTIMISE_SIZE
    GLOBAL_CFLAGS += /O1
else ifdef CONFIG_MSVC_OPTIMISE_SPEED
    GLOBAL_CFLAGS += /O2
else ifdef CONFIG_MSVC_OPTIMISE_FULL
    GLOBAL_CFLAGS += /Ox
endif


GLOBAL_CFLAGS += /wd4100 #disable unused parameter warning


#}}}}}}}}  END OF GLOBAL_CFLAGS PREPARATIONS }}}}}}}}


# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)


ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDLL )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , WIN32 )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDOWS )
endif

#end of flags definitions


ifdef CONFIG_MSC_TARGET_ARCH_X86
    ifeq ($(VS_VERSION),2017)
        CL_BIN_DIR := $(MSVC_BIN_DIR)\..\x86
    else
        CL_BIN_DIR := $(MSVC_BIN_DIR)
    endif
else ifdef CONFIG_MSC_TARGET_ARCH_X64
    ifeq ($(VS_VERSION),2017)
        CL_BIN_DIR := $(MSVC_BIN_DIR)\..\x64
    else
        CL_BIN_DIR := $(MSVC_BIN_DIR)\x86_amd64
    endif
endif

CC   := $(MSVC_SET_ADDITIONAL_PATHS) "$(CL_BIN_DIR)\cl.exe" /c
ASM  := $(MSVC_SET_ADDITIONAL_PATHS) "$(CL_BIN_DIR)\cl.exe" /c
