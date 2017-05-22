/*
 *
 *   file  :  flash_wrapper.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "flash_wrapper.h"
#include "flash_wrapper_api.h"


/***************   defines    *******************/


/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/





/**
 * flash_wrapper_pwrite()
 *
 * return:
 */
size_t flash_wrapper_pwrite(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct flash_wrapper_runtime_t *runtime_handle;
	struct flash_wrapper_cfg_t *cfg_hndl;
	struct dev_desc_t	*hw_flash_dev;
	uint32_t	erase_block_size;
	uint8_t 	*write_buffer;
    uint32_t 	pageAddr;
    uint32_t	 wrAddr;
    uint32_t 	written_size;
	uint32_t	flash_size;
	uint32_t	page_mask;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	hw_flash_dev = cfg_hndl->hw_flash_dev;
	erase_block_size = runtime_handle->erase_block_size;
	write_buffer = runtime_handle->write_buffer;
	flash_size = runtime_handle->flash_size;


	page_mask = erase_block_size-1; // for example : 4k page -> mask = 0xfff
	written_size = 0;
	wrAddr = aOffset;
    while (aLength)
    {
    	size_t wrSize;
    	size_t offsetInPage;

    	pageAddr = wrAddr & (~page_mask);
    	if (wrAddr >= flash_size)
    	{
    		break;
    	}
    	DEV_PREAD(hw_flash_dev, write_buffer, erase_block_size, pageAddr);

    	offsetInPage = wrAddr & page_mask;
		wrSize = erase_block_size;
    	if (0 != offsetInPage)
    	{
    		wrSize = erase_block_size - offsetInPage;
    	}
    	if (wrSize > aLength)
    	{
    		wrSize = aLength;
    	}

    	memcpy(write_buffer + offsetInPage, apData, wrSize);
		DEV_IOCTL_1_PARAMS(hw_flash_dev, IOCTL_FLASH_WRAPPER_ERASE, &pageAddr);
		DEV_PWRITE(hw_flash_dev, write_buffer,	wrSize, pageAddr);

        apData += wrSize;
        wrAddr += wrSize;
        written_size += wrSize;
        aLength -= wrSize;
    }

	return written_size;
}


/**
 * flash_wrapper_pread()
 *
 * return:
 */
size_t flash_wrapper_pread(struct dev_desc_t *adev,
					uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct flash_wrapper_cfg_t *cfg_hndl;
	struct dev_desc_t	*hw_flash_dev;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	hw_flash_dev = cfg_hndl->hw_flash_dev;

	return DEV_PREAD(hw_flash_dev, apData,	aLength, aOffset);
}


/**
 * flash_wrapper_ioctl()
 *
 * return:
 */
uint8_t flash_wrapper_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct flash_wrapper_cfg_t *cfg_hndl;
	struct dev_desc_t	*hw_flash_dev;
	struct flash_wrapper_runtime_t *runtime_handle;
	uint32_t	erase_block_size;
	uint32_t	flash_size;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	hw_flash_dev = cfg_hndl->hw_flash_dev;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		DEV_IOCTL_0_PARAMS(hw_flash_dev, IOCTL_DEVICE_START );
		DEV_IOCTL_1_PARAMS(hw_flash_dev,
				IOCTL_FLASH_WRAPPER_GET_ERASE_SIZE, &erase_block_size);
		runtime_handle->erase_block_size = erase_block_size;
		runtime_handle->write_buffer = (uint8_t*)malloc(erase_block_size);
		DEV_IOCTL_1_PARAMS(hw_flash_dev,
				IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE, &flash_size);
		runtime_handle->flash_size = flash_size;
		break;

	default :
		return 1;
	}
	return 0;
}
