

LD :="$(CXSTM8_ROOT_DIR)\clnk"  
STM8_TO_HEX :="$(CXSTM8_ROOT_DIR)\chex"
HEX_TO_BIN :="$(TOOLS_ROOT_DIR)\HexToBin\bin\mot2bin.exe"

#caclulating linker flags



GLOBAL_LDFLAGS += --library_type=microlib --strict 
GLOBAL_LDFLAGS += --map --datacompressor=off --info=inline --entry do_startup 
GLOBAL_LDFLAGS += --summary_stderr --info summarysizes --info sizes --info totals 
GLOBAL_LDFLAGS += --list $(OUT_DIR)/$(OUTPUT_APP_NAME).map

GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) #add this line to stop calculate GLOBAL_LDFLAGS recursively


############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############

LDS_PREPROCESSOR_DEFINES :=
ifdef CONFIG_ATOM_THREADS
    LDS_PREPROCESSOR_DEFINES += CONFIG_ATOM_THREADS
endif
LDS_PREPROCESSOR_DEFINES_FRMT 	= $(patsubst %,-d%,$(LDS_PREPROCESSOR_DEFINES))
##########################################################

#list all files in $(OBJ_DIR) and exclude stm8_interrupt_vector.o file (it's included manually in scatter file)
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
ALL_OBJ_FILES := $(sort $(call rwildcard,$(OBJ_DIR)/,*.o) $(call rwildcard,$(OBJ_DIR)/,*.o.asm) $(call rwildcard,$(OBJ_DIR)/,*.O.asm))
ALL_OBJ_FILES := $(patsubst %stm8_interrupt_vector.o,,$(ALL_OBJ_FILES))
ALL_OBJ_FILES := $(patsubst %stacks.o,,$(ALL_OBJ_FILES))

#need to split in two variables because preprossesor cannot deal with strings larger then 2048 chars
OBJ_FILES1 := $(wordlist 1, 5, $(ALL_OBJ_FILES))
OBJ_FILES2 := $(wordlist 6, 1000, $(ALL_OBJ_FILES))

LINKER_OUTPUT :=$(call fix_path_if_in_windows,\
                                     $(OUT_DIR)/$(OUTPUT_APP_NAME).stm8)
OUTPUT_BIN :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_APP_NAME).bin)
OUTPUT_HEX :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_APP_NAME).s19)
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))
OUTPUT_HISTORY_BIN :=$(call fix_path_if_in_windows,\
      $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)r$(DATE_STR).bin)
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,\
                $(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR).stm8)


build_outputs : 
	$(PREPROCESSOR) -e -dOBJ_DIR="$(OBJ_DIR)" -dFILES_LIST1="$(OBJ_FILES1)" -dFILES_LIST2="$(OBJ_FILES2)" $(LDS_PREPROCESSOR_DEFINES_FRMT)  $(BUILD_TOOLS_ROOT_DIR)/scatter_files/cxstm8/scatter_file_pattern.lkf > $(OUT_DIR)/$(OUTPUT_APP_NAME).lkf
	$(LD)  -m $(OUT_DIR)/$(OUTPUT_APP_NAME).map -o $(LINKER_OUTPUT) $(OUT_DIR)/$(OUTPUT_APP_NAME).lkf
	$(CXSTM8_ROOT_DIR)\cvdwarf  $(LINKER_OUTPUT) -o $(OUT_DIR)/$(OUTPUT_APP_NAME).elf
	$(STM8_TO_HEX)  -o $(OUTPUT_HEX) $(LINKER_OUTPUT)
	$(HEX_TO_BIN) $(OUTPUT_HEX) 
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN) 
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
	@echo ----
	@echo ---- for debuging in STVD do the following :
	@echo ---- 1: remove {some_path}/stvd  entry from   project settings/Debug/Source directories
	@echo ---- 2: add $(COMMON_PUBLIC_DIR)/hw_drivers/arch/STM8/src/stm8_interrupt_vector.c  to Project/Source Files
	@echo ---- 3: add $(COMMON_PUBLIC_DIR)/hw_drivers/arch/STM8/src/stm8_interrupt_vector.c  to  project settings/Linker/Input/Vector file name
	@echo ----
ifeq ($(findstring YES,$(CONFIG_CALCULATE_CRC32)),YES) 	 
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32) 
endif
