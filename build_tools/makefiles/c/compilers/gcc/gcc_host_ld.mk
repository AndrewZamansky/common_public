ifdef CONFIG_REDEFINED_OUTPUT_NAME

    OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))

    HISTORY_OUTPUT_NAME :=$(OUTPUT_NAME)_$(MAIN_VERSION_STR)
    HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR)

else
#    ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
#        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
#        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
#    else ifdef CONFIG_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME)

        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR)
#    else
#        $(info err: unknown output type)
#        $(call exit,1)
#    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
MAP_FILE := $(OUT_DIR)/$(OUTPUT_NAME).map
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_NAME).stripped

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ifdef CONFIG_OUTPUT_TYPE_APPLICATION
        LINKER_OUTPUT :=$(LINKER_OUTPUT).exe
        LINKER_HISTORY_OUTPUT :=$(LINKER_HISTORY_OUTPUT).exe
        OUTPUT_BIN :=$(OUTPUT_BIN).exe
    endif
endif



DISASSEMBLER	:=	$(FULL_GCC_PREFIX)objdump -d


GLOBAL_LIBS := $(GLOBAL_LIBS)

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
   STD_LIBRARIES := libm.a libgcc.a
   GLOBAL_LIBS := $(GLOBAL_LIBS) $(STD_LIBRARIES)
   ifdef CONFIG_TOOLCHAIN_LIBRARIES_ARE_SPEED_CRUCIAL
       SPEED_CRITICAL_STD_LIBS := $(STD_LIBRARIES)
   endif
endif

LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))


#init LDFLAGS
LDFLAGS := 

#LDFLAGS += -fno-builtin-printf


LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map
LDFLAGS += -g -g3 -ggdb3

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)



GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)



rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_OBJ_FILES := $(call rwildcard,$(OBJ_DIR)/,*.o)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oo)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oop) 
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
# some time on windows .O.asm and .o.asm will appear as same files,
# so $(sort) will eliminate duplication
ALL_OBJ_FILES := $(sort $(ALL_OBJ_FILES))

LINKER_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_OUTPUT))
OUTPUT_BIN :=$(call fix_path_if_in_windows,$(OUTPUT_BIN))
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_HISTORY_OUTPUT))
OUTPUT_HISTORY_BIN :=$(call fix_path_if_in_windows,$(OUTPUT_HISTORY_BIN))


build_outputs :
	$(LD) $(LDFLAGS) $(LIBRARIES_DIRS) $(ALL_OBJ_FILES) $(LIBS) -o $(LINKER_OUTPUT)
	$(DISASSEMBLER) $(LINKER_OUTPUT) > $(OUT_DIR)/$(OUTPUT_APP_NAME).asm
	$(CP)  $(LINKER_OUTPUT) $(OUTPUT_BIN)
	$(STRIP) $(OUTPUT_BIN)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
