;  

LR_ROM      FLASH_START_ADDR  0x00200000  ; 2MB (SPI FLash)
{
	ER_STARTUP +0
    {
        vectors_armcc.o.asm(RESET, +First)
	}
    
    ER_RO +0
    {
		*(+RO)
	}

    ; Relocate vector table in SRAM for fast interrupt handling.
    ER_VECTOR2  RAM_START_ADDR  EMPTY   0x00000100
    {
    }

    ER_RW  +0 (0x00010000 - ImageLimit(ER_VECTOR2))
	{
		*(+RW +ZI)
	}
	
	
}

