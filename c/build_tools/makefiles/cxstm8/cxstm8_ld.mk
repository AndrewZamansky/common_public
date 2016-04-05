

LD				:=	"$(CXSTM8_ROOT_DIR)\clnk"  
STM8_TO_HEX		:=	"$(CXSTM8_ROOT_DIR)\chex"
HEX_TO_BIN		:=	"$(TOOLS_ROOT_DIR)\HexToBin\bin\mot2bin.exe"
LINKER_OUTPUT 	:=  $(OUT_DIR)/$(OUTPUT_APP_NAME).stm8
OUTPUT_BIN 		:=  $(OUT_DIR)/$(OUTPUT_APP_NAME).bin
OUTPUT_HEX 		:=  $(OUT_DIR)/$(OUTPUT_APP_NAME).s19

#caclulating linker flags

ifdef CONFIG_INCLUDE_FPU
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif

#ifdef CONFIG_CODE_LOCATION_FLASH
#	GLOBAL_LDFLAGS += --ro_base=$(CONFIG_FLASH_START_ADDR)
#	GLOBAL_LDFLAGS += --entry=$(CONFIG_FLASH_START_ADDR)
#else
#	GLOBAL_LDFLAGS += --ro_base=$(CONFIG_RAM_START_ADDR)
#	GLOBAL_LDFLAGS += --entry=$(CONFIG_RAM_START_ADDR)
#endif

GLOBAL_LDFLAGS += --library_type=microlib --strict 
GLOBAL_LDFLAGS += --map --datacompressor=off --info=inline --entry do_startup 
GLOBAL_LDFLAGS += --summary_stderr --info summarysizes --info sizes --info totals 
GLOBAL_LDFLAGS += --list $(OUT_DIR)/$(OUTPUT_APP_NAME).map

GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) #add this line to stop calculate GLOBAL_LDFLAGS recursively


############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############
LDS_PREPROCESSOR_DEFINES +=
LDS_PREPROCESSOR_DEFINES_FRMT 	= $(patsubst %,-d%,$(LDS_PREPROCESSOR_DEFINES))
##########################################################

#list all files in $(OBJ_DIR) and exclude stm8_interrupt_vector.o file (it's included manually in scatter file)
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
ALL_OBJ_FILES := $(sort $(call rwildcard,$(OBJ_DIR)/,*.o) $(call rwildcard,$(OBJ_DIR)/,*.o.asm) $(call rwildcard,$(OBJ_DIR)/,*.O.asm))
ALL_OBJ_FILES := $(patsubst %stm8_interrupt_vector.o,,$(ALL_OBJ_FILES))
ALL_OBJ_FILES := $(patsubst %stacks.o,,$(ALL_OBJ_FILES))

OUTPUT_HISTORY_BIN :=  $(OUT_DIR_HISTORY)/$(OUTPUT_APP_NAME).v$(DATE_STR).bin

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
	#replace backslash for slash
	OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
	OUTPUT_HEX := $(subst /,\,$(OUTPUT_HEX))
	OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
	OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
endif



build_outputs : 
	$(PREPROCESSOR) -e -dOBJ_DIR="$(OBJ_DIR)" -dFILES_LIST="$(ALL_OBJ_FILES)" $(LDS_PREPROCESSOR_DEFINES_FRMT)  $(BUILD_TOOLS_ROOT_DIR)/scatter_files/cxstm8/scatter_file_pattern.lkf > $(OUT_DIR)/$(OUTPUT_APP_NAME).lkf
	$(LD)  -m $(OUT_DIR)/$(OUTPUT_APP_NAME).map -o $(LINKER_OUTPUT) $(OUT_DIR)/$(OUTPUT_APP_NAME).lkf
	$(CXSTM8_ROOT_DIR)\cvdwarf  $(LINKER_OUTPUT) -o $(OUT_DIR)/$(OUTPUT_APP_NAME).elf
	$(STM8_TO_HEX)  -o $(OUTPUT_HEX) $(LINKER_OUTPUT)
	$(HEX_TO_BIN) $(OUTPUT_HEX) 
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN) 
ifeq ($(findstring YES,$(CONFIG_CALCULATE_CRC32)),YES) 	 
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32) 
endif	
