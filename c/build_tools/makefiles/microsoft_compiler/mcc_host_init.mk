#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


MCC_ROOT_DIR :=



ifdef REDEFINE_MICROSOFT_COMPILER_ROOT_DIR

    $(info  microsoft compiler dir  redefined to $(REDEFINE_MICROSOFT_COMPILER_ROOT_DIR) )
    ifeq ("$(wildcard $(REDEFINE_MICROSOFT_COMPILER_ROOT_DIR))","")
        $(info !--- microsoft compiler path $(REDEFINE_MICROSOFT_COMPILER_ROOT_DIR) does not exists)
        $(info !--- to use default microsoft compiler location remove/comment REDEFINE_MICROSOFT_COMPILER_ROOT_DIR variable in  $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
        $(info !--- you can set customized microsoft compiler path in REDEFINE_MICROSOFT_COMPILER_ROOT_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
        $(error )
    else
        MCC_ROOT_DIR 	:= 	$(REDEFINE_MICROSOFT_COMPILER_ROOT_DIR)
    endif

else #for 'ifdef REDEFINE_MICROSOFT_COMPILER_ROOT_DIR'

    ifdef  CONFIG_MCC_COMPILER_LOCATION_WINDOWS_DEFAULT

        ifdef CONFIG_MCC_VISUAL_STUDIO_VERSION_2012
            MCC_VS_VERSION=12.0
            MCC_VS_VERSION_NAME=2012
        else ifdef CONFIG_MCC_VISUAL_STUDIO_VERSION_2013
            MCC_VS_VERSION=13.0
            MCC_VS_VERSION_NAME=2013
        else
            $(info !--- microsoft visual studio version isn not defined)
            $(error )
        endif
        ifndef MCC_NOT_FOUND
           TEST_MCC_ROOT_DIR 	:= C:\Program\ Files\ (x86)\Microsoft\ Visual\ Studio\ $(MCC_VS_VERSION)\VC
           ifeq ("$(wildcard $(TEST_MCC_ROOT_DIR))","")
               $(info !--- $(TEST_MCC_ROOT_DIR) does not exists )
               #$(info !--- (if needed you can change microsoft compiler version using menuconfig in "Building System" menu ))
               MCC_NOT_FOUND :=1
           endif
        endif


        ifdef MCC_NOT_FOUND
            $(info !--- microsoft compiler does not exists )
            $(info !--- download visual studio  $(MCC_VS_VERSION_NAME) and install to default location)
            $(error )
        endif

        MCC_ARCH := $(subst \,,$(MCC_ARCH))
        MCC_ROOT_DIR :=C:\Program Files (x86)\Microsoft Visual Studio $(MCC_VS_VERSION)\VC
        MCC_ROOT_DIR_wESCAPED_CHAR :=$(TEST_MCC_ROOT_DIR)

    else

        TEST_MCC_ROOT_DIR 	:= $(TOOLS_ROOT_DIR)/mcc
        ifeq ("$(wildcard $(TEST_MCC_ROOT_DIR)*)","")
            $(info !--- $(TEST_MCC_ROOT_DIR) does not exists )
            MCC_NOT_FOUND :=1
        endif
        
        ifdef CONFIG_MCC_COMPILER_32
            MCC_ARCH=x86_32_
        else ifdef CONFIG_MCC_COMPILER_64
            MCC_ARCH=x86_64_
        else
            $(info !--- microsoft compiler architecture not defined)
            $(error )
        endif


        TEST_MCC_ROOT_DIR 	:= $(TOOLS_ROOT_DIR)/mcc_$(MCC_ARCH)
        ifeq ("$(wildcard $(TEST_MCC_ROOT_DIR)*)","")
            $(info !--- $(TEST_MCC_ROOT_DIR) does not exists )
            $(info !--- microsoft compiler with specified architecture does not exists )
            MCC_NOT_FOUND :=1
        endif

        ifdef CONFIG_MCC_VISUAL_STUDIO_VERSION_2012
            MCC_VS_VERSION_NAME=2012
        else ifdef CONFIG_MCC_VISUAL_STUDIO_VERSION_2013
            MCC_VS_VERSION_NAME=2013
        else
            $(info !--- microsoft visual studio version isn not defined)
            $(error )
        endif
        ifndef MCC_NOT_FOUND
           TEST_MCC_ROOT_DIR 	:= $(TOOLS_ROOT_DIR)/mcc_$(MCC_ARCH)_$(MCC_VS_VERSION_NAME)
           ifeq ("$(wildcard $(TEST_MCC_ROOT_DIR))","")
               $(info !--- $(TEST_MCC_ROOT_DIR) does not exists )
               $(info !--- microsoft compiler with specified version does not exists )
               MCC_NOT_FOUND :=1
           endif
        endif


        ifdef MCC_NOT_FOUND
            TEST_MCC_ROOT_DIR 	:=  $(TOOLS_ROOT_DIR)/mcc_$(MCC_ARCH)_$(MCC_VS_VERSION_NAME)
            $(info !--- mcc path $(TEST_MCC_ROOT_DIR) dont exists )
            $(info !--- download microsoft compiler  $(MCC_VS_VERSION_NAME) and install to $(TEST_MCC_ROOT_DIR))
            $(info !--- make sure bin and lib  folders is located in $(TEST_MCC_ROOT_DIR)/  after unpacking   )
            $(info !--- or use set CONFIG_MCC_COMPILER_LOCATION_WINDOWS_DEFAULT in menuconfig   )
            $(info !--- you can also set customized MCC path in REDEFINE_MICROSOFT_COMPILER_ROOT_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
            $(error )
        endif

        MCC_ROOT_DIR :=  $(TOOLS_ROOT_DIR)/mcc_$(MCC_ARCH)_$(MCC_VS_VERSION_NAME)
        MCC_ROOT_DIR_wESCAPED_CHAR :=$(TEST_MCC_ROOT_DIR)

    endif
endif

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
MCC_BIN_DIR_wESCAPED_CHAR :=$(MCC_ROOT_DIR_wESCAPED_CHAR)
MCC_BIN_DIR := $(subst /,\,$(MCC_BIN_DIR))
MCC_BIN_DIR_wESCAPED_CHAR := $(subst /,\,$(MCC_BIN_DIR_wESCAPED_CHAR))
COMPILER_INCLUDE_DIR 	:= $(MCC_ROOT_DIR)/include
MCC_LIB_ROOT_DIR  		:= $(MCC_ROOT_DIR)/lib


### GLOBAL_CFLAGS calculation

ifdef CONFIG_MCC_COMPILER_32
    GLOBAL_CFLAGS += /ZI
else ifdef CONFIG_MCC_COMPILER_64
    GLOBAL_CFLAGS += /Zi
endif

GLOBAL_CFLAGS += /MP /GS /analyze- /W4 /Zc:wchar_t /Gm- /Od /Fd"$(OUT_DIR)\\" /fp:precise 
GLOBAL_CFLAGS += /errorReport:prompt /WX- /Zc:forScope /GR /Gd /Oy-
GLOBAL_CFLAGS += /EHsc#/EHsc
GLOBAL_CFLAGS += /nologo /Fp"$(OUT_DIR)\out.pch" /FS
#GLOBAL_CFLAGS += /Fa"Debug\\"

ifdef CONFIG_MCC_CRT_LIBRARIES_LINKED_DINAMICALLY
    CRT_LIBRARIES_OPTION := /MD
else
    CRT_LIBRARIES_OPTION := /MT
endif

ifdef CONFIG_MCC_OPTIMISE_NONE
    #following line removed because on some PC cubase is not openning VST (dll) pluging
    #when compile with debug runtime libraries (maybe debug runtime libraries are missing , TO CHECK !!)
    #CRT_LIBRARIES_OPTION :=$(CRT_LIBRARIES_OPTION)d
endif
GLOBAL_CFLAGS += $(CRT_LIBRARIES_OPTION)

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
