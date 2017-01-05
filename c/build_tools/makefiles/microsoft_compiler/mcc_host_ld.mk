
ifdef CONFIG_MCC_COMPILER_32
    LD := set "PATH=$(MCC_BIN_DIR)" & "$(MCC_BIN_DIR)\link"
else ifdef CONFIG_MCC_COMPILER_64
    LD := set "PATH=$(MCC_BIN_DIR)" & "$(MCC_BIN_DIR)\x86_amd64\link"
endif

GLOBAL_LIBS := $(GLOBAL_LIBS)

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES :=
   GLOBAL_LIBS := $(GLOBAL_LIBS) $(STD_LIBRARIES)
endif

ifdef CONFIG_USE_WINDOWS_KITS
   WINDOWS_KITS_LIBRARIES := kernel32.lib user32.lib gdi32.lib winspool.lib
   WINDOWS_KITS_LIBRARIES += comdlg32.lib advapi32.lib shell32.lib ole32.lib
   WINDOWS_KITS_LIBRARIES += oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
   GLOBAL_LIBS := $(GLOBAL_LIBS) $(WINDOWS_KITS_LIBRARIES)
endif

LIBS := $(patsubst lib%,"%",$(GLOBAL_LIBS))

ifdef CONFIG_MCC_COMPILER_32
    ARCH_NAME :=x86_32
else
    ARCH_NAME :=x86_64
endif

ifdef CONFIG_COMPILE_FOR_DEBUG
    DBG_NAME :=.dbg
else
    DBG_NAME :=
endif

ifdef CONFIG_MCC_OUTPUT_TYPE_EXE
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).exe
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).$(MAIN_VERSION_STR).exe
endif

ifdef CONFIG_MCC_OUTPUT_TYPE_DLL
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).dll
    LINKER_LIB_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).lib
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).dll
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME).$(ARCH_NAME)$(DBG_NAME).$(MAIN_VERSION_STR).dll
endif


#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf

ifdef CONFIG_MCC_COMPILER_32
    LDFLAGS += /MACHINE:X86
else
    LDFLAGS += /MACHINE:X64
endif

LDFLAGS += /MANIFEST /NXCOMPAT /PDB:"$(OUT_DIR)\out.pdb" /DYNAMICBASE 
LDFLAGS += /LARGEADDRESSAWARE# /PGD:"$(OUT_DIR)\out.pgd"
LDFLAGS += /SUBSYSTEM:WINDOWS 
LDFLAGS += /MANIFESTUAC:"level='asInvoker' uiAccess='false'"

ifdef CONFIG_MCC_IGNORE_DEFAULT_LIBRARIES
    ifdef CONFIG_MCC_IGNORE_ALL_DEFAULT_LIBRARIES
        LDFLAGS += /NODEFAULTLIB
    else
        ifdef MCC_IGNORE_LIBCMT_LIBRARY
            LDFLAGS += /NODEFAULTLIB:"libcmt.lib"
        endif
        ifdef MCC_IGNORE_MSVCRT_LIBRARY
            LDFLAGS += /NODEFAULTLIB:"msvcrt.lib"
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
    ifdef CONFIG_MCC_COMPILER_32
        LDFLAGS += /SAFESEH
    endif
endif

ifdef CONFIG_MCC_OUTPUT_TYPE_EXE
    LDFLAGS += /ManifestFile:"$(OUT_DIR)\$(PROJECT_NAME).exe.intermediate.manifest"
endif
ifdef CONFIG_MCC_OUTPUT_TYPE_DLL
    LDFLAGS += /DLL
    LDFLAGS += /IMPLIB:"$(OUT_DIR)\$(PROJECT_NAME).lib"
    LDFLAGS += /ManifestFile:"$(OUT_DIR)\$(PROJECT_NAME).dll.intermediate.manifest"
endif
LDFLAGS += /ERRORREPORT:PROMPT /NOLOGO /TLBID:1

ifdef CONFIG_USE_WINDOWS_KITS
    ifdef CONFIG_MCC_COMPILER_32
        LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\WINV6.3\UM\X86"
        LDFLAGS += /LIBPATH:"$(MCC_ROOT_DIR)\lib"
    else
        LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\WINV6.3\UM\X64"
        LDFLAGS += /LIBPATH:"$(MCC_ROOT_DIR)\lib\amd64"
    endif
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


build_outputs :
	$(LD) /OUT:"$(LINKER_OUTPUT)" $(LDFLAGS) $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) 
	$(CP)  $(LINKER_OUTPUT) $(LINKER_OUTPUT_NAMED)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
