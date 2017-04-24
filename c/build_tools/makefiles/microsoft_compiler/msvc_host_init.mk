#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


MSVC_ROOT_DIR :=

#{{{{{{{{   test for existence of microsoft compiler   {{{{{{{{
   # MSVC_ROOT_DIR will contain directry of microsoft compiler
SEARCHED_TOOL:=cl
SEARCHED_DIR_VARIABLE:=MSVC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_VISUAL_STUDIO_DIR
TEST_FILE_IN_SEARCHED_DIR:=bin\cl.exe
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
#}}}}}}}}  #}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}

MSVC_BIN_DIR	:=$(MSVC_ROOT_DIR)\bin
MSVC_BIN_DIR := $(subst /,\,$(MSVC_BIN_DIR))

SHELL_OUT :=$(shell "$(MSVC_BIN_DIR)\cl.exe" 2>&1)
ifneq ($(findstring Version 18,$(SHELL_OUT)),)
    MSVC_SET_ADDITIONAL_PATHS :=set "PATH=$(MSVC_BIN_DIR)\bin" &
    VS_VERSION :=2013
    MSVC_VERSION :=18
else ifneq ($(findstring Version 19,$(SHELL_OUT)),)
    MSVC_SET_ADDITIONAL_PATHS :=set "PATH=$(MSVC_BIN_DIR)\bin" &
    VS_VERSION :=2015
    MSVC_VERSION :=19
else
    SHEL_CMD :=set "PATH=$(MSVC_ROOT_DIR)\..\Common7\IDE" &
    SHEL_CMD +=$(MSVC_BIN_DIR)\cl.exe" 2>&1
    SHELL_OUT :=$(shell $(SHEL_CMD))
    ifneq ($(findstring Version 17,$(SHELL_OUT)),)
        MSVC_SET_ADDITIONAL_PATHS :=set "PATH=$(MSVC_ROOT_DIR)\..\Common7\IDE" &
        VS_VERSION :=2012
        MSVC_VERSION :=17
    else
        $(info !---- unsupported version of visual studio)
        $(info !---- install one of following versions : VS2012, VS2013, VS2015)
        $(error )
    endif
endif
$(info ---- Visual studio version :  $(VS_VERSION))

GLOBAL_CFLAGS += /I"$(MSVC_ROOT_DIR)/include"

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

    TEST_WINDOWS_KIT_DIR :=$(subst $(SPACE),\$(SPACE),$(WDK_DIR))
    ifeq ("$(wildcard $(TEST_WINDOWS_KIT_DIR))","")
        $(info !--- $(TEST_WINDOWS_KIT_DIR) does not exists )
        $(info !--- window kit $(WINDOWS_KIT_VERSION) not found )
        $(info !--- download/install windows kit $(WINDOWS_KIT_VERSION) to default location)
        $(error )
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

    ifeq ($(VS_VERSION),2015)
        WDK_10_VERSION:=
        WDK_10_DIR :=$(WDK_DIR)\Include\10.0.10586.0
        TEST_DIR :=$(subst $(SPACE),\$(SPACE),$(WDK_10_DIR))
        ifneq ("$(wildcard $(TEST_WDK_10_DIR))","")
            WDK_10_VERSION :=10.0.10586.0
            WDK_10_DIR :=
        endif
        ifeq ($(WDK_10_VERSION),)
            WDK_10_DIR :=$(WDK_DIR)\Include\10.0.10240.0
            TEST_WDK_10_DIR :=$(subst $(SPACE),\$(SPACE),$(WDK_10_DIR))
            ifneq ("$(wildcard $(TEST_WDK_10_DIR))","")
                WDK_10_VERSION :=10.0.10240.0
            endif
        endif
        ifeq ($(WDK_10_VERSION),)
            WDK_10_DIR :=$(WDK_DIR)\Include\10.0.10150.0
            TEST_WDK_10_DIR :=$(subst $(SPACE),\$(SPACE),$(WDK_10_DIR))
            ifneq ("$(wildcard $(TEST_WDK_10_DIR))","")
                WDK_10_VERSION :=10.0.10150.0
            endif
        endif
        ifeq ($(WDK_10_VERSION),)
             $(info !--- no supported WDK 10 version found.)
             $(info !--- supported versions are 10.0.10586.0 , 10.0.10240.0 , 10.0.10150.0)
             $(error )
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


ifdef CONFIG_MSVC_OUTPUT_TYPE_DLL
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDLL )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , WIN32 )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDOWS )
endif

#end of flags definitions


ifdef CONFIG_MSVC_COMPILER_32
    CC   := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\cl.exe" /c
    ASM  := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\cl.exe" /c
else ifdef CONFIG_MSVC_COMPILER_64
    CC   := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\x86_amd64\cl.exe" /c
    ASM  := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\x86_amd64\cl.exe" /c
endif




endif
### end of section that run just one time per build
#####################################################
