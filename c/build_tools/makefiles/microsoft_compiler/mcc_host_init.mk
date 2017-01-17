#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


MCC_ROOT_DIR :=

####### test for existence of microsoft compiler and put its directory name in MCC_ROOT_DIR #####
SEARCHED_TOOL:=visual studio
SEARCHED_DIR_VARIABLE:=MCC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_VISUAL_STUDIO_DIR
TEST_FILE_IN_SEARCHED_DIR:=bin\cl.exe
include $(MAKEFILE_DEFS_ROOT_DIR)/tool_existence_check.mk
####### end of make existence test #####

MCC_ROOT_DIR_wESCAPED_CHAR :=$(TEST_MCC_ROOT_DIR)


ifdef CONFIG_USE_WINDOWS_KITS

    WINDOWS_KIT_VERSION :=$(patsubst "%",%,$(CONFIG_WINDOWS_KIT_VERSION))
    TEST_WINDOWS_KIT_ROOT_DIR 	:= C:\Program\ Files\ (x86)\WINDOWS\ KITS\$(WINDOWS_KIT_VERSION)\LIB\WINV6.3\UM\X86
    ifeq ("$(wildcard $(TEST_WINDOWS_KIT_ROOT_DIR))","")
        $(info !--- $(TEST_WINDOWS_KIT_ROOT_DIR) does not exists )
        WINDOWS_KIT_NOT_FOUND :=1
    endif

    ifdef WINDOWS_KIT_NOT_FOUND
        $(info !--- window kit $(WINDOWS_KIT_VERSION) not found )
        $(info !--- download windows kit $(WINDOWS_KIT_VERSION) and install to default location)
        $(error )
    endif

    WINDOWS_KIT_ROOT_DIR := C:\Program Files (x86)\WINDOWS KITS\$(WINDOWS_KIT_VERSION)

endif

#clear PATH environment variable to get rid of different microsoft compiler conflicts
MCC_BIN_DIR	:=$(MCC_ROOT_DIR)\bin
MCC_BIN_DIR := $(subst /,\,$(MCC_BIN_DIR))
COMPILER_INCLUDE_DIR 	:= $(MCC_ROOT_DIR)/include
MCC_LIB_ROOT_DIR  		:= $(MCC_ROOT_DIR)/lib


### GLOBAL_CFLAGS calculation

GLOBAL_CFLAGS += /Zi
GLOBAL_CFLAGS += /MP /GS /analyze- /W4 /Zc:wchar_t /Gm- /Fd"$(OUT_DIR)\\" /fp:precise
GLOBAL_CFLAGS += /errorReport:prompt /WX- /Zc:forScope /GR /Gd /Oy-
GLOBAL_CFLAGS += /EHsc#/EHsc
GLOBAL_CFLAGS += /nologo /Fp"$(OUT_DIR)\out.pch" /FS
#GLOBAL_CFLAGS += /Fa"Debug\\"#folder should be on local disk

ifdef CONFIG_MCC_CRT_LIBRARIES_LINKED_DINAMICALLY
    CRT_LIBRARIES_OPTION := /MD
else
    CRT_LIBRARIES_OPTION := /MT
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , DEBUG)
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , _DEBUG)
    #following can cause executables not to open .
    #for example some PC cubase is not openning VST (dll) pluging
    #when compile with debug runtime libraries (maybe debug runtime libraries are missing , TO CHECK !!)
    CRT_LIBRARIES_OPTION :=$(CRT_LIBRARIES_OPTION)d
else
    DUMMY := $$(call ADD_TO_GLOBAL_DEFINES , NDEBUG)
    GLOBAL_CFLAGS += /GL
endif
GLOBAL_CFLAGS += $(CRT_LIBRARIES_OPTION)

ifdef CONFIG_MCC_OPTIMISE_NONE
    GLOBAL_CFLAGS += /Od
else ifdef CONFIG_MCC_OPTIMISE_SIZE
    GLOBAL_CFLAGS += /O1
else ifdef CONFIG_MCC_OPTIMISE_SPEED
    GLOBAL_CFLAGS += /O2
else ifdef CONFIG_MCC_OPTIMISE_FULL
    GLOBAL_CFLAGS += /Ox
endif


GLOBAL_CFLAGS += /wd4100 #disable unused parameter warning


# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)


ifdef CONFIG_MCC_OUTPUT_TYPE_DLL
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDLL )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , WIN32 )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _WINDOWS )
endif

#end of flags definitions


ifdef CONFIG_MCC_COMPILER_32
    CC   := set "PATH=$(MCC_BIN_DIR)\bin" & "$(MCC_BIN_DIR)\cl.exe" /c
    ASM  := set "PATH=$(MCC_BIN_DIR)\bin" & "$(MCC_BIN_DIR)\cl.exe" /c
else ifdef CONFIG_MCC_COMPILER_64
    CC   := set "PATH=$(MCC_BIN_DIR)" & "$(MCC_BIN_DIR)\x86_amd64\cl.exe" /c
    ASM  := set "PATH=$(MCC_BIN_DIR)" & "$(MCC_BIN_DIR)\x86_amd64\cl.exe" /c
endif




endif
### end of section that run just one time per build
#####################################################
