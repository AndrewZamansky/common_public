### extracting u-boot commands routines
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    # assuming there is no string YrkHZOpQckoSO32i0tOM in processed file,
    # findstr /v YrkHZOpQckoSO32i0tOM will pipe file as is.
    # but it will solve "FINDSTR: Line NNNNNNN is too long." issue by
    # redirecting error to stdout
    get_cmd = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1)) \
                          | findstr /v YrkHZOpQckoSO32i0tOM 2>&1\
                          | findstr /C:"cmd_tbl_t\ _u_boot_list_2_cmd_2_"),)
else
    get_cmd = $(if $(1),$(shell \
             cat $(1) | grep "cmd_tbl_t _u_boot_list_2_cmd_2_"),)
endif


# following functions will extract _UBOOT_CMD_PREFIX_YYY word and replace it
# with: extern int  YYY(struct cmd_tbl_s *, int, int, char * const []);
EXT_CMD_PREFIX :=extern int #last space is importent
EXT_CMD_SUFIX :=(struct cmd_tbl_s *, int, int, char * const []);
get_cmd_functions_1 = $(filter _UBOOT_CMD_PREFIX_%, $(1))
get_cmd_functions = $(patsubst \
    _UBOOT_CMD_PREFIX_%,$(EXT_CMD_PREFIX)%$(EXT_CMD_SUFIX),\
    $(call get_cmd_functions_1, $(1)))

# some compilers (like armcc) doesn't replace ";;" by ";"
CLEAN_CMD_REM_TEXT4 :=};;
CLEAN_CMD_NEW_TEXT4 :=};
clean_cmd_4 =$(subst $(CLEAN_CMD_REM_TEXT4),$(CLEAN_CMD_NEW_TEXT4),$(1))
# the extracted line will have following structure :
# extern int _UBOOT_CMD_PREFIX_YYY ; static cmd_tbl_t _u_boot_list_2_cmd_2_XXX \
#__attribute__((unused)) = { "XXX", a, b, YYY, c, };
clean_cmd_3 =$(patsubst _UBOOT_CMD_PREFIX_%,,$(call clean_cmd_4,$(1)))
# after clean_cmd_3 the line will have following structure :
# extern int ; static cmd_tbl_t _u_boot_list_2_cmd_2_XXX \
#__attribute__((unused)) = { "XXX", a, b, YYY, c, };
clean_cmd_2 =$(patsubst _u_boot_list_2_cmd_2_%,,$(call clean_cmd_3,$(1)))
# after clean_cmd_2 the line will have following structure :
# extern int ; static \
# cmd_tbl_t __attribute__((unused)) = { "XXX", a, b, YYY, c, };
CLEAN_CMD_REM_TEXT1 := extern int ; static cmd_tbl_t __attribute__((unused)) =
clean_cmd_1 =$(subst $(CLEAN_CMD_REM_TEXT1),,$(call clean_cmd_2,$(1)))
# after clean_cmd_1 the line will have following structure :
#  { "XXX", a, b, YYY, c, };
CLEAN_CMD_REM_TEXT0 :=, };
CLEAN_CMD_NEW_TEXT0 :=, },
clean_cmd =$(subst \
      $(CLEAN_CMD_REM_TEXT0),$(CLEAN_CMD_NEW_TEXT0),$(call clean_cmd_1,$(1)))
# after clean_cmd the line will have following structure :
# { "XXX", a, b, YYY, c, },

create_cmd_file = $(if $(strip $(2)), \
          $(call mkdir_if_not_exists, $(dir $1)) \
          $(shell echo $2>$1) $(shell echo $3>$1.ext) ,)



### extracting auto_init structures
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    # assuming there is no string YrkHZOpQckoSO32i0tOM in processed file,
    # findstr /v YrkHZOpQckoSO32i0tOM will pipe file as is.
    # but it will solve "FINDSTR: Line NNNNNNN is too long." issue by
    # redirecting error to stdout
    get_auto_init = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1))\
              | findstr /v YrkHZOpQckoSO32i0tOM 2>&1\
              | findstr /B /C:"struct auto_init_struct_t\ auto_init_"),)
else
    get_auto_init = $(if $(1),$(shell \
             cat $(1) | grep "^auto_init_struct_t auto_init_"),)
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
CLEAN_AUTO_INITS_REM_TEXT1 :=struct = #last space is important
clean_auto_inits_1 =\
       $(subst $(CLEAN_AUTO_INITS_REM_TEXT1),,$(call clean_auto_inits_2,$(1)))
# after clean_auto_inits_1 the line will have following structure :
# { YYY};
CLEAN_AUTO_INITS_REM_TEXT0 :=};
CLEAN_AUTO_INITS_NEW_TEXT0 :=},
clean_auto_inits = $(subst  \
    $(CLEAN_AUTO_INITS_REM_TEXT0),$(CLEAN_AUTO_INITS_NEW_TEXT0),\
    $(call clean_auto_inits_1,$(1)))
# after clean_auto_inits_1 the line will have following structure :
# { YYY},

create_auto_init_file = $(if $(strip $(2)), \
          $(call mkdir_if_not_exists, $(dir $1)) \
          $(shell echo $2>$1) $(shell echo $3>$1.ext) ,)



# input :
# $1 - filename that under process.
# $2 - 1 word in format shell_frontend_bin_cmd_0x00_0xXX
# output:
#  will write "extern struct shell_bin_cmd_struct_t \
#  shell_frontend_bin_cmd_0x00_0xXX;"  into $1.bin_cmd.ext
EXTERN_CMD_PREFIX :=extern struct shell_bin_cmd_struct_t
bin_cmd_step5 = $(if $(strip $(2)),\
       $(shell echo $(EXTERN_CMD_PREFIX) $(2);>>$1.bin_cmd.ext),\
       do nothing)

# input :
# $1 - filename that under process.
# $2 - 1 word in format shell_frontend_bin_cmd_0x00_0xXX
# output:
#  will write "&shell_frontend_bin_cmd_0x00_0xXX,"  $1.bin_cmd
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    bin_cmd_step4 = $(if $(strip $(2)), $(shell echo ^&$2,>>$1.bin_cmd),)
else
    bin_cmd_step4 = $(if $(strip $(2)), $(shell echo &$2,>>$1.bin_cmd),)
endif

# input :
# $1 - filename that under process.
# $2 - words in format shell_frontend_bin_cmd_0x00_0xXX
# output:
# 1) will create folder if needed
# 2) truncate bin_cmd/bin_cmd.ext files and will call step4 and step5
bin_cmd_step3 = $(if $(strip $(2)), \
      $(call mkdir_if_not_exists, $(dir $1)) \
      $(shell echo /*from $1*/>$1.bin_cmd)\
      $(shell echo /*from $1*/>$1.bin_cmd.ext)\
      $(foreach var,\
         $2,$(call bin_cmd_step4,$1,$(var)) $(call bin_cmd_step5,$1,$(var)))\
      ,do nothing)

# input :
# $1 - filename that under process.
# $2 - string have following structure:
# struct shell_bin_cmd_struct_t shell_frontend_bin_cmd_0x00_0xXX = \
# { 0xXX, func};
# output:
# will extract shell_frontend_bin_cmd_0x00_0xXX and will call step3
bin_cmd_step2 = $(if $(strip $(2)),$(eval \
    __x = $(call bin_cmd_step3,$1,$(filter shell_frontend_bin_cmd_%,$2))),)

### extracting shell_bin_cmd_struct_t structures
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    # assuming there is no string YrkHZOpQckoSO32i0tOM in processed file,
    # findstr /v YrkHZOpQckoSO32i0tOM will pipe file as is.
    # but it will solve "FINDSTR: Line NNNNNNN is too long." issue by
    # redirecting error to stdout
    bin_cmd_step1 = $(if $(1),$(shell type $(call fix_path_if_in_windows,$(1))\
              | findstr /v YrkHZOpQckoSO32i0tOM 2>&1\
              | findstr /B /C:"struct shell_bin_cmd_struct_t shell_frontend"),)
else
    bin_cmd_step1 = $(if $(1),$(shell \
             cat $(1) | grep "^struct shell_bin_cmd_struct_t shell_frontend"),)
endif

create_shell_binary_commands_file =$(if $1,\
        $(eval BIN_CMDS=$(call bin_cmd_step1,$1))\
        $(eval $(call bin_cmd_step2,$1,$(BIN_CMDS))),\
        do nothing)

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
	$(call create_shell_binary_commands_file,$<)
