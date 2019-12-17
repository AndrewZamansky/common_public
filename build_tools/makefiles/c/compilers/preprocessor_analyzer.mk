### extracting u-boot commands routines
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    get_cmd = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1)) \
                          | findstr /C:"cmd_tbl_t\ _u_boot_list_2_cmd_2_"),)
else
    get_cmd = $(if $(1),$(shell \
             cat $(1) | grep "cmd_tbl_t\ _u_boot_list_2_cmd_2_"),)
endif


# following functions will extract _UBOOT_CMD_PREFIX_YYY word and replace it
# with: extern int  YYY(struct cmd_tbl_s *, int, int, char * const []);
EXT_CMD_PREFIX :=extern int #last space is importent
EXT_CMD_SUFIX :=(struct cmd_tbl_s *, int, int, char * const []);
get_cmd_functions_1 = $(filter _UBOOT_CMD_PREFIX_%, $(1))
get_cmd_functions = $(patsubst \
    _UBOOT_CMD_PREFIX_%,$(EXT_CMD_PREFIX)%$(EXT_CMD_SUFIX),\
    $(call get_cmd_functions_1, $(1)))

# the extracted line will have following structure :
# extern int _UBOOT_CMD_PREFIX_YYY ; cmd_tbl_t _u_boot_list_2_cmd_2_XXX \
# __attribute__((__aligned__(4))) \
#__attribute__((unused, section(".u_boot_list_2_""cmd""_2_""XXX"))) = \
# { "XXX", a, b, YYY, c, };
clean_cmd_6 =$(patsubst _UBOOT_CMD_PREFIX_%,,$(1))
# after clean_cmd_6 the line will have following structure :
# extern int  ; cmd_tbl_t _u_boot_list_2_cmd_2_XXX \
# __attribute__((__aligned__(4))) \
#__attribute__((unused, section(".u_boot_list_2_""cmd""_2_""XXX"))) = \
# { "XXX", a, b, YYY, c, };
clean_cmd_5 =$(patsubst _u_boot_list_2_cmd_2_%,,$(call clean_cmd_6,$(1)))
# after clean_cmd_5 the line will have following structure :
# extern int  ; cmd_tbl_t __attribute__((__aligned__(4))) \
#__attribute__((unused, section(".u_boot_list_2_""cmd""_2_""XXX"))) = \
# { "XXX", a, b, YYY, c, };
UNWANTED_TEXT4 :="))) = {
clean_cmd_4 =$(subst $(UNWANTED_TEXT4), {,$(call clean_cmd_5,$(1)))
# after clean_cmd_4 the line will have following structure :
# extern int  ; cmd_tbl_t __attribute__((__aligned__(4))) \
#__attribute__((unused, section(".u_boot_list_2_""cmd""_2_""XXX \
# { "XXX", a, b, YYY, c, };
UNWANTED_TEXT3 :=section(".u_boot_list_2_""cmd""_2_""
clean_cmd_3 =$(patsubst $(UNWANTED_TEXT3)%,,$(call clean_cmd_4,$(1)))
# after clean_cmd_3 the line will have following structure :
# extern int  ; cmd_tbl_t __attribute__((__aligned__(4))) \
#__attribute__((unused, { "XXX", a, b, YYY, c, };
UNWANTED_TEXT2 := \
   extern int ; cmd_tbl_t __attribute__((__aligned__(4))) __attribute__((unused,
clean_cmd_2 =$(subst $(UNWANTED_TEXT2),,$(call clean_cmd_3,$(1)))
# after clean_cmd_2 the line will have following structure :
# { "XXX", a, b, YYY, c, };
UNWANTED_TEXT0 :=, };
WANTED_TEXT0 :=, },
clean_cmd =$(subst  $(UNWANTED_TEXT0),$(WANTED_TEXT0),$(call clean_cmd_2,$(1)))
# after clean_cmd the line will have following structure :
# { "XXX", a, b, YYY, c, },

create_cmd_file = $(if $(strip $(2)), \
          $(call mkdir_if_not_exists, $(dir $1)) \
          $(shell echo $2>$1) $(shell echo $3>$1.ext) ,)



### extracting auto_init structures
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    get_auto_init = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1)) \
                      | findstr /B /C:"struct auto_init_struct_t\ auto_init_"),)
else
    get_auto_init = $(eval $(error TODO in linux)) $(if $(1),$(shell \
             cat $(1) | grep "auto_init_struct_t\ auto_init_"),)
endif

# following function will extract YYY}; from '{ YYY};' and replace it
# with: extern void YYY(void);
EXT_AUTO_FUNC_PREFIX :=extern void #last space is importent
EXT_AUTO_FUNC_SUFIX :=(void);
get_auto_init_functions = $(patsubst  \
       %};,$(EXT_AUTO_FUNC_PREFIX)%$(EXT_AUTO_FUNC_SUFIX), $(filter %};, $(1)))

# the extracted line will have following structure :
# struct auto_init_struct_t auto_init_XXX = { YYY};
clean_auto_inits_2 =$(patsubst auto_init_%,,$(1))
# after clean_auto_inits_2 the line will have following structure :
# struct = { YYY};
UNWANTED_TEXT1 :=struct = #last space is important
clean_auto_inits_1 =$(subst $(UNWANTED_TEXT1),,$(call clean_auto_inits_2,$(1)))
# after clean_auto_inits_1 the line will have following structure :
# { YYY};
UNWANTED_TEXT0 :=};
WANTED_TEXT0 :=},
clean_auto_inits =\
     $(subst  $(UNWANTED_TEXT0),$(WANTED_TEXT0),$(call clean_auto_inits_1,$(1)))
# after clean_auto_inits_1 the line will have following structure :
# { YYY},

create_auto_init_file = $(if $(strip $(2)), \
          $(call mkdir_if_not_exists, $(dir $1)) \
          $(shell echo $2>$1) $(shell echo $3>$1.ext) ,)

$(CURR_OBJ_DIR)/%.analyzer: $(CURR_OBJ_DIR)/%.preproc %
	$(info .    ------- analyzing $<)
	$(eval CMDS:=$(call get_cmd,$<))
	$(eval EXT_CMDS:=$(call get_cmd_functions,$(CMDS)))
	$(eval CMDS:=$(call clean_cmd,$(CMDS)))
	$(call create_cmd_file,$<.cmd,$(CMDS),$(EXT_CMDS))
	$(eval AUTO_INITS:=$(call get_auto_init,$<))
	$(eval EXT_AUTO_INITS:=$(call get_auto_init_functions,$(AUTO_INITS)))
	$(eval AUTO_INITS:=$(call clean_auto_inits,$(AUTO_INITS)))
	$(call create_auto_init_file,$<.auto_init,$(AUTO_INITS),$(EXT_AUTO_INITS))	