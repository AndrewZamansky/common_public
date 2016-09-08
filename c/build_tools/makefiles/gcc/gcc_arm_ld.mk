

ELF_TO_BIN		:=	$(FULL_GCC_PREFIX)objcopy -O binary
DISASSEMBLER	:=	$(FULL_GCC_PREFIX)objdump -d
ELF_TO_HEX		:=	$(FULL_GCC_PREFIX)objcopy -O ihex


GLOBAL_LIBS := $(GLOBAL_LIBS)

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES := libc.a libm.a libc_nano.a libgcc.a
   GLOBAL_LIBS := $(GLOBAL_LIBS) $(STD_LIBRARIES)
   ifdef CONFIG_TOOLCHAIN_LIBRARIES_ARE_SPEED_CRUCIAL
       CRITICAL_SPEED_STD_LIBRARIES := $(STD_LIBRARIES)
   endif
endif

LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))


#LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).axf
LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).elf
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_APP_NAME).bin
OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).hex



#init LDFLAGS
LDFLAGS :=

#LDFLAGS += -fno-builtin-printf

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
    LDFLAGS += -mthumb
endif



LDFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -mthumb-interwork -Wl,--gc-sections -nostartfiles  # -msoft-float -mfloat-abi=soft
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft

ifdef CONFIG_USE_NANO_STD_LIBS
    LDFLAGS += -specs=nano.specs
endif

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
    else
        LDFLAGS += -mfloat-abi=soft -mfpu=soft
    endif
endif


LDFLAGS += -g -g3 -ggdb3 #-gstabs3  #-O0   

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)


FILES_TO_FORCE_IN_RAM := $(SPEED_CRITICAL_FILES) $(CRITICAL_SPEED_STD_LIBRARIES)
FILES_TO_FORCE_IN_RAM := $(sort $(FILES_TO_FORCE_IN_RAM)) 
FILES_TO_FORCE_IN_RAM := $(patsubst %.c,%.o,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %.s,%.o.asm,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %.S,%.O.asm,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %,*%,$(FILES_TO_FORCE_IN_RAM))# add * to deal with libraries pathes
FILES_TO_FORCE_IN_RAM := $(FILES_TO_FORCE_IN_RAM) 123_DUMMY.X # add dummy file to create non-empy field in lds file for correct syntax
############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############


LDS_PREPROCESSOR_DEFINES :=
LDS_PREPROCESSOR_DEFINES += DEBUG_SECTIONS_INCLUDE_FILE="\"$(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm_gcc/debug_sections.lds\""
ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
    LDS_PREPROCESSOR_DEFINES += CORTEX_M
endif
LDS_PREPROCESSOR_DEFINES_FRMT 	:= $(patsubst %,-D%,$(LDS_PREPROCESSOR_DEFINES))
LDS_PREPROCESSOR_DEFINES_FRMT := $(LDS_PREPROCESSOR_DEFINES_FRMT) -DFILES_TO_FORCE_IN_RAM="$(FILES_TO_FORCE_IN_RAM)"


##########################################################


ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/fpu
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/thumb
else	
    GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)
endif


OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)r$(DATE_STR).bin
LINKER_HISTORY_OUTPUT := $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR).elf


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

ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif

build_outputs :
	$(CC) -E -P -x c -I z_auto_generated_files $(LDS_PREPROCESSOR_DEFINES_FRMT)  $(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm_gcc/arm_scatter_file_pattern.lds -o $(OUT_DIR)/$(OUTPUT_APP_NAME).lds
	$(LD) $(LDFLAGS) -T $(OUT_DIR)/$(OUTPUT_APP_NAME).lds  $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) -o $(LINKER_OUTPUT)
	$(DISASSEMBLER) $(LINKER_OUTPUT) > $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
	$(ELF_TO_BIN) $(LINKER_OUTPUT) $(OUTPUT_BIN)
	$(ELF_TO_HEX) $(LINKER_OUTPUT) $(OUTPUT_HEX)
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif
