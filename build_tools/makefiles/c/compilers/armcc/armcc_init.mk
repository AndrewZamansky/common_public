GCC_ROOT_DIR := $(ARMCC_ROOT_DIR)

CC   :=	$(ARMCC_ROOT_DIR)/bin/armcc
ASM  :=	$(ARMCC_ROOT_DIR)/bin/armasm
LD   :=	$(ARMCC_ROOT_DIR)/bin/armlink
ELF_TO_BIN	:=	$(ARMCC_ROOT_DIR)/bin/fromelf --bin
DISASSEMBLER	:=	$(ARMCC_ROOT_DIR)/bin/fromelf -c


LINKER_OUTPUT :=  $(OUTPUT_BIN).axf
OUTPUT_BIN := $(OUTPUT_BIN).bin

GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -c --apcs=interwork --split_sections --gnu --c99

ifdef CONFIG_INCLUDE_CORTEX_M_FPU
   GLOBAL_CFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_CFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif


ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif


GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -$(CONFIG_OPTIMIZE_LEVEL) -g 


#caclulating assembler flags

ifdef CONFIG_INCLUDE_CORTEX_M_FPU
   GLOBAL_ASMFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_ASMFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif
GLOBAL_ASMFLAGS += -g --16 --apcs=interwork

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS) #add this line to stop calculate GLOBAL_ASMFLAGS recursively

#caclulating linker flags

ifdef CONFIG_INCLUDE_CORTEX_M_FPU
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE).fp
else 	 
   GLOBAL_LDFLAGS += --cpu=$(CONFIG_CPU_TYPE) 
endif



GLOBAL_LDFLAGS += --library_type=microlib --strict 
GLOBAL_LDFLAGS += --map --datacompressor=off --info=inline --entry do_startup 
GLOBAL_LDFLAGS += --summary_stderr --info summarysizes --info sizes --info totals 
GLOBAL_LDFLAGS += --list $(OUT_DIR)/$(OUTPUT_APP_NAME).map

GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) #add this line to stop calculate GLOBAL_LDFLAGS recursively


#end of flags definitions



############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############
LDS_PREPROCESSOR_DEFINES += RAM_START_ADDR=$(RAM_START_ADDR) RAM_SIZE=$(RAM_SIZE)
LDS_PREPROCESSOR_DEFINES += ROM_START_ADDR=$(ROM_START_ADDR) ROM_SIZE=$(ROM_SIZE)
LDS_PREPROCESSOR_DEFINES += DEBUG_SECTIONS_INCLUDE_FILE="\"$(BUILD_TOOLS_ROOT_DIR)/scatter_files/gcc/debug_sections.lds\""
ifdef CONFIG_CODE_LOCATION_FLASH
	LDS_PREPROCESSOR_DEFINES += RUN_FROM_FLASH
endif
ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
	LDS_PREPROCESSOR_DEFINES += CORTEX_M
endif
LDS_PREPROCESSOR_DEFINES_FRMT 	= $(patsubst %,-D%,$(LDS_PREPROCESSOR_DEFINES))
##########################################################
