/*
 *
 *   file  :  flash_wrapper.c
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_tasks_defines.h"
#include "errors_api.h"

#include "dev_management_api.h"

#include "os_wrapper.h"
#include "flash_wrapper.h"
#include "flash_wrapper_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "flash_wrapper_add_component.h"

/***************   defines    *******************/


/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/


static os_mutex_t  flash_wrapper_mutex;

static void save_to_flash_and_fetch_new_block( struct dev_desc_t *adev,
		uint32_t curr_block_addr, uint32_t new_block_addr)
{
	struct flash_wrapper_runtime_t *runtime_handle;
	struct flash_wrapper_cfg_t *cfg_hndl;
	struct dev_desc_t	*hw_flash_dev;
	uint8_t 	*write_buffer;
	uint32_t	erase_block_size;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	erase_block_size = runtime_handle->erase_block_size;
	hw_flash_dev = cfg_hndl->hw_flash_dev;
	write_buffer = runtime_handle->write_buffer;

	if (runtime_handle->curr_block_is_dirty)
	{
		DEV_IOCTL_1_PARAMS(hw_flash_dev,
				IOCTL_FLASH_WRAPPER_ERASE, &curr_block_addr);
		DEV_PWRITE(hw_flash_dev,
				write_buffer, erase_block_size, curr_block_addr);
	}
	DEV_PREAD(hw_flash_dev, write_buffer, erase_block_size, new_block_addr);
	runtime_handle->curr_block_addr = new_block_addr;
	runtime_handle->curr_block_is_dirty = 0;

}


/**
 * flash_wrapper_pwrite()
 *
 * return:
 */
size_t flash_wrapper_pwrite(struct dev_desc_t *adev,
						const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct flash_wrapper_runtime_t *runtime_handle;
	uint32_t	erase_block_size;
	uint8_t 	*write_buffer;
    uint32_t 	pageAddr;
    uint32_t	 wrAddr;
    uint32_t 	written_size;
	uint32_t	flash_size;
	uint32_t	page_mask;
	uint32_t	curr_block_addr;

	os_mutex_take_infinite_wait(flash_wrapper_mutex);

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	erase_block_size = runtime_handle->erase_block_size;
	write_buffer = runtime_handle->write_buffer;
	flash_size = runtime_handle->flash_size;
	curr_block_addr = runtime_handle->curr_block_addr;


	page_mask = erase_block_size - 1; // for example : 4k page -> mask = 0xfff
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

    	if (pageAddr != curr_block_addr)
    	{
    		save_to_flash_and_fetch_new_block(adev, curr_block_addr, pageAddr);
    	}

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
    	runtime_handle->curr_block_is_dirty = 1;

        apData += wrSize;
        wrAddr += wrSize;
        written_size += wrSize;
        aLength -= wrSize;
    }

	os_mutex_give(flash_wrapper_mutex);

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
 * flash_wrapper_task()
 *
 * return:
 */
static void flash_wrapper_task( void *adev )
{
	struct flash_wrapper_runtime_t *runtime_handle;
	uint32_t	curr_block_addr;
	uint8_t		curr_block_is_dirty;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	for( ;; )
	{
		os_delay_ms(2000);

		if (OS_MUTEX_TAKE_SUCCESS !=
				os_mutex_take_infinite_wait(flash_wrapper_mutex))
		{
			CRITICAL_ERROR("cannot take mutex\n");
		}
		curr_block_is_dirty = runtime_handle->curr_block_is_dirty;
		if (2 == curr_block_is_dirty)
		{
			curr_block_addr = runtime_handle->curr_block_addr;
			save_to_flash_and_fetch_new_block(
					adev, curr_block_addr, curr_block_addr);
		}
		else if (1 == curr_block_is_dirty)
		{
			runtime_handle->curr_block_is_dirty = 2;
		}
		os_mutex_give(flash_wrapper_mutex);

		os_stack_test();
	}
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
		errors_api_check_if_malloc_succeed(runtime_handle->write_buffer);
		DEV_IOCTL_1_PARAMS(hw_flash_dev,
				IOCTL_FLASH_WRAPPER_GET_FLASH_SIZE, &flash_size);
		runtime_handle->flash_size = flash_size;
		runtime_handle->curr_block_addr = flash_size;//dummy address
    	runtime_handle->curr_block_is_dirty = 0;

		flash_wrapper_mutex = os_create_mutex();
		os_create_task("flash_wrapper_task",
				flash_wrapper_task, adev,
				FLASH_WRAPPER_TASK_STACK_SIZE,
				FLASH_WRAPPER_TASK_PRIORITY);

		break;

	default :
		return 1;
	}
	return 0;
}
