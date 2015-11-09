
##################################################################
#### the following section we need to run just one time per build
#ifndef SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS
#

FMT_GLOBAL_INCLUDE_DIR	:= 	$(patsubst %,-I%,$(GLOBAL_INCLUDE_DIR))
FMT_GLOBAL_DEFINES	:= 	$(patsubst %,-D%,$(GLOBAL_DEFINES))
FMT_GLOBAL_ASM_DEFINES := $(patsubst %,--predefine "% SETL {TRUE}",$(GLOBAL_DEFINES))


LDFLAGS = $(GLOBAL_LDFLAGS)

#endif###
### end of section that run just one time per build
#####################################################

define CALCULATE_ALL_INCLUDE_DIRS
	ALL_INCLUDE_DIRS = $(FMT_GLOBAL_INCLUDE_DIR) $(patsubst %,-I%,$(INCLUDE_DIR) $(COMPILER_INCLUDE_DIR))
endef

define CALCULATE_ALL_DEFINES
	ALL_DEFINES	=  $(subst ",\", $(FMT_GLOBAL_DEFINES) $(patsubst %,-D%,$(DEFINES)) )#substitute " to \" for string defines
endef

define CALCULATE_ALL_ASM_DEFINES
	ALL_ASM_DEFINES	=
	#ALL_ASM_DEFINES	=$(FMT_GLOBAL_ASM_DEFINES)   $(patsubst %,--predefine "% SETL {TRUE}",$(DEFINES))
endef



define CALCULATE_CC_OUTPUT_FLAG_AND_FILE
	CC_OUTPUT_FLAG_AND_FILE = -o $$@#this will transform to $@ in make rule 
endef

define CALCULATE_ASM_OUTPUT_FLAG_AND_FILE
	ASM_OUTPUT_FLAG_AND_FILE = -o $$@#this will transform to $@ in make rule 
endef

define CALCULATE_LINKER_FILE_SETUP_CMD_LINE
	LINKER_FILE_SETUP = $(CC) -E -P $(LDS_PREPROCESSOR_DEFINES_FRMT)  $(BUILD_TOOLS_ROOT_DIR)/scatter_files/armcc/scatter_file_pattern.lds -o $(OUT_DIR)/$(OUTPUT_APP_NAME).lds
endef


ALL_OBJ_FILES = $$(wildcard $(OBJ_DIR)/*)
define CALCULATE_LINKER_CMD_LINE
	LINKER_CMD_LINE = $(LD) $(LDFLAGS)  --scatter $(OUT_DIR)/$(OUTPUT_APP_NAME).lds $(ALL_OBJ_FILES) -o $(LINKER_OUTPUT)
endef

define CALCULATE_DISASSEMBLER_CMD_LINE
	DISASSEMBLER_CMD_LINE = $(DISASSEMBLER) $(LINKER_OUTPUT) --output $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
endef

define CALCULATE_BIN_GENERATION_CMD_LINE
	BIN_GENERATION_CMD_LINE = $(ELF_TO_BIN) $(LINKER_OUTPUT) --output $(OUTPUT_BIN)
endef
