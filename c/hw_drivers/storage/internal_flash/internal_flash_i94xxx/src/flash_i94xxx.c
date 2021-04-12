/*
 *
 *   file  :  flash_i94xxx.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "string.h"

#include "dev_management_api.h"

#include "internal_flash_i94xxx_api.h"
#include "I94100.h"

#include "fmc.h"
#include "string.h"

#include "flash_wrapper_api.h"
#include "dt_memory_layout.h"


#define ERASE_BLOCK_SIZE   0x1000    /* 4k */

extern uint32_t __end_of_firmware_on_ROM__;

static uint8_t init_done = 0;

static uint32_t get_actual_read_write_len( uint32_t aLength, uint32_t aOffset)
{
	uint32_t max_size_to_rw;
	if (DT_INTERNAL_FLASH_MEMORY_SIZE <= aOffset)
	{
		return 0;
	}
	max_size_to_rw = DT_INTERNAL_FLASH_MEMORY_SIZE - aOffset;
	if (aLength > max_size_to_rw)
	{
		aLength = max_size_to_rw;
	}
	return aLength;
}


/**
 * internal_flash_i94xxx_pwrite()
 *
 * return:
 */
static size_t internal_flash_i94xxx_pwrite(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset)
{
	size_t written_len;
	uint32_t wrAddr;
	uint32_t write_u32;
	uint32_t word_offset;

	if (0 == init_done) return 0;

	aLength = get_actual_read_write_len(aLength, aOffset);
	written_len = aLength;

	wrAddr = aOffset + DT_INTERNAL_FLASH_BASE_ADDR;
	if (wrAddr < (size_t)&__end_of_firmware_on_ROM__)
	{
		return 0;
	}

	FMC_ENABLE_AP_UPDATE();

	word_offset = wrAddr & 0x3;// read/write are in chunks of 4 bytes
	wrAddr = (wrAddr & (~0x3));
	if (0 != word_offset)
	{
		uint8_t *u32_addr;
		uint8_t  copy_size;

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
		apData += 4;
		wrAddr += 4;
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
static size_t internal_flash_i94xxx_pread(struct dev_desc_t *adev,
					uint8_t *apData, size_t aLength, size_t aOffset)
{
	size_t read_len;
	uint32_t readAddr;
	uint32_t read_u32;
	uint32_t word_offset;

	if (0 == init_done) return 0;

	// test alignment
	aLength = get_actual_read_write_len(aLength, aOffset);
	read_len = aLength;

	readAddr = aOffset + DT_INTERNAL_FLASH_BASE_ADDR;

	word_offset = readAddr & 0xf;
	readAddr = (readAddr & (~0xf));
	if (0 != word_offset)
	{
		uint8_t *u32_addr;
		uint8_t  copy_size;

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
static int  set_flash_base(uint32_t u32DFBA)
{
	uint32_t   au32Config[2]; /* User Configuration */

	/* Read User Configuration 0 & 1 */
	if (FMC_ReadConfig(au32Config, 2) < 0) {
		printf("\nRead User Config failed!\n"); /* Error message */
		return -1;  /* failed to read User Configuration */
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


static void device_start()
{
	FMC_Open();

	/* following can be removed after adding erase of config page and
	 * writing 0x5A5A into CONFIG2 after erasing
	 */
	//set_flash_base(DT_INTERNAL_FLASH_BASE_ADDR);
	init_done = 1;
}


static uint8_t flash_erase(uint32_t errase_addr)
{
	if (DT_INTERNAL_FLASH_MEMORY_SIZE <= errase_addr)
	{
		return 1;
	}
	errase_addr += DT_INTERNAL_FLASH_BASE_ADDR;
	FMC_ENABLE_AP_UPDATE();
	FMC_Erase(errase_addr);
	FMC_DISABLE_AP_UPDATE();
	return 0;
}


/**
 * internal_flash_i94xxx_ioctl()
 *
 * return:
 */
static uint8_t internal_flash_i94xxx_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
//	struct I2S_onSPI_i94xxx_cfg_t *cfg_hndl;
//	struct dev_desc_t	*src_clock;

//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
//	src_clock = cfg_hndl->src_clock;

	if ((0 == init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		return 1;
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		device_start();
		break;
	case IOCTL_FLASH_WRAPPER_GET_ERASE_SIZE :
		*(uint32_t*)aIoctl_param1 = ERASE_BLOCK_SIZE;
		break;
	case IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE :
		*(uint32_t*)aIoctl_param1 = DT_INTERNAL_FLASH_MEMORY_SIZE;
		break;
	case IOCTL_FLASH_WRAPPER_ERASE :
		return flash_erase(*(uint32_t*)aIoctl_param1);
		break;
	default :
		return 1;
	}
	return 0;
}

#define	MODULE_NAME             internal_flash_i94xxx
#define	MODULE_IOCTL_FUNCTION   internal_flash_i94xxx_ioctl
#define MODULE_PWRITE_FUNCTION  internal_flash_i94xxx_pwrite
#define MODULE_PREAD_FUNCTION   internal_flash_i94xxx_pread
#define MODULE_HAS_NO_CONFIG_DATA
#define MODULE_HAS_NO_RUNTIME_DATA
#include "add_module.h"
