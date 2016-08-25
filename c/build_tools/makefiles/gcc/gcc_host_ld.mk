

DISASSEMBLER	:=	$(FULL_GCC_PREFIX)objdump -d


GLOBAL_LIBS := $(GLOBAL_LIBS)

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES := libm.a libgcc.a
   GLOBAL_LIBS := $(GLOBAL_LIBS) $(STD_LIBRARIES)
   ifdef CONFIG_TOOLCHAIN_LIBRARIES_ARE_SPEED_CRUCIAL
       CRITICAL_SPEED_STD_LIBRARIES := $(STD_LIBRARIES)
   endif
endif

LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).hex

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).exe
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_APP_NAME).stripped.exe



#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf


LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft
LDFLAGS += -g -g3 -ggdb3

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)


############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############




##########################################################


ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/fpu
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/thumb
else	
    GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)
endif


LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR).exe
OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)r$(DATE_STR).stripped.exe


rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#some time on windows .O.asm and .o.asm will appear as same files . so $(sort) will eliminate duplication


ALL_OBJ_FILES := $(sort $(call rwildcard,$(OBJ_DIR)/,*.o) $(call rwildcard,$(OBJ_DIR)/,*.oo) $(call rwildcard,$(OBJ_DIR)/,*.o.asm) $(call rwildcard,$(OBJ_DIR)/,*.O.asm))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
    OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif



build_outputs :
	$(LD) $(LDFLAGS) $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) -o $(LINKER_OUTPUT)
	$(DISASSEMBLER) $(LINKER_OUTPUT) > $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
	$(CP)  $(LINKER_OUTPUT) $(OUTPUT_BIN)
	$(STRIP) $(OUTPUT_BIN)
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
