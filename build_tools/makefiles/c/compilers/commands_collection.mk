ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    get_cmd = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1)) \
                          | findstr "cmd_tbl_t\ _u_boot_list_2_cmd_2_"),)
else
    get_cmd = $(if $(1),$(shell \
             cat $(1) | grep "cmd_tbl_t\ _u_boot_list_2_cmd_2_"),)
endif


EXT_CMD_PREFIX :=extern int #last space is importent
EXT_CMD_SUFIX :=(struct cmd_tbl_s *, int, int, char * const []);
get_cmd_functions_1 = $(filter _UBOOT_CMD_PREFIX_%, $(1))
get_cmd_functions = $(patsubst \
    _UBOOT_CMD_PREFIX_%,$(EXT_CMD_PREFIX)%$(EXT_CMD_SUFIX),\
    $(call get_cmd_functions_1, $(1)))

# clean_cmd_x will remove everthing except command {a, b, c ..} structure
clean_cmd_6 =$(patsubst _UBOOT_CMD_PREFIX_%,,$(1))
clean_cmd_5 =$(patsubst _u_boot_list_2_cmd_2_%,,$(call clean_cmd_6,$(1)))
UNWANTED_TEXT4 :="))) = {
clean_cmd_4 =$(subst $(UNWANTED_TEXT4), {,$(call clean_cmd_5,$(1)))
UNWANTED_TEXT3 :=section(".u_boot_list_2_""cmd""_2_""
clean_cmd_3 =$(patsubst $(UNWANTED_TEXT3)%,,$(call clean_cmd_4,$(1)))
UNWANTED_TEXT2 := \
   extern int ; cmd_tbl_t __attribute__((__aligned__(4))) __attribute__((unused,
clean_cmd_2 =$(subst $(UNWANTED_TEXT2),,$(call clean_cmd_3,$(1)))
UNWANTED_TEXT0 :=, };
WANTED_TEXT0 :=, },
clean_cmd =$(subst  $(UNWANTED_TEXT0),$(WANTED_TEXT0),$(call clean_cmd_2,$(1)))
create_cmd_file = $(if $(strip $(2)), \
          $(call mkdir_if_not_exists, $(dir $1)) \
          $(shell echo $2>$1) $(shell echo $3>$1.ext) ,)


$(CURR_OBJ_DIR)/cmds/%.cmd: $(CURR_OBJ_DIR)/%.preproc %.c
	$(info .    ------- $@)
	$(eval CMDS:=$(call get_cmd,$<))
	$(info ++++++++ $(CMDS))
	$(eval EXT_CMDS:=$(call get_cmd_functions,$(CMDS)))
	$(info ++++2++++ $(EXT_CMDS))
	$(eval CMDS:=$(call clean_cmd,$(CMDS)))
	$(info ++++1++++ $(CMDS))
	$(call create_cmd_file,$@,$(CMDS),$(EXT_CMDS))
	$(info ++++0++++ $(CMDS))
