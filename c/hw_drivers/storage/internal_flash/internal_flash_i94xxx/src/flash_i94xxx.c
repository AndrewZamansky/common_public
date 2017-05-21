/*
 *
 *   file  :  flash_i94xxx.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "internal_flash_i94xxx_api.h"
#include "ISD94XXXSeries.h"

#include "fmc.h"


/***************   defines    *******************/
#define DATA_FLASH_BASE    0x40000    /* Data Flash  start address */
#define DATA_FLASH_SIZE   (FMC_APROM_END - DATA_FLASH_BASE)

/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/

static uint8_t wr_data[4*1024];


static size_t read_buff(uint8_t *buff,
		size_t aLength, size_t aOffsetFromAbsoluteStart)
{
	size_t actual_size_to_read;
	size_t max_size_to_read;
	size_t left_to_read;
	size_t rdAddr;

    if (FMC_APROM_END <= aOffsetFromAbsoluteStart)
    {
    	return 0;
    }
	actual_size_to_read = aLength;
	max_size_to_read = FMC_APROM_END - aOffsetFromAbsoluteStart;
	if (max_size_to_read < actual_size_to_read)
	{
		actual_size_to_read = max_size_to_read;
	}

	rdAddr = aOffsetFromAbsoluteStart;
	left_to_read = actual_size_to_read;
	while (left_to_read)
	{
		uint32_t read_data;
		uint8_t  copy_size;

		read_data = FMC_Read(rdAddr);
		if (4 <= left_to_read)
		{
			copy_size = 4;
		}
		else
		{
			copy_size = left_to_read;
		}
		memcpy(buff, &read_data, copy_size);

		buff += copy_size;
		rdAddr += copy_size;
		left_to_read -= copy_size;
	}
	return actual_size_to_read;
}


/**
 * internal_flash_i94xxx_pwrite()
 *
 * return:
 */
size_t internal_flash_i94xxx_pwrite(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset)
{
	size_t actual_size_to_write;
    uint32_t pageAddr;
    uint32_t wrAddr;
    uint32_t retVal;
    size_t	i;

    if (DATA_FLASH_SIZE <= aOffset)
    {
    	return 0;
    }
	actual_size_to_write = aLength;
	if (DATA_FLASH_SIZE < actual_size_to_write)
	{
		actual_size_to_write = DATA_FLASH_SIZE;
	}
	retVal = actual_size_to_write;
	wrAddr = aOffset + DATA_FLASH_BASE;

	FMC_ENABLE_AP_UPDATE();
    while (actual_size_to_write)
    {
    	size_t wrSize;
    	size_t offsetInPage;

    	pageAddr = wrAddr & (~0xfff);
    	offsetInPage = wrAddr & 0xfff;
    	read_buff(wr_data, 0x1000, pageAddr);

    	if (0 != offsetInPage)
    	{
    		wrSize = 0x1000 - offsetInPage;
    	}
    	else
    	{
			wrSize = actual_size_to_write;
			if (0x1000 < wrSize)
			{
				wrSize = 0x1000;
			}
    	}

    	memcpy(wr_data + offsetInPage, apData, wrSize);
        FMC_Erase(pageAddr);     /* Erase page */
        for (i = 0; i < 0x1000; i += 4)
        {
        	uint32_t write_u32;

        	memcpy(&write_u32, &wr_data[i], 4);
            FMC_Write(pageAddr, write_u32);
            pageAddr += 4;
        }

        actual_size_to_write -= wrSize;
        apData += wrSize;
        wrAddr += wrSize;
    }
	FMC_DISABLE_AP_UPDATE();

	return retVal;
}


/**
 * internal_flash_i94xxx_pread()
 *
 * return:
 */
size_t internal_flash_i94xxx_pread(struct dev_desc_t *adev,
					uint8_t *apData, size_t aLength, size_t aOffset)
{
    if (DATA_FLASH_SIZE <= aOffset)
    {
    	return 0;
    }
    return read_buff(apData, aLength, aOffset + DATA_FLASH_BASE);
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

	default :
		return 1;
	}
	return 0;
}
