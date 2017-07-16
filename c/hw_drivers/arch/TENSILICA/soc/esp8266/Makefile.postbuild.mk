$(info ----- aditional post build actions -----)

PYTHON_ROOT_DIR :=

####### test for existence of python and  #####
####### put its directory name in PYTHON_ROOT_DIR     #####
SEARCHED_TOOL :=python
SEARCHED_DIR_VARIABLE :=PYTHON_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_PYTHON_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=python.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=python
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

BIN_GEN_ORIG_SCRIPT :=$(SDK_ESP8266_PATH)\tools\gen_appbin.py
FLASH_BIN_FILE :=$(OUT_DIR)/eagle.app.flash.bin

COMPILER_PATH :=$(GCC_ROOT_DIR)/bin
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    COMPILER_PATH := $(subst /,\,$(COMPILER_PATH))
    SDK_ESP8266_PATH := $(subst /,\,$(SDK_ESP8266_PATH))
    BIN_GEN_ORIG_SCRIPT := $(subst /,\,$(BIN_GEN_ORIG_SCRIPT))
    FLASH_BIN_FILE := $(subst /,\,$(FLASH_BIN_FILE))
    CD := cd /D 
    RM := del /Q
endif

BUILD_FLASH_IMAGE :=set PATH=%PATH%;$(COMPILER_PATH)&
BUILD_FLASH_IMAGE += set COMPILE=gcc&
BUILD_FLASH_IMAGE += $(CP) $(BIN_GEN_ORIG_SCRIPT) $(OUT_DIR)&
BUILD_FLASH_IMAGE += $(CD) $(OUT_DIR)&
ifdef CONFIG_SDK_NONOS
    BUILD_FLASH_IMAGE += gen_appbin.py $(LINKER_OUTPUT) 0 0 0 0 0
else ifdef CONFIG_SDK_RTOS
    BUILD_FLASH_IMAGE += gen_appbin.py $(LINKER_OUTPUT) 0 0 0 0
else
    $(info !--- unknown SDK )
    $(error )
endif


build_outputs:
	$(FULL_GCC_PREFIX)objcopy --only-section .text -O binary $(LINKER_OUTPUT) $(OUT_DIR)/eagle.app.v6.text.bin
	$(FULL_GCC_PREFIX)objcopy --only-section .data -O binary $(LINKER_OUTPUT) $(OUT_DIR)/eagle.app.v6.data.bin
	$(FULL_GCC_PREFIX)objcopy --only-section .rodata -O binary $(LINKER_OUTPUT) $(OUT_DIR)/eagle.app.v6.rodata.bin
	$(FULL_GCC_PREFIX)objcopy --only-section .irom0.text -O binary $(LINKER_OUTPUT) $(OUT_DIR)/eagle.app.v6.irom0text.bin
	-$(RM) $(FLASH_BIN_FILE)
	$(BUILD_FLASH_IMAGE)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
	@echo ---- INFO : rm error message CAN BE IGNORED IN WINDOWS
endif
