
LD   :=	set "PATH=$(TEST_MCC_ROOT_DIR)/bin" & "$(FULL_MCC_PREFIX)link"
DISASSEMBLER	:=	$(FULL_GCC_PREFIX)objdump -d


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

ifdef CONFIG_MCC_OUTPUT_TYPE_EXE
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
    OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_APP_NAME).stripped.exe
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR).exe
    OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)r$(DATE_STR).stripped.exe
endif

ifdef CONFIG_MCC_OUTPUT_TYPE_DLL
    LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).dll
    LINKER_LIB_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).lib
    LINKER_OUTPUT_NAMED := $(OUT_DIR)/$(PROJECT_NAME).dll
    LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR).dll
    OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)r$(DATE_STR).stripped.dll
endif


#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf


LDFLAGS += /MANIFEST /NXCOMPAT /PDB:"$(OUT_DIR)\out.pdb" /DYNAMICBASE 
LDFLAGS += /LARGEADDRESSAWARE /DEBUG /MACHINE:X86 /NODEFAULTLIB:"libcmt.lib" /NODEFAULTLIB:"msvcrt.lib"
LDFLAGS += /SAFESEH:NO# /PGD:"$(OUT_DIR)\out.pgd"
LDFLAGS += /SUBSYSTEM:WINDOWS 
LDFLAGS += /MANIFESTUAC:"level='asInvoker' uiAccess='false'" 
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
    LDFLAGS += /LIBPATH:"$(WINDOWS_KIT_ROOT_DIR)\LIB\WINV6.3\UM\X86"
endif

LDFLAGS += /LIBPATH:"$(MCC_ROOT_DIR)\lib"
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

#
#	$(DISASSEMBLER) $(LINKER_OUTPUT) > $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
#	$(CP)  $(LINKER_OUTPUT) $(OUTPUT_BIN)
#	$(STRIP) $(OUTPUT_BIN)
#	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
build_outputs :
	$(LD) /OUT:"$(LINKER_OUTPUT)" $(LDFLAGS) $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) 
	$(CP)  $(LINKER_OUTPUT) $(LINKER_OUTPUT_NAMED)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
