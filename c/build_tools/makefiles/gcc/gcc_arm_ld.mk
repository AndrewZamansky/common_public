


GLOBAL_LIBS := $(GLOBAL_LIBS) $(CONFIG_INCLUDE_STD_LIBRARIES)

LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).hex

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

ifeq ($(findstring YES,$(CONFIG_POSITION_INDEPENDENT)),YES) 	 
	LDFLAGS += -Wl,-pie
endif

LDFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -mthumb-interwork -Wl,--gc-sections -nostartfiles  # -msoft-float -mfloat-abi=soft  
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft  

ifeq ($(findstring YES,$(CONFIG_USE_NANO_STD_LIBS)),YES) 	 
    LDFLAGS += -specs=nano.specs 
endif

ifeq ($(findstring cortex-m4,$(CONFIG_CPU_TYPE)),cortex-m4) 	
    LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 
endif


LDFLAGS += -g -g3 -ggdb3 #-gstabs3  #-O0   

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)


FILES_TO_FORCE_IN_RAM := $(SPEED_CRITICAL_FILES) $(CONFIG_CRITICAL_SPEED_STD_LIBRARIES)
FILES_TO_FORCE_IN_RAM := $(sort $(FILES_TO_FORCE_IN_RAM)) 
FILES_TO_FORCE_IN_RAM := $(patsubst %.c,%.o,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %.s,%.o.asm,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %.S,%.O.asm,$(FILES_TO_FORCE_IN_RAM))
FILES_TO_FORCE_IN_RAM := $(patsubst %,*%,$(FILES_TO_FORCE_IN_RAM))# add * to deal with libraries pathes
FILES_TO_FORCE_IN_RAM := $(FILES_TO_FORCE_IN_RAM) 123_DUMMY.X # add dummy file to create non-empy field in lds file for correct syntax
############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############
LDS_PREPROCESSOR_DEFINES := 
LDS_PREPROCESSOR_DEFINES += CONFIG_RAM_START_ADDR=$(CONFIG_RAM_START_ADDR) CONFIG_RAM_SIZE=$(CONFIG_RAM_SIZE)
LDS_PREPROCESSOR_DEFINES += CONFIG_FLASH_START_ADDR=$(CONFIG_FLASH_START_ADDR) CONFIG_FLASH_SIZE=$(CONFIG_FLASH_SIZE)
LDS_PREPROCESSOR_DEFINES += DEBUG_SECTIONS_INCLUDE_FILE="\"$(BUILD_TOOLS_ROOT_DIR)/scatter_files/gcc/debug_sections.lds\""
ifeq ($(findstring flash,$(CONFIG_CODE_LOCATION)),flash)
	LDS_PREPROCESSOR_DEFINES += RUN_FROM_FLASH
endif
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


OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(OUTPUT_APP_NAME).v$(DATE_STR).bin


rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#some time on windows .O.asm and .o.asm will appear as same files . so $(sort) will eliminate duplication


ALL_OBJ_FILES := $(sort $(call rwildcard,$(OBJ_DIR)/,*.o) $(call rwildcard,$(OBJ_DIR)/,*.o.asm) $(call rwildcard,$(OBJ_DIR)/,*.O.asm))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	
	OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
	OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
	OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif

build_outputs : 
	$(CC) -E -P -x c $(LDS_PREPROCESSOR_DEFINES_FRMT)  $(BUILD_TOOLS_ROOT_DIR)/scatter_files/gcc/scatter_file_pattern.lds -o $(OUT_DIR)/$(OUTPUT_APP_NAME).lds
	$(LD) $(LDFLAGS) -T $(OUT_DIR)/$(OUTPUT_APP_NAME).lds  $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) -o $(LINKER_OUTPUT)
	$(DISASSEMBLER) $(LINKER_OUTPUT) > $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
	$(ELF_TO_BIN) $(LINKER_OUTPUT) $(OUTPUT_BIN)
	 $(ELF_TO_HEX) $(LINKER_OUTPUT) $(OUTPUT_HEX)
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN) 
ifeq ($(findstring YES,$(CONFIG_CALCULATE_CRC32)),YES) 	 
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32) 
endif	
ifeq ($(findstring YES,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),YES) 	 
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif	

