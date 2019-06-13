

ifdef CONFIG_MSC_TARGET_ARCH_X86
    LD := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\link"
else ifdef CONFIG_MSC_TARGET_ARCH_X64
    LD := $(MSVC_SET_ADDITIONAL_PATHS) "$(MSVC_BIN_DIR)\link"
endif

LIBS := $(sort $(GLOBAL_LIBS))

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES :=
   LIBS += $(STD_LIBRARIES)
endif



ifdef CONFIG_MSC_TARGET_ARCH_X86
    ARCH_NAME :=x86_32
else
    ARCH_NAME :=x86_64
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DBG_NAME :=.dbg
else
    DBG_NAME :=
endif

NAMED_OUT_PREFIX :=$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME)

HISTORY_OUT_PREFIX :=$(NAMED_OUT_PREFIX)_$(MAIN_VERSION_STR)
HISTORY_OUT_PREFIX :=$(HISTORY_OUT_PREFIX)$(REVISION_FOR_FILE_STR)

ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
    EXTENSION :=dll
else
    EXTENSION :=exe
endif

LINKER_OUTPUT :=$(call fix_path_if_in_windows,\
                              $(OUT_DIR)/$(OUTPUT_APP_NAME).$(EXTENSION))
LINKER_LIB_OUTPUT :=$(call fix_path_if_in_windows,\
                             $(OUT_DIR)/$(OUTPUT_APP_NAME).lib)
LINKER_OUTPUT_NAMED :=$(call fix_path_if_in_windows,\
                       $(OUT_DIR)/$(NAMED_OUT_PREFIX).$(EXTENSION))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,\
                  $(OUT_DIR_HISTORY)/$(HISTORY_OUT_PREFIX).$(EXTENSION))
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))


#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf

ifdef CONFIG_MSC_TARGET_ARCH_X86
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
    #LINK removes unreferenced packaged functions and data:
    LDFLAGS += /OPT:REF
    
    #perform 2(default) interations to find unreferenced code or data :
    LDFLAGS += /OPT:ICF
    
    LDFLAGS += /LTCG
    ifdef CONFIG_MSC_TARGET_ARCH_X86
        LDFLAGS += /SAFESEH
    endif
endif

ifdef CONFIG_MSVC_OUTPUT_TYPE_WINDOWS_APPLICATION
    MANIFEST_FILE :=$(OUT_DIR)\$(PROJECT_NAME).exe.intermediate.manifest
endif
ifdef CONFIG_MSVC_OUTPUT_TYPE_CONSOLE_APPLICATION
    MANIFEST_FILE :=$(OUT_DIR)\$(PROJECT_NAME).exe.intermediate.manifest
endif
ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
    LDFLAGS += /DLL
    LDFLAGS += /IMPLIB:"$(OUT_DIR)\$(PROJECT_NAME).lib"
    MANIFEST_FILE :=$(OUT_DIR)\$(PROJECT_NAME).dll.intermediate.manifest
endif

LDFLAGS += /ManifestFile:"$(MANIFEST_FILE)"

LDFLAGS += /ERRORREPORT:PROMPT /NOLOGO /TLBID:1

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    ifdef CONFIG_MSC_TARGET_ARCH_X86
        LDFLAGS += /LIBPATH:"$(MSVC_BIN_DIR)\..\lib"#for MSVC <= 2015
        LDFLAGS += /LIBPATH:"$(MSVC_BIN_DIR)\..\..\..\lib\x86"#for MSVC >= 2017
    else
        LDFLAGS += /LIBPATH:"$(MSVC_BIN_DIR)\..\lib\amd64"#for MSVC <= 2015
        LDFLAGS += /LIBPATH:"$(MSVC_BIN_DIR)\..\..\..\lib\x64"#for MSVC >= 2017
    endif
endif

ifdef CONFIG_USE_WINDOWS_KITS
    WINDOWS_KITS_LIBRARIES := kernel32.lib user32.lib gdi32.lib winspool.lib
    WINDOWS_KITS_LIBRARIES += comdlg32.lib advapi32.lib shell32.lib ole32.lib
    WINDOWS_KITS_LIBRARIES += oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
    LIBS += $(WINDOWS_KITS_LIBRARIES)
    LIBS := $(sort $(LIBS))

    ifeq ($(VS_VERSION),2012)
        ifdef CONFIG_MSC_TARGET_ARCH_X86
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\WIN8\UM\X86"
        else
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\WIN8\UM\X64"
        endif
    endif

    ifeq ($(VS_VERSION),2013)
        ifdef CONFIG_MSC_TARGET_ARCH_X86
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\WINV6.3\UM\X86"
        else
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\WINV6.3\UM\X64"
        endif
    endif
    
    ifneq ($(filter 2015 2017,$(VS_VERSION)),)# if 2015 or 2017
        ifdef CONFIG_MSC_TARGET_ARCH_X86
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\$(WDK_10_VERSION)\ucrt\x86"
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\$(WDK_10_VERSION)\um\x86"
        else
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\$(WDK_10_VERSION)\ucrt\x64"
            LDFLAGS += /LIBPATH:"$(WDK_DIR)\LIB\$(WDK_10_VERSION)\um\x64"
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





rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))



ALL_OBJ_FILES := $(call rwildcard,$(OBJ_DIR)/,*.o)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oo)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oop) 
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
# some time on windows .O.asm and .o.asm will appear as same files,
# so $(sort) will eliminate duplication
ALL_OBJ_FILES := $(sort $(ALL_OBJ_FILES))

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file.mk
#end of file creation

LINKER_CMD =$(LD) /OUT:"$(LINKER_OUTPUT)" $(LDFLAGS) 
LINKER_CMD +=$(LIBRARIES_DIRS) @$(ALL_OBJECTS_LIST_FILE) $(LIBS) 

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_OUTPUT_NAMED)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
