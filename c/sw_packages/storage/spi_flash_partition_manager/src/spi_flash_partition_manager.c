/*
 *
 * file :   SPI_FLASH_PARTITION_MANAGER.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "spi_flash_partition_manager_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_spi_flash_partition_manager_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_management_api.h}

#include "spi_flash_api.h"

#include "spi_flash_partition_manager_api.h"
#include "spi_flash_partition_manager.h"

/********  defines *********************/
#define INSTANCE(hndl)	((spi_flash_partition_manager_Instance_t*)hndl)


#define SPI_FLASH_PARTITION_MANAGER_TIMEOUT  3000


/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/



/***********   local variables    **************/
#if SPI_FLASH_PARTITION_MANAGER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0
 static spi_flash_partition_manager_Instance_t SPI_FLASH_PARTITION_MANAGER_InstanceParams = {0} ;
#endif





#define TOTAL_SIZE_OF_FLASH			(1024*1024) // 1M
#define SECTOR_SIZE					(512) //
#define TOTAL_NUM_OF_SECTORS		(TOTAL_SIZE_OF_FLASH / SECTOR_SIZE)

#define SECTORS_PER_TRACK			8 //
#define TOTAL_NUM_OF_CYLINDERS		(TOTAL_NUM_OF_SECTORS / SECTORS_PER_TRACK)

#define NUM_OF_RESERVED_CYLINDERS	6 // reserved cylenders for bufferring
#define REPORTED_NUM_OF_CYLINDERS	(TOTAL_NUM_OF_CYLINDERS - NUM_OF_RESERVED_CYLINDERS) // windows will not recognize less then 0x11
#define NUM_OF_HEADS				0x1 //

#define REPORTED_SECTORS_ON_STORAGE	   ((NUM_OF_HEADS * REPORTED_NUM_OF_CYLINDERS) * SECTORS_PER_TRACK)
#define REPORTED_SECTORS_ON_PARTITION   (REPORTED_SECTORS_ON_STORAGE - 1*(SECTORS_PER_TRACK)) // first block is MBR

#define uint16_low8(x)          ((x) & 0xFF)
#define uint16_high8(x)         (((x) >> 8) & 0xFF)


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : spi_flash_partition_manager_pread
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
size_t  spi_flash_partition_manager_pread(const void *apHandle ,uint8_t *apData , size_t length ,size_t startAddr)
{
	return DEV_PREAD(INSTANCE(apHandle)->spi_flash_server_dev, apData , length , startAddr);

}



#define SPI_FLASH_PARTITION_MANAGER_SIZE   			(1024*1024)
#define SPI_FLASH_PARTITION_MANAGER_4K_BLOCKS_NUM   	(1024/4)
#define SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR   	(SPI_FLASH_PARTITION_MANAGER_SIZE - (4*1024))
#define SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR  (0xffffffff)

static size_t currWrite4KblockAddr = SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR;
static size_t dirty_512bytes_block;

#define TEMP_BUFF_SIZE 16
/*******************************************************************************
* Function Name  : SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(const void *apHandle)
{
	size_t i,j;
	uint8_t buffer[TEMP_BUFF_SIZE];
	pdev_descriptor_const   spi_flash_server_dev=INSTANCE(apHandle)->spi_flash_server_dev;

	// fill bufer with current data from main storage
	for(i=0 ; 8>i ; i++)
	{
		if( 0==( dirty_512bytes_block & (1<<i)))
		{
			for(j=0 ; 512 > j; j=j+TEMP_BUFF_SIZE)
			{
				spi_flash_partition_manager_pread(apHandle , buffer , TEMP_BUFF_SIZE , currWrite4KblockAddr + (512 * i) + j);
				DEV_PWRITE(spi_flash_server_dev, buffer , TEMP_BUFF_SIZE , SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR + (512 * i) + j);
				//SPI_FLASH_PARTITION_MANAGER_WriteData(apHandle , SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR + (512 * i) + j , buffer , TEMP_BUFF_SIZE);
			}
		}
	}

	//erase sector in main storage area
	DEV_IOCTL(spi_flash_server_dev, IOCTL_SPI_FLASH_ERRASE_SECTOR,&currWrite4KblockAddr);
//	SPI_FLASH_PARTITION_MANAGER_SectorErase(apHandle,currWrite4KblockAddr);

	// transfer from buder to  main storage area
	for(i=0 ; 8>i ; i++)
	{
		for(j=0 ; 512 > j; j=j+TEMP_BUFF_SIZE)
		{
			spi_flash_partition_manager_pread(apHandle , buffer , TEMP_BUFF_SIZE, SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR + (512 * i) + j);
			DEV_PWRITE(spi_flash_server_dev, buffer , TEMP_BUFF_SIZE , currWrite4KblockAddr + (512 * i) + j);
			//SPI_FLASH_PARTITION_MANAGER_WriteData(apHandle , currWrite4KblockAddr + (512 * i) + j , buffer , TEMP_BUFF_SIZE);
		}
	}

	//erase fuffer sector
	currWrite4KblockAddr = SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR;
	DEV_IOCTL(spi_flash_server_dev, IOCTL_SPI_FLASH_ERRASE_SECTOR, &currWrite4KblockAddr);
	//SPI_FLASH_PARTITION_MANAGER_SectorErase(apHandle,SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR);

	currWrite4KblockAddr = SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR;
	dirty_512bytes_block=0;

}



/*******************************************************************************
* Function Name  : spi_flash_partition_manager_pwrite
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
size_t  spi_flash_partition_manager_pwrite(const void *apHandle ,const uint8_t *apData , size_t length ,size_t startAddr)
{
	size_t offset_in_4k_block;
	size_t next_dirty_512bytes_block;

	offset_in_4k_block = startAddr & 0xfff;// startAddr % 4k = startAddr % (2^12)
	startAddr = (startAddr & (~0xfff) ) ; //  startAddr in 4k blocks

	if(startAddr != currWrite4KblockAddr)
	{
		if(SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR != currWrite4KblockAddr)
		{
			SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(apHandle);
		}
	}

	currWrite4KblockAddr=startAddr;
	next_dirty_512bytes_block = (1<<(offset_in_4k_block >> 9)); //  offset_in_4k_block / 512

	if(next_dirty_512bytes_block & dirty_512bytes_block)
	{
		SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(apHandle);
	}

	dirty_512bytes_block |= next_dirty_512bytes_block;
	DEV_PWRITE(INSTANCE(apHandle)->spi_flash_server_dev, apData , 512 , SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR +  offset_in_4k_block);
//	SPI_FLASH_PARTITION_MANAGER_WriteData(apHandle , SPI_FLASH_PARTITION_MANAGER_BUFFER_ADDR +  offset_in_4k_block , apData , 512);

	if( 0xff == dirty_512bytes_block)
	{
		SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(apHandle);
	}

	return length;
//
}


// (6*4k) will be saved
const uint8_t PartitionTable[16] =
{
	    0x00,// boot/non-boot signature
	    0x00, 0x01, 0x01, // CHS of first section in partition
	    0x01/*0x04*/, // FAT type
	    0x00, SECTORS_PER_TRACK , REPORTED_NUM_OF_CYLINDERS-1 ,  // CHS of last section in partition
	    SECTORS_PER_TRACK, 0x00,  0x00, 0x00, // LBA of first sector in partition
	    uint16_low8(REPORTED_SECTORS_ON_PARTITION),uint16_high8(REPORTED_SECTORS_ON_PARTITION),  0x00, 0x00,	// partition size																		 0x00, 0x00,
};

#define START_OF_PARTITION_SECTOR  0x08
#define SECTORS_PER_FAT  0x08
#define RESERVED_SECTORS  0x08
const uint8_t VBR[62] =
{
	0xEB, 0x3C, 0x90,
	'A', 'N', 'D', 'R', 'E', 'W', '_', 'Z',// system that created partition
	0x00, 0x02, // bytes per sector
	0x01, // logical sectors per claster
	uint16_low8(RESERVED_SECTORS),uint16_high8(RESERVED_SECTORS), // reserved logical sectors (we align to 4k )
	0x02,// number of fats
	0x00, 0x01, //number of root entries . 0x100=256 entries
	uint16_low8(REPORTED_SECTORS_ON_PARTITION),uint16_high8(REPORTED_SECTORS_ON_PARTITION),	// partition size																		 0x00, 0x00,
	0xF0, // partition type
	uint16_low8(SECTORS_PER_FAT),uint16_high8(SECTORS_PER_FAT), // sectrors per FAT
	uint16_low8(SECTORS_PER_TRACK),uint16_high8(SECTORS_PER_TRACK), // sector per track/header
	uint16_low8(NUM_OF_HEADS),uint16_high8(NUM_OF_HEADS), //number of heads
	0x01, 0x00, 0x00, 0x00,// hidden sectors (sectors before partition starts)

	0x00, 0x00, 0x00, 0x00, // total number of sectors
	0x80, 0x00, 0x29, 0x59, 0x25, 0xEB, 0x1B,
	'A' , 'N' , 'D' , 'R' , 'E' , 'W' , '_' , 'P' , 'R' , 'T' , ' ' ,
	'F' , 'A' , 'T' , '1' , '2' , ' ' , ' ' , ' ' ,
};

const uint8_t StartOfFatSection[3] =
{
		0xF8,0xFF,0xFF,
};

const uint8_t StartOfRootSection[32] =
{
		'A', 'N', 'D', 'R', 'E', 'W', ' ' , ' ' , ' ' , ' ' , ' ' , 0x08,  0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0x45,	0x2F, 0x45, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
};

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_PARTITION_MANAGER_Format                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  SPI_FLASH_PARTITION_MANAGER_Format(const void *apHandle)
{
	uint8_t *block_data;
	size_t i,j;

	block_data=(uint8_t *)malloc(0x200);// = 512

	// writing MBR
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0x1be],PartitionTable,16);
	block_data[0x1fe]=0x55;
	block_data[0x1ff]=0xaa;
	spi_flash_partition_manager_pwrite(apHandle, block_data , 0x200 ,0 );

	// writing VBR
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0],VBR,62);
	block_data[0x1fe]=0x55;
	block_data[0x1ff]=0xaa;
	spi_flash_partition_manager_pwrite(apHandle  , block_data , 0x200,512 * START_OF_PARTITION_SECTOR/*next 4k*/);

	// writing FAT
	for (j=0;j<2;j++)
	{
		memset(block_data ,0 , 0x200);
		memcpy(&block_data[0],StartOfFatSection,3);
		spi_flash_partition_manager_pwrite(apHandle ,  block_data , 0x200,
				512 * (START_OF_PARTITION_SECTOR + RESERVED_SECTORS +(j*SECTORS_PER_FAT)) );
		memset(block_data ,0 , 0x200);
		for(i=1;i<SECTORS_PER_FAT;i++)
		{
			spi_flash_partition_manager_pwrite(apHandle , block_data , 0x200,
					512 * (START_OF_PARTITION_SECTOR+ RESERVED_SECTORS + (j*SECTORS_PER_FAT) + i) );
		}
	}

	// writing root section
	memset(block_data ,0 , 0x200);
	memcpy(&block_data[0],StartOfRootSection,32);
	spi_flash_partition_manager_pwrite(apHandle , block_data , 0x200,
			512 * (START_OF_PARTITION_SECTOR + RESERVED_SECTORS +(2*SECTORS_PER_FAT))  );
	memset(block_data ,0 , 0x200);
	for(i=1;i<SECTORS_PER_FAT;i++)
	{
		spi_flash_partition_manager_pwrite(apHandle , block_data , 0x200,
				512 * (START_OF_PARTITION_SECTOR+ RESERVED_SECTORS + (2*SECTORS_PER_FAT) + i)  );
	}

	free(block_data);

	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:       SPI_FLASH_PARTITION_MANAGER_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void spi_flash_partition_manager_task( void *pvParameters )
{

	for( ;; )
	{
		vTaskDelay( 3000 );

		if(SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR != currWrite4KblockAddr)
		{
			SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(pvParameters);
		}

		os_stack_test();

	}


}
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_PARTITION_MANAGER_start                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  SPI_FLASH_PARTITION_MANAGER_start(pdev_descriptor_t apdev)
{
	uint8_t mbr_signiture[2];
//	uint32_t id;

//	uint8_t buffer[TEMP_BUFF_SIZE*10];
	dirty_512bytes_block=0;


	spi_flash_partition_manager_pread(aHandle, mbr_signiture , 2 , 0x1fe);
	if( (0x55 != mbr_signiture[0]) ||  (0xAA != mbr_signiture[1]))
	{
		SPI_FLASH_PARTITION_MANAGER_Format(aHandle);
	}


	os_create_task("spi_flash_partition_manage_task",
			spi_flash_partition_manager_task,  aHandle ,
			SPI_FLASH_PARTITION_MANAGER_CONFIG_TASK_STACK_SIZE ,
			SPI_FLASH_PARTITION_MANAGER_CONFIG_TASK_PRIORITY);


	return 0;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_flash_partition_manager_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t spi_flash_partition_manager_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =   0; //size
			break;

		case IOCTL_DEVICE_START :
			SPI_FLASH_PARTITION_MANAGER_start(aHandle);
			break;

		case IOCTL_SPI_FLASH_PARTITION_MANAGER_GET_BLOCK_COUNT :
			*((uint32_t*)aIoctl_param1) = REPORTED_SECTORS_ON_STORAGE ; // block is 512bytes = 2^9 bytes
			break;

		case IOCTL_SPI_FLASH_PARTITION_MANAGER_GET_BLOCK_SIZE :
			*((uint32_t*)aIoctl_param1) = SECTOR_SIZE ;
			break;

		case IOCTL_SPI_FLASH_PARTITION_MANAGER_FORMAT :
			SPI_FLASH_PARTITION_MANAGER_Format(aHandle) ;
			break;

		case IOCTL_SPI_FLASH_PARTITION_MANAGER_FLUSH :
			if(SPI_FLASH_PARTITION_MANAGER_NON_EXISTENT_ADDR != currWrite4KblockAddr)
			{
				SPI_FLASH_PARTITION_MANAGER_Copy_from_buffer_to_main_area(aHandle);
			}
			break;

		default :
			return 1;
	}
	return 0;
}

#if SPI_FLASH_PARTITION_MANAGER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_FLASH_PARTITION_MANAGER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  spi_flash_partition_manager_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &SPI_FLASH_PARTITION_MANAGER_InstanceParams;
	aDevDescriptor->ioctl = spi_flash_partition_manager_ioctl;
	aDevDescriptor->pread = spi_flash_partition_manager_pread;
	aDevDescriptor->pwrite = spi_flash_partition_manager_pwrite;

	return 0 ;

}
#endif
