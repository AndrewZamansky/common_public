#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

GCC_ROOT_DIR := $(AVR_GCC_ROOT_DIR)

LINKER_OUTPUT :=  $(OUTPUT_BIN).elf
OUTPUT_BIN := $(OUTPUT_BIN).bin
OUTPUT_HEX :=  $(OUTPUT_BIN).hex

ifeq ($(findstring AVR-GCC,$(CONFIG_USE_COMPILER)),AVR-GCC) 	 
	GNU_COMPILATION_PREFIX	:= avr
endif


# Modify this to the CPU you are using
#PART=atmega328p


# GCC flags
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -mmcu=$(CONFIG_CHIP_TYPE)


# define flags for asm compiler :
GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS) -mmcu=$(CONFIG_CHIP_TYPE)



# define flags for linker :
#GLOBAL_LDFLAGS += -mcpu=$(CONFIG_CPU_TYPE) -Wl,--gc-sections -nostartfiles  -specs=nano.specs # -msoft-float -mfloat-abi=soft  
GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) -mmcu=$(CONFIG_CHIP_TYPE) -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft  


#end of flags definitions

############   PREPROCESSOR FLAGS FOR LINKER SCRIPT #############

#LDS_PREPROCESSOR_DEFINES += CONFIG_RAM_START_ADDR=$(CONFIG_RAM_START_ADDR) CONFIG_RAM_SIZE=$(CONFIG_RAM_SIZE)
#LDS_PREPROCESSOR_DEFINES += CONFIG_FLASH_START_ADDR=$(CONFIG_FLASH_START_ADDR) CONFIG_FLASH_SIZE=$(CONFIG_FLASH_SIZE)
#LDS_PREPROCESSOR_DEFINES += DEBUG_SECTIONS_INCLUDE_FILE="\"$(BUILD_TOOLS_ROOT_DIR)/scatter_files/gcc/debug_sections.lds\""
#ifdef CONFIG_CODE_LOCATION_FLASH
#	LDS_PREPROCESSOR_DEFINES += RUN_FROM_FLASH
#endif
#ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
#	LDS_PREPROCESSOR_DEFINES += CORTEX_M
#endif
#LDS_PREPROCESSOR_DEFINES_FRMT 	= $(patsubst %,-D%,$(LDS_PREPROCESSOR_DEFINES))

##########################################################



include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_common_init.mk


endif
### end of section that run just one time per build
#####################################################

