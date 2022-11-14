$(info ----- aditional post build actions -----)

SIM_RUN_CMD :=$(XCC_ROOT_DIR)/bin/xt-run --xtensa-core=$(XCC_CORE)
SIM_RUN_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
SIM_RUN_CMD +=  --xtensa-params= --console $(LINKER_OUTPUT)

$(call exit)

build_outputs:
	@echo run following command for simulations :
	@echo $(SIM_RUN_CMD)
