
include $(MAKEFILE_DEFS_ROOT_DIR)/common_init.mk

include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32


ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_ld.mk
    else ifdef CONFIG_GPP
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_arm_ld.mk
    else ifdef CONFIG_ARMCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/armcc/armcc_ld.mk
    endif
else ifdef CONFIG_AVR
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_avr_ld.mk
else ifdef CONFIG_PIC32
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_pic32_ld.mk
else ifdef CONFIG_STM8
    include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_ld.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_ld.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(MAKEFILE_DEFS_ROOT_DIR)/microsoft_compiler/mcc_host_ld.mk
        endif
    else
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_ld.mk
    endif
else
    $(error ---- unknown compiler ----)
endif

