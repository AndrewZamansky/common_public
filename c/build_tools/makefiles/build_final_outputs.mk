
include $(MAKEFILE_DEFS_ROOT_DIR)/common_init.mk

include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32



ifeq ($(findstring ARM-NONE-EABI-GCC,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-GCC) 	 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_ld.mk
else ifeq ($(findstring ARM-NONE-EABI-G++,$(CONFIG_USE_COMPILER)),ARM-NONE-EABI-G++) 	 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_ld.mk
else ifeq ($(findstring ARMCC,$(CONFIG_USE_COMPILER)),ARMCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/armcc/armcc_ld.mk
else ifeq ($(findstring CXSTM8,$(CONFIG_USE_COMPILER)),CXSTM8) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_ld.mk
else ifeq ($(findstring AVR-GCC,$(CONFIG_USE_COMPILER)),AVR-GCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_avr_ld.mk
else ifeq ($(findstring HOST-GCC,$(CONFIG_USE_COMPILER)),HOST-GCC) 
	include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_ld.mk
endif




