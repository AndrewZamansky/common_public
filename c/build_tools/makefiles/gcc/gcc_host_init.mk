#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


GCC_ROOT_DIR := 

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
	#replace backslash for slash
    GCC_ROOT_DIR := $(HOST_GCC_ROOT_DIR)
    GNU_COMPILATION_PREFIX	:= mingw32
    OUTPUT_BIN := $(OUTPUT_BIN).exe
    FIND_MINGW_PATH=$(shell echo %path%)
    ifeq ($(findstring $(HOST_GCC_ROOT_DIR)\bin,$(FIND_MINGW_PATH)),) 	 
        $(info $(FIND_MINGW_PATH))
        $(error ---- add $(HOST_GCC_ROOT_DIR)\bin to your system PATH environment)
    endif
    
endif

LINKER_OUTPUT :=  $(OUTPUT_BIN)




# define flags for asm compiler :
#GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

# define flags for linker :
GLOBAL_LDFLAGS := $(GLOBAL_PROJECT_SPECIFIC_LDFLAGS)
#GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) -mcpu=$(CONFIG_CPU_TYPE) -Wl,--gc-sections -nostartfiles  -specs=nano.specs # -msoft-float -mfloat-abi=soft  
GLOBAL_LDFLAGS := $(GLOBAL_LDFLAGS) -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map   # -msoft-float -mfloat-abi=soft  



#end of flags definitions





include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_common_init.mk


LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))


endif
### end of section that run just one time per build
#####################################################

