
#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

ifdef CONFIG_HEXAGON_VERSION_60
    LDFLAGS += -mv60 
else
    $(error unknown hexagon version)
endif

LDFLAGS += -O0 
LDFLAGS += -mG0lib 
LDFLAGS += -G0 
LDFLAGS += -v 
LDFLAGS += -fpic 
LDFLAGS += -shared 
LDFLAGS += -Wl,-Bsymbolic
LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=free 
LDFLAGS += -Wl,--wrap=realloc -Wl,--wrap=memalign -Wl,--wrap=__stack_chk_fail
LDFLAGS += -Wl,-soname=$(OUTPUT_APP_NAME).so
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)

#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    STD_LIBRARIES := libc.a
    STD_LIBRARIES +=libm.a libgcc.a
    GLOBAL_LIBS += $(STD_LIBRARIES)
    ifdef CONFIG_HEXAGON_VERSION_60
        GLOBAL_LIBS_PATH += $(HEXAGON_ROOT_DIR)/target/hexagon/lib/v60/G0/pic
    endif
endif
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




#{{{{{{{{  CREATING OBJECT LIST {{{{{{{{

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_OBJ_FILES = $(call rwildcard,$(OBJ_DIR)/,*.o)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oo)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
#some time, on windows, scan for .O.asm and .o.asm will generate duplicate files
#so $(sort) will eliminate duplication
ALL_OBJ_FILES :=$(sort $(ALL_OBJ_FILES))
ALL_OBJ_FILES :=$(subst \,/,$(ALL_OBJ_FILES))

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file_in_one_line.mk
#end of file creation

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}




#{{{{{{{{  DEFINING OUTPUTS {{{{{{{{

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME).so
#OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).asm

HISTORY_OUT_PREFIX :=$(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)
LINKER_HISTORY_OUTPUT := $(HISTORY_OUT_PREFIX).so

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
    OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif

ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif

#}}}}}}}}  END OF DEFINING OUTPUTS  }}}}}}}}



LINKER_CMD =$(LD) $(LDFLAGS) $(LIBRARIES_DIRS)
LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(LIBS) -o $(LINKER_OUTPUT)

#	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
