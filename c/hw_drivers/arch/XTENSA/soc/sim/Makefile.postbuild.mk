$(info ----- aditional post build actions -----)

SIM_RUN_CMD :=$(XCC_ROOT_DIR)/bin/xt-run --xtensa-core=hifi3_bd5
SIM_RUN_CMD += --xtensa-system=$(REDEFINE_XTENSA_CONFIGS_DIR)/hifi3_bd5/config
SIM_RUN_CMD +=  --xtensa-params= --console $(LINKER_OUTPUT)

build_outputs:
	@echo run following command for simulations
	@echo $(SIM_RUN_CMD)
	