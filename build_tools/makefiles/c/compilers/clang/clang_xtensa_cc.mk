format_all_include_dir = $(patsubst %,-I%,$(INCLUDE_DIR) $(GLOBAL_INCLUDE_DIR))
format_all_asm_include_dir = $(call format_all_include_dir)

# substitute " to \" for string defines
format_all_defines = \
              $(subst ",\",$(patsubst %,-D%,$(DEFINES) $(GLOBAL_DEFINES)))
format_all_asm_defines = $(call format_all_defines)


# $(1) - if command is too long it contain the name of command file
# $(2) - command to run (as or gcc or g++ or ...) with all flags
# $(3) - all defines and includes
# $(4) - input file
# $(5) - output file
# if command is too long, use command file(true of 'if')
define run_compiler
   $(if $(1),\
     $(2) @$(1) -o $(5) $(4),\
     $(2) $(3) -o $(5) $(4))
endef

# for next 5 functions:
# $(1) - if command is too long it contain the name of command file
# $(2) - input file
# $(3) - output file
run_asm_compiler =  $(call run_compiler,$(1),\
        $(ASM) $(ALL_ASM_FLAGS),$(ALL_ASM_DEFS_AND_INCLUDES),$(2),$(3))

run_c_preprocessor = $(call run_compiler,$(1),\
        $(CC) -E -dD $(ALL_CFLAGS),$(ALL_DEFS_AND_INCLUDES),$(2),$(3))

run_c_compiler =  $(call run_compiler,$(1),\
        $(CC) $(ALL_CFLAGS),$(ALL_DEFS_AND_INCLUDES),$(2),$(3))

run_cpp_preprocessor = $(call run_compiler,$(1),\
        $(CCPP) -E -dD $(ALL_CPPFLAGS),$(ALL_DEFS_AND_INCLUDES),$(2),$(3))

run_cpp_compiler = $(call run_compiler,$(1),\
        $(CCPP) $(ALL_CPPFLAGS),$(ALL_DEFS_AND_INCLUDES),$(2),$(3))
