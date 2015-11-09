#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

GCC_ROOT_DIR := $(ARMCC_ROOT_DIR)

CC   :=	$(ARMCC_ROOT_DIR)/bin/armcc
ASM  :=	$(ARMCC_ROOT_DIR)/bin/armasm
LD   :=	$(ARMCC_ROOT_DIR)/bin/armlink
ELF_TO_BIN	:=	$(ARMCC_ROOT_DIR)/bin/fromelf --bin
DISASSEMBLER	:=	$(ARMCC_ROOT_DIR)/bin/fromelf -c


LINKER_OUTPUT :=  $(OUTPUT_BIN).axf
OUTPUT_BIN := $(OUTPUT_BIN).bin

GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -c --apcs=interwork --split_sections --gnu --c99

ifeq ($(findstring YES,$(CONFIG_INCLUDE_FPU)),YES)
   GLOBAL_CFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_CFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif


ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif


GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -$(CONFIG_OPTIMIZE_LEVEL) -g 


#caclulating assembler flags

ifeq ($(findstring YES,$(CONFIG_INCLUDE_FPU)),YES)
   GLOBAL_ASMFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_ASMFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif
GLOBAL_ASMFLAGS += -g --16 --apcs=interwork

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS) #add this line to stop calculate GLOBAL_ASMFLAGS recursively

#caclulating linker flags

ifeq ($(findstring YES,$(CONFIG_INCLUDE_FPU)),YES)
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif

#ifeq ($(findstring flash,$(CONFIG_CODE_LOCATION)),flash)
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


#end of flags definitions

GLOBAL_DEFINES := $(GLOBAL_DEFINES) ARMCC


############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############
LDS_PREPROCESSOR_DEFINES += CONFIG_RAM_START_ADDR=$(CONFIG_RAM_START_ADDR) CONFIG_RAM_SIZE=$(CONFIG_RAM_SIZE)
LDS_PREPROCESSOR_DEFINES += CONFIG_FLASH_START_ADDR=$(CONFIG_FLASH_START_ADDR) CONFIG_FLASH_SIZE=$(CONFIG_FLASH_SIZE)
LDS_PREPROCESSOR_DEFINES += DEBUG_SECTIONS_INCLUDE_FILE="\"$(BUILD_TOOLS_ROOT_DIR)/scatter_files/gcc/debug_sections.lds\""
ifeq ($(findstring flash,$(CONFIG_CODE_LOCATION)),flash)
	LDS_PREPROCESSOR_DEFINES += RUN_FROM_FLASH
endif
ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
	LDS_PREPROCESSOR_DEFINES += CORTEX_M
endif
LDS_PREPROCESSOR_DEFINES_FRMT 	= $(patsubst %,-D%,$(LDS_PREPROCESSOR_DEFINES))
##########################################################




endif
### end of section that run just one time per build
#####################################################
