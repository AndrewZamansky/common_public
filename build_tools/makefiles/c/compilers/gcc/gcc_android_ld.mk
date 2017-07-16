ifdef CONFIG_REDEFINED_OUTPUT_NAME
    CONFIG_REDEFINED_OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))
    OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)
    HISTORY_OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)_$(MAIN_VERSION_STR).so
else
    ifdef CONFIG_GCC_OUTPUT_TYPE_DYNAMIC_LIBRARY
        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
    else ifdef CONFIG_GCC_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME)
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
    else
        $(error ---- unknown output type ----)
    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
#OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif

#}}}}}}}}  END OF DEFINING OUTPUTS  }}}}}}}}


#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=


LDFLAGS += -O0 
LDFLAGS += $(GLOBAL_LDFLAGS)

ifdef CONFIG_ANDROID_NDK_ARCH_ARM
    GCC_ARM_ARCH :=armv7-a
else
    $(error UNIMPLEMENTED ARCH)
endif
LDFLAGS += -march=$(GCC_ARM_ARCH)

LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map

LDFLAGS += -mthumb
LDFLAGS += -fpie 
LDFLAGS += -fPIE 
LDFLAGS += -fPIC 
LDFLAGS += -pie 
LDFLAGS += -Wl,-unresolved-symbols=ignore-in-shared-libs
LDFLAGS += -nostdlib

ifdef CONFIG_GCC_OUTPUT_TYPE_APPLICATION
    LDFLAGS += -Bdynamic
    LDFLAGS += -Wl,-llog
    LDFLAGS += -Wl,-ldl
endif

LDFLAGS_SO += -nostartfiles
LDFLAGS_SO += -shared
LDFLAGS_SO += -Bsymbolc

#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    STD_LIBRARIES := libc.a
    STD_LIBRARIES +=libm.a libgcc.a libstdc++
    GLOBAL_LIBS += $(STD_LIBRARIES)
    GLOBAL_LIBS_PATH += $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/lib
endif
GLOBAL_LIBS :=$(sort $(GLOBAL_LIBS)) #remove duplicates
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

GLOBAL_LIBS_PATH :=$(sort $(GLOBAL_LIBS_PATH)) #remove duplicates
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





ifdef CONFIG_GCC_OUTPUT_TYPE_APPLICATION
    
    LINKER_CMD =$(LD) $(LDFLAGS)  $(LIBRARIES_DIRS) 
    #LINKER_CMD += -Wl,--start-group -Wl,--whole-archive
    LINKER_CMD += $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/lib/crtbegin_dynamic.o
    LINKER_CMD += $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/lib/crtend_android.o
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(GLOBAL_ARCHIVES) $(LIBS) -o $(LINKER_OUTPUT)
#    LINKER_CMD += $(LIBS) -Wl,--no-whole-archive -Wl,--end-group
#    LINKER_CMD += -Wl,--start-group
#    LINKER_CMD += -Wl,--dynamic-linker=
#    LINKER_CMD += -Wl,-E
#    LINKER_CMD += -Wl,--force-dynamic
#    LINKER_CMD += $(APQ8096_SRC_DIR)/common/rtld/ship/hexagon_Debug_dynamic/rtld.a
#    LINKER_CMD += $(APQ8096_SRC_DIR)/common/a1std/ship/hexagon_Debug_dynamic/a1std.a
#    LINKER_CMD += $(APQ8096_SRC_DIR)/audio/voice_imc_utils/ship/hexagon_Debug_dynamic/voice_imc_utils.a
#    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) -Wl,--end-group -o $(LINKER_OUTPUT)

else ifdef CONFIG_GCC_OUTPUT_TYPE_DYNAMIC_LIBRARY 
   
    LINKER_CMD =$(LD) $(LDFLAGS) $(LDFLAGS_SO) $(LIBRARIES_DIRS)  $(GLOBAL_ARCHIVES)
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(LIBS) -o $(LINKER_OUTPUT)
 
endif



#	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
	