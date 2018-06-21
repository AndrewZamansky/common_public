#################################################################
### the following section we need to run just one time per build


COMPILERS_DIR :=$(MAKEFILES_ROOT_DIR)/c/compilers
ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_GPP
        include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_ARMCC
        include $(COMPILERS_DIR)/armcc/armcc_init.mk
    endif
else ifdef CONFIG_PIC32
    include $(COMPILERS_DIR)/gcc/gcc_pic32_init.mk
else ifdef CONFIG_AVR
    include $(COMPILERS_DIR)/gcc/gcc_avr_init.mk
else ifdef CONFIG_XTENSA_GCC
    include $(COMPILERS_DIR)/gcc/gcc_xtensa_init.mk
else ifdef CONFIG_XTENSA_XCC
    include $(COMPILERS_DIR)/xcc/xcc_xtensa_init.mk
else ifdef CONFIG_STM8
    include $(COMPILERS_DIR)/cxstm8/cxstm8_init.mk
else ifdef CONFIG_HEXAGON
    include $(COMPILERS_DIR)/clang/clang_hexagon_init.mk
else ifdef CONFIG_ANDROID_NDK
    include $(COMPILERS_DIR)/gcc/gcc_android_init.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(COMPILERS_DIR)/microsoft_compiler/msvc_host_init.mk
        endif
    else
        include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
    endif
else
    $(info err: unknown compiler)
    $(call exit,1)
endif


#########################################################
#####   components configuration includes folder :  #####  
GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/project_config_includes
GLOBAL_INCLUDE_DIR += $(AUTO_GENERATED_FILES_DIR)

### end of section that run just one time per build
#####################################################
