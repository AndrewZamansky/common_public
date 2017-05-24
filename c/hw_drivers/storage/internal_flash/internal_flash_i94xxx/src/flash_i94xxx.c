/*
 *
 *   file  :  flash_i94xxx.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "internal_flash_i94xxx_api.h"
#include "ISD94XXXSeries.h"

#include "fmc.h"

#include "flash_wrapper_api.h"

#include "_flash_i94xxx_prerequirements_check.h"

#include "internal_flash_i94xxx_add_component.h"

/***************   defines    *******************/
#define ERASE_BLOCK_SIZE   0x1000    /* 4k */
#define DATA_FLASH_BASE    0x40000    /* Data Flash  start address */
#define DATA_FLASH_SIZE   (FMC_APROM_END - DATA_FLASH_BASE)

/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/



static uint32_t get_actual_read_write_len( uint32_t aLength, uint32_t aOffset)
{
	if (DATA_FLASH_SIZE <= aOffset)
	{
		return 0;
	}
	if (DATA_FLASH_SIZE <= (aOffset + aLength))
	{
		aLength = DATA_FLASH_SIZE - aOffset;
	}
	return aLength;
}


/**
 * internal_flash_i94xxx_pwrite()
 *
 * return:
 */
size_t internal_flash_i94xxx_pwrite(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset)
{
	size_t written_len;
    uint32_t wrAddr;
    uint32_t write_u32;
    uint32_t word_offset;
	uint8_t  copy_size;

    aLength = get_actual_read_write_len(aLength, aOffset);
    written_len = aLength;

	wrAddr = aOffset + DATA_FLASH_BASE;

	FMC_ENABLE_AP_UPDATE();

	word_offset = wrAddr & 0xf;
	wrAddr = (wrAddr & (~0xf));
	if (0 != word_offset)
	{
		uint8_t *u32_addr;

		copy_size = 4 - word_offset;
		if (copy_size > aLength)
		{
			copy_size = aLength;
		}
		write_u32 = FMC_Read(wrAddr);
		u32_addr = (uint8_t*)(&write_u32);
    	memcpy(&u32_addr[word_offset], apData, copy_size);
        FMC_Write(wrAddr, write_u32);
		aLength -= copy_size;
		apData += copy_size;
		wrAddr += 4;
	}

	while (4 <= aLength)
    {
    	memcpy(&write_u32, apData, 4);
        FMC_Write(wrAddr, write_u32);
        aLength -= 4;
        apData +=4;
        wrAddr +=4;
    }

	if (aLength)
	{
		write_u32 = FMC_Read(wrAddr);
		memcpy(&write_u32, apData, aLength);
        FMC_Write(wrAddr, write_u32);
	}
	FMC_DISABLE_AP_UPDATE();

	return written_len;
}


/**
 * internal_flash_i94xxx_pread()
 *
 * return:
 */
size_t internal_flash_i94xxx_pread(struct dev_desc_t *adev,
					uint8_t *apData, size_t aLength, size_t aOffset)
{
	size_t read_len;
    uint32_t readAddr;
    uint32_t read_u32;
    uint32_t word_offset;
	uint8_t  copy_size;

	// test alignment
    aLength = get_actual_read_write_len(aLength, aOffset);
    read_len = aLength;

    readAddr = aOffset + DATA_FLASH_BASE;

	word_offset = readAddr & 0xf;
	readAddr = (readAddr & (~0xf));
	if (0 != word_offset)
	{
		uint8_t *u32_addr;
		copy_size = 4 - word_offset;
		if (copy_size > aLength)
		{
			copy_size = aLength;
		}
		read_u32 = FMC_Read(readAddr);
		u32_addr = (uint8_t*)(&read_u32);
		memcpy(apData, &u32_addr[word_offset], copy_size);
		aLength -= copy_size;
		apData += copy_size;
		readAddr += 4;
	}

	while (4 <= aLength)
	{
		read_u32 = FMC_Read(readAddr);
		memcpy(apData, &read_u32, 4);

		apData += 4;
		readAddr += 4;
		aLength -= 4;
	}

	if (aLength)
	{
		read_u32 = FMC_Read(readAddr);
		memcpy(apData, &read_u32, aLength);
	}

    return read_len;
}

#if 0
static int  set_data_flash_base(uint32_t u32DFBA)
{
    uint32_t   au32Config[2];          /* User Configuration */

    /* Read User Configuration 0 & 1 */
    if (FMC_ReadConfig(au32Config, 2) < 0) {
        printf("\nRead User Config failed!\n");       /* Error message */
        return -1;                     /* failed to read User Configuration */
    }

    /* Check if Data Flash is enabled and is expected address. */
    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
        return 0;              /* no need to modify User Configuration */

    FMC_ENABLE_CFG_UPDATE();           /* Enable User Configuration update. */

    au32Config[0] &= ~0x1;       /* Clear CONFIG0 bit 0 to enable Data Flash */
    au32Config[1] = u32DFBA;     /* Give Data Flash base address  */

    /* Update User Configuration settings. */
    if (FMC_WriteConfig(au32Config, 2) < 0)
        return -1;                     /* failed to write user configuration */



    /* Perform chip reset to make new User Config take effect. */
    SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
    return 0;                          /* success */
}
#endif


/**
 * internal_flash_i94xxx_ioctl()
 *
 * return:
 */
uint8_t internal_flash_i94xxx_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
//	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
//	struct dev_desc_t	*src_clock;

//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
//	src_clock = cfg_hndl->src_clock;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		FMC_Open();

		/* following can be removed after adding erase of config page and
		 * writing 0x5A5A into CONFIG2 after erasing
		 */
		//set_data_flash_base(DATA_FLASH_BASE);
		break;
	case IOCTL_FLASH_WRAPPER_GET_ERASE_SIZE :
		*(uint32_t*)aIoctl_param1 = ERASE_BLOCK_SIZE;
		break;
	case IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE :
		*(uint32_t*)aIoctl_param1 = DATA_FLASH_SIZE;
		break;
	case IOCTL_FLASH_WRAPPER_ERASE :
		{
		    uint32_t errAddr;

		    errAddr = (*(uint32_t*)aIoctl_param1);
		    if (DATA_FLASH_SIZE <= errAddr)
		    {
		    	return 1;
		    }
		    errAddr += DATA_FLASH_BASE;
			FMC_ENABLE_AP_UPDATE();
			FMC_Erase(errAddr);
			FMC_DISABLE_AP_UPDATE();
		}
		break;
	default :
		return 1;
	}
	return 0;
}
