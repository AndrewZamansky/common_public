
MEMORY {
#ifdef CONFIG_INTERNAL_FLASH
	internal_flash	:   ORIGIN = CONFIG_DT_SOC_FLASH_START_ADDR, 	LENGTH = CONFIG_DT_SOC_FLASH_SIZE
#endif
#ifdef CONFIG_INTERNAL_SRAM
	internal_sram  	:   ORIGIN = CONFIG_DT_SOC_RAM_START_ADDR ,   	LENGTH =  CONFIG_DT_SOC_RAM_SIZE
#endif
#ifdef CONFIG_DDR
	ddr  	:   ORIGIN = CONFIG_DDR_START_ADDR ,   	LENGTH =  CONFIG_DDR_SIZE
#endif
    /* The following section is for checking illegal sections */
    dummy_area    :   ORIGIN = 0xFFFFFFFFFFFFFFFF,           LENGTH = 0
}
