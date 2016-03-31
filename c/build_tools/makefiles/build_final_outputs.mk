
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
else ifdef CONFIG_STM8
    include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_ld.mk
else ifdef CONFIG_HOST
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_host_ld.mk
else
    $(error ---- unknown compiler ----)
endif

