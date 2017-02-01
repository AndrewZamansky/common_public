
ifdef CONFIG_MSVC_COMPILER_32
    LD := set "PATH=$(MSVC_BIN_DIR)" & "$(MSVC_BIN_DIR)\link"
else ifdef CONFIG_MSVC_COMPILER_64
    LD := set "PATH=$(MSVC_BIN_DIR)" & "$(MSVC_BIN_DIR)\x86_amd64\link"
endif

LIBS := $(sort $(GLOBAL_LIBS))

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES :=
   LIBS += $(STD_LIBRARIES)
endif



ifdef CONFIG_MSVC_COMPILER_32
    ARCH_NAME :=x86_32
else
    ARCH_NAME :=x86_64
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DBG_NAME :=.dbg
else
    DBG_NAME :=
endif

ifdef CONFIG_MSVC_OUTPUT_TYPE_WINDOWS_APPLICATION
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).$(MAIN_VERSION_STR).exe
endif

ifdef CONFIG_MSVC_OUTPUT_TYPE_CONSOLE_APPLICATION
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).$(MAIN_VERSION_STR).exe
endif
ifdef CONFIG_MSVC_OUTPUT_TYPE_DLL
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).dll
    LINKER_LIB_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).lib
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).dll
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).$(MAIN_VERSION_STR).dll
endif


#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf

ifdef CONFIG_MSVC_COMPILER_32
    LDFLAGS += /MACHINE:X86
else
    LDFLAGS += /MACHINE:X64
endif

LDFLAGS += /MANIFEST /NXCOMPAT /PDB:"$(OUT_DIR)\out.pdb" /DYNAMICBASE 
LDFLAGS += /LARGEADDRESSAWARE# /PGD:"$(OUT_DIR)\out.pgd"

ifdef CONFIG_MSVC_OUTPUT_TYPE_CONSOLE_APPLICATION
    LDFLAGS += /SUBSYSTEM:CONSOLE
else
    LDFLAGS += /SUBSYSTEM:WINDOWS
endif

LDFLAGS += /MANIFESTUAC:"level='asInvoker' uiAccess='false'"

ifdef CONFIG_MSVC_IGNORE_DEFAULT_LIBRARIES
    ifdef CONFIG_MSVC_IGNORE_ALL_DEFAULT_LIBRARIES
        LDFLAGS += /NODEFAULTLIB
    else
        ifdef CONFIG_MSVC_IGNORE_LIBCMT_LIBRARY
            LDFLAGS += /NODEFAULTLIB:"libcmt.lib"
        endif
        ifdef CONFIG_MSVC_IGNORE_MSVCRT_LIBRARY
            LDFLAGS += /NODEFAULTLIB:"msvcrt.lib"
            LDFLAGS += /NODEFAULTLIB:"msvcrtd.lib"
        endif
    endif
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    LDFLAGS += /SAFESEH:NO
    LDFLAGS += /DEBUG
else
    LDFLAGS += /OPT:REF#LINK removes unreferenced packaged functions and data
    LDFLAGS += /OPT:ICF#perform 2(default) interations to find unreferenced code or data
    LDFLAGS += /LTCG
    ifdef CONFIG_MSVC_COMPILER_32
        LDFLAGS += /SAFESEH
    endif
endif

ifdef CONFIG_MSVC_OUTPUT_TYPE_WINDOWS_APPLICATION
    LDFLAGS += /ManifestFile:"$(OUT_DIR)\$(PROJECT_NAME).exe.intermediate.manifest"
endif
ifdef CONFIG_MSVC_OUTPUT_TYPE_CONSOLE_APPLICATION
    LDFLAGS += /ManifestFile:"$(OUT_DIR)\$(PROJECT_NAME).exe.intermediate.manifest"
endif
ifdef CONFIG_MSVC_OUTPUT_TYPE_DLL
    LDFLAGS += /DLL
    LDFLAGS += /IMPLIB:"$(OUT_DIR)\$(PROJECT_NAME).lib"
    LDFLAGS += /ManifestFile:"$(OUT_DIR)\$(PROJECT_NAME).dll.intermediate.manifest"
endif
LDFLAGS += /ERRORREPORT:PROMPT /NOLOGO /TLBID:1

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    ifdef CONFIG_MSVC_COMPILER_32
        LDFLAGS += /LIBPATH:"$(MSVC_ROOT_DIR)\lib"
    else
        LDFLAGS += /LIBPATH:"$(MSVC_ROOT_DIR)\lib\amd64"
    endif
endif

ifdef CONFIG_USE_WINDOWS_KITS
    WINDOWS_KITS_LIBRARIES := kernel32.lib user32.lib gdi32.lib winspool.lib
    WINDOWS_KITS_LIBRARIES += comdlg32.lib advapi32.lib shell32.lib ole32.lib
    WINDOWS_KITS_LIBRARIES += oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
    LIBS += $(WINDOWS_KITS_LIBRARIES)
    LIBS := $(sort $(LIBS))

    ifeq ($(VS_VERSION),2013)
        ifdef CONFIG_MSVC_COMPILER_32
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\WINV6.3\UM\X86"
        else
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\WINV6.3\UM\X64"
        endif
    endif
    
    ifeq ($(VS_VERSION),2015)
        ifdef CONFIG_MSVC_COMPILER_32
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\$(WDK_10_VERSION)\ucrt\x86"
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\$(WDK_10_VERSION)\um\x86"
        else
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\$(WDK_10_VERSION)\ucrt\x64"
            LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\$(WDK_10_VERSION)\um\x64"
        endif
    endif
    
endif

ifneq ($(strip $(GLOBAL_LIBS_PATH)),)
    GLOBAL_LIBS_PATH :=$(sort $(GLOBAL_LIBS_PATH)) 
    GLOBAL_LIBS_PATH := $(patsubst %,/LIBPATH:"%",$(GLOBAL_LIBS_PATH))
    LDFLAGS += $(GLOBAL_LIBS_PATH)
endif

LDFLAGS += /MAP:"$(OUT_DIR)\$(PROJECT_NAME).map" /MAPINFO:EXPORTS

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)


############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############




##########################################################



#LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))




rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#some time on windows .O.asm and .o.asm will appear as same files . so $(sort) will eliminate duplication


ALL_OBJ_FILES := $(call rwildcard,$(OBJ_DIR)/,*.o) $(call rwildcard,$(OBJ_DIR)/,*.oo)  $(call rwildcard,$(OBJ_DIR)/,*.oop) 
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm) $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
ALL_OBJ_FILES := $(sort $(ALL_OBJ_FILES))
#ALL_OBJ_FILES	:= 	$(patsubst %.oop,%.obj,$(ALL_OBJ_FILES))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    LINKER_OUTPUT_NAMED := $(subst /,\,$(LINKER_OUTPUT_NAMED))
    OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
    OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/add_item_list_to_file.mk
#end of file creation

build_outputs :
	$(LD) /OUT:"$(LINKER_OUTPUT)" $(LDFLAGS) $(LIBRARIES_DIRS) @$(ALL_OBJECTS_LIST_FILE) $(LIBS) 
	$(CP)  $(LINKER_OUTPUT) $(LINKER_OUTPUT_NAMED)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
