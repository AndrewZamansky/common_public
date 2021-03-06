WORKSPACE_CFG :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk

ifdef REDEFINE_PIC32_GCC_ROOT_DIR
    $(info  pic32 gcc dir  redefined to $(REDEFINE_PIC32_GCC_ROOT_DIR))
    ifeq ("$(wildcard $(REDEFINE_PIC32_GCC_ROOT_DIR))","")
        $(info err: pic32 gcc path $(REDEFINE_PIC32_GCC_ROOT_DIR) dont exists)
        $(info ---: To use default pic32 gcc location remove/comment
        $(info ---:  REDEFINE_PIC32_GCC_ROOT_DIR variable in $(WORKSPACE_CFG))
        $(info ---: You can set customized gcc path in REDEFINE_PIC32_GCC_ROOT_DIR)
        $(info ---:  variable in $(WORKSPACE_CFG))
        $(call exit,1)
    else
        GCC_ROOT_DIR 	:= 	$(REDEFINE_PIC32_GCC_ROOT_DIR)
    endif
else
    ifdef CONFIG_PIC32_COMPILER_LOCATION_WINDOWS_DEFAULT
        $(info  looking for pic32 gcc in default windows location)
        TOOLS_ROOT_DIR :=C:\Program\ Files\ (x86)\Microchip\MPLAB\ C32\ Suite
        ifeq ("$(wildcard $(TOOLS_ROOT_DIR))","")
            $(info err: path  $(TOOLS_ROOT_DIR) does not exist)
            $(info ---: get MPLAB C32 Suite and put it into $(TOOLS_ROOT_DIR))
            $(info ---: make sure thatpic32-libs,bin and pic32mx  folders is)
            $(info ---: located in $(TOOLS_ROOT_DIR)/  after unpacking)
            $(info ---: or use set CONFIG_PIC32_COMPILER_LOCATION_WORKSPACE_TOOLS_DIRECTORY)
            $(info ---: variable in menuconfig)
            $(call exit,1)
        endif    
        GCC_ROOT_DIR :=C:\Program Files (x86)\Microchip\MPLAB C32 Suite
    else ifdef CONFIG_PIC32_COMPILER_LOCATION_WORKSPACE_TOOLS_DIRECTORY 
        $(info  looking for pic32 gcc in workspace tools location)
        ifeq ("$(wildcard $(TOOLS_ROOT_DIR)/gcc/pic32)","")
            $(info err: path  $(TOOLS_ROOT_DIR)/gcc/pic32 dont exists create it )
            $(call exit,1)
        endif
        
        TEST_GCC_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/gcc/pic32/c32
        ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR)*)","")
            $(info --- gcc for pic32 dont exists )
            GCC_NOT_FOUND :=1
        endif
        
        PIC32_C32_VERSION :=$(patsubst "%",%,$(CONFIG_PIC32_C32_VERSION))
        ifndef GCC_NOT_FOUND
           TEST_GCC_ROOT_DIR :=$(TOOLS_ROOT_DIR)/gcc/pic32/c32-$(PIC32_C32_VERSION)
           ifeq ("$(wildcard $(TEST_GCC_ROOT_DIR))","")
               $(info err: $(TEST_GCC_ROOT_DIR) dont exists)
               $(info ---: (if needed you can change compiler version using)
               $(info ---:  menuconfig in "Building System" menu))
               GCC_NOT_FOUND :=1
           endif
    	endif


        ifdef GCC_NOT_FOUND
            TEST_GCC_ROOT_DIR :=$(TOOLS_ROOT_DIR)/gcc/pic32/c32-$(PIC32_C32_VERSION)
            $(info err: gcc path $(TEST_GCC_ROOT_DIR) does not exist)
            $(info ---: Get MPLAB C32 Suite and put it into $(TEST_GCC_ROOT_DIR))
            $(info ---:  make sure thatpic32-libs,bin and pic32mx folders is)
            $(info ---:  located in $(TEST_GCC_ROOT_DIR)/  after unpacking.)
            $(info ---: Or use set CONFIG_PIC32_COMPILER_LOCATION_WORKSPACE_TOOLS_DIRECTORY)
            $(info ---:  in menuconfig)
            $(info ---: You can also set customized gcc path in)
            $(info ---:  REDEFINE_PIC32_GCC_ROOT_DIR variable in $(WORKSPACE_CFG))
            $(call exit,1)
        endif

        #take the latest gcc version
        GCC_ROOT_DIR :=$(lastword $(wildcard $(TEST_GCC_ROOT_DIR)))
        
    endif

endif



ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif

GNU_COMPILATION_PREFIX	:=pic32-


ifdef CONFIG_GCC_OPTIMISE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_GCC_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_GCC_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_GCC_OPTIMISE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_GCC_OPTIMISE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
endif



### GLOBAL_CFLAGS calculation

GLOBAL_CFLAGS += -Wall -fdata-sections


ifdef CONFIG_GCC_OPTIMISE_SIZE
	GLOBAL_CFLAGS +=  -ffunction-sections
	GLOBAL_CFLAGS +=  -mips16
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL) -g -MMD

ifdef CONFIG_PIC32MX575
    GLOBAL_CFLAGS += -mprocessor=32MX575F512L
endif

#stop GLOBAL_CFLAGS calculation each time it used
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation

GLOBAL_ASMFLAGS += -x assembler-with-cpp

GLOBAL_ASMFLAGS += -g

#stop GLOBAL_ASMFLAGS calculation each time it used
GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

#end of flags definitions



FULL_GCC_PREFIX 		:= $(GCC_ROOT_DIR)/bin/$(GNU_COMPILATION_PREFIX)
COMPILER_INCLUDE_DIR 	:= $(GCC_ROOT_DIR)/pic32mx/include
GCC_LIB_ROOT_DIR  		:= $(GCC_ROOT_DIR)/pic32mx/lib

CC   :=	"$(FULL_GCC_PREFIX)gcc" -c
ASM  :=	"$(FULL_GCC_PREFIX)gcc" -c
LD   :=	"$(FULL_GCC_PREFIX)gcc"
