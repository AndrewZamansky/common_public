include $(MAKEFILES_ROOT_DIR)/common.mk

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32


ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(COMPILERS_DIR)/gcc/gcc_arm_ld.mk
    else ifdef CONFIG_GPP
        include $(COMPILERS_DIR)/gcc/gcc_arm_ld.mk
    else ifdef CONFIG_ARMCC
        include $(COMPILERS_DIR)/armcc/armcc_ld.mk
    endif
else ifdef CONFIG_AVR
    include $(COMPILERS_DIR)/gcc/gcc_avr_ld.mk
else ifdef CONFIG_XTENSA_GCC
    include $(COMPILERS_DIR)/gcc/gcc_xtensa_ld.mk
else ifdef CONFIG_XTENSA_XCC
    include $(COMPILERS_DIR)/xcc/xcc_xtensa_ld.mk
else ifdef CONFIG_PIC32
    include $(COMPILERS_DIR)/gcc/gcc_pic32_ld.mk
else ifdef CONFIG_HEXAGON
    include $(COMPILERS_DIR)/clang/clang_hexagon_ld.mk
else ifdef CONFIG_ANDROID_NDK
    include $(COMPILERS_DIR)/gcc/gcc_android_ld.mk
else ifdef CONFIG_STM8
    include $(COMPILERS_DIR)/cxstm8/cxstm8_ld.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(COMPILERS_DIR)/gcc/gcc_host_ld.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(COMPILERS_DIR)/microsoft_compiler/msvc_host_ld.mk
        endif
    else
        include $(COMPILERS_DIR)/gcc/gcc_host_ld.mk
    endif
else
    $(error ---- unknown compiler ----)
endif

