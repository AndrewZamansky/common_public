/*
 *
 * usb_i94xxx.c

 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "usb_i94xxx.h"
#include "I94100.h"

#include "irq_api.h"
#include "usb_device_api.h"
#include "clock_control_i94xxx_api.h"

#include "usbd.h"
#include "string.h"

#include "os_wrapper.h"

#include "hid_trans.h"
#undef USBD_MAX_POWER
#undef LEN_CONFIG_AND_SUBORDINATE
#undef EP0_MAX_PKT_SIZE
#undef EP2_MAX_PKT_SIZE
#undef EP3_MAX_PKT_SIZE
#undef EP0_BUF_LEN
#undef EP1_BUF_LEN
#undef EP2_BUF_LEN
#undef EP3_BUF_LEN
#undef INT_IN_EP_NUM
#undef INT_OUT_EP_NUM
#include "audio_class.h"

/*following line add module to available module list for dynamic device tree*/
#include "usb_i94xxx_add_component.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/

/* ------------------------ local variables --------*/
static S_USBD_INFO_T l_gsInfo;
CLASS_REQ pfnClassReq;

#define MAX_NUM_OF_ENDPOINTS    12
static usb_dev_out_endpoint_callback_func_t
				callback_functions[MAX_NUM_OF_ENDPOINTS];
struct dev_desc_t *callback_devs[MAX_NUM_OF_ENDPOINTS];

/*--------------------------------------------------------------------------*/
/* Global variables for Control Pipe */
int32_t g_TotalSectors = 0;

uint8_t volatile g_u8EP2Ready = 0;
uint8_t volatile g_u8EP3Ready = 0;
uint8_t volatile g_u8StartOfFrame = 0;

/* USB flow control variables */
uint8_t g_u8BulkState;
uint8_t g_u8Prevent = 0;
uint8_t g_u8Size;

volatile uint8_t g_usbd_RecMute       = 0x01;     /* Record MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_RecVolumeL    = 0x1000;   /* Record left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecVolumeR    = 0x1000;   /* Record right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecMaxVolume  = 0x7FFF;
volatile int16_t g_usbd_RecMinVolume  = 0x8000;
volatile int16_t g_usbd_RecResVolume  = 0x400;

volatile uint8_t g_usbd_PlayMute      = 0x01;     /* Play MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_PlayVolumeL   = 0x1000;   /* Play left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayVolumeR   = 0x1000;   /* Play right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayMaxVolume = 0x7FFF;
volatile int16_t g_usbd_PlayMinVolume = 0x8000;
volatile int16_t g_usbd_PlayResVolume = 0x400;

/* Status */
static volatile uint8_t g_u8RecEn = 0;
static volatile uint8_t g_u8PlayEn = 0;      /* To indicate data is output to I2S */
static volatile int32_t g_i32AdjFlag = 0;    /* To indicate current I2S frequency adjustment status */

/* Temp buffer for play and record */
uint32_t g_au32UsbTmpBuf[((PLAY_RATE > REC_RATE) ? PLAY_RATE : REC_RATE) / 2000 * ((PLAY_CHANNELS > REC_CHANNELS) ? PLAY_CHANNELS : REC_CHANNELS)] = {0};

/* Player Buffer and its pointer */
uint32_t g_au32PcmPlayBuf[BUF_LEN] = {0};
volatile uint32_t g_u32PlayPos_Out = 0;
volatile uint32_t g_u32PlayPos_In = 0;



// HID Transfer definition
#define HIDTRANS_CMD_SIGNATURE    0x43444948
#define HIDTRANS_CMD_NONE         0x00
#define HIDTRANS_CMD_ERASE        0x71
#define HIDTRANS_CMD_READ         0xD2
#define HIDTRANS_CMD_WRITE        0xC3

#ifdef __GNUC__
typedef struct __attribute__((packed))
#else
typedef __packed struct
#endif
{
    uint8_t  u8Cmd;
    uint8_t  u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
} HIDTRANS_CMD_T;

#define DEMO_BUFSIZE     (HIDTRANS_SECTORSIZE*8)
// Test demo buffer to upload/download through HID report
uint8_t g_u8DemoBuffer[DEMO_BUFSIZE] = {0};
/* Page buffer to upload/download through HID report */
static uint8_t  g_u8PageBuff[HIDTRANS_PAGESIZE] = {0};
/* The bytes of data in g_u8PageBuff */
static uint32_t g_u32BytesInPageBuf = 0;
/* HID transfer command handler */
static HIDTRANS_CMD_T g_sHidCmd;
/* HID process data buffer pointer */
static uint8_t* g_pu8DataBuff;

#define printf(...)
#define DBG_PRINTF(...)

// HID transfer command function
void HIDTrans_EraseSector(uint32_t u32StartSector,uint32_t u32Sectors)
{
	printf("  >> Get erase sector request -\r\n");
	printf("     - Start sector number : %d\r\n", u32StartSector);
	printf("     - Erase sector counts : %d\r\n", u32Sectors);

	memset(g_u8DemoBuffer+u32StartSector*HIDTRANS_SECTORSIZE, 0xFF, u32Sectors*HIDTRANS_SECTORSIZE);
}
// Provide user prepare read buffer for USB request.
void HIDTrans_PrepareReadPage(uint32_t* pu32Address,uint32_t u32StartPage,uint32_t u32Pages)
{
	printf("  >> Get read page request -\r\n");
	printf("     - Start page number : %d\r\n", u32StartPage);
	printf("     - Read page counts  : %d\r\n", u32Pages);

	if( (u32Pages>0) && ((u32StartPage+u32Pages)<DEMO_BUFSIZE/HIDTRANS_PAGESIZE) )
		*pu32Address = (uint32_t)g_u8DemoBuffer + u32StartPage*HIDTRANS_PAGESIZE;
	else
		*pu32Address = (size_t)NULL;
}
// Provide user prepare write buffer for USB request.
void HIDTrans_PrepareWritePage(uint32_t* pu32Address,uint32_t u32StartPage,uint32_t u32Pages)
{
	printf("  >> Get write page request -\r\n");
	printf("     - Start page number : %d\r\n", u32StartPage);
	printf("     - Write page counts : %d\r\n", u32Pages);

	if( (u32Pages>0) && ((u32StartPage+u32Pages)<DEMO_BUFSIZE/HIDTRANS_PAGESIZE) )
		*pu32Address = (uint32_t)g_u8DemoBuffer + u32StartPage*HIDTRANS_PAGESIZE;
	else
		*pu32Address = (size_t)NULL;
}
// Provide user get write data.
void HIDTrans_GetWriteData(uint32_t u32Address,uint32_t u32Pages)
{
	printf("  >> Get write data finish message.\r\n");
	printf("     - Write page counts : %d\r\n", u32Pages);
}


int32_t HIDTrans_CmdEraseSectors(HIDTRANS_CMD_T *pCmd)
{
	HIDTrans_EraseSector(pCmd->u32Arg1,pCmd->u32Arg2);
    pCmd->u8Cmd = HIDTRANS_CMD_NONE;
    return 0;
}
int32_t HIDTrans_CmdReadPages(HIDTRANS_CMD_T *pCmd)
{
	uint32_t u32Address = 0;

    if(pCmd->u32Arg2)
    {
		// Call read page function to provide input buffer data.
		HIDTrans_PrepareReadPage(&u32Address,pCmd->u32Arg1,pCmd->u32Arg2);
		// Update data to page buffer to upload
		if( u32Address == 0 )
		{
			g_pu8DataBuff = NULL;
			memset(g_u8PageBuff,0, HIDTRANS_PAGESIZE);
		}
		else
		{
			g_pu8DataBuff = (uint8_t*)u32Address;
			memcpy(g_u8PageBuff, g_pu8DataBuff, HIDTRANS_PAGESIZE);
		}
		g_u32BytesInPageBuf = HIDTRANS_PAGESIZE;
        // The signature word is used as page counter
        pCmd->u32Signature = 1;
        // Trigger HID IN
        USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(INT_IN_EP_NUM)), (void *)g_u8PageBuff, EP6_MAX_PKT_SIZE);
        USBD_SET_PAYLOAD_LEN(INT_IN_EP_NUM, EP6_MAX_PKT_SIZE);
        g_u32BytesInPageBuf -= EP6_MAX_PKT_SIZE;
    }
    return 0;
}
int32_t HIDTrans_CmdWritePages(HIDTRANS_CMD_T *pCmd)
{
    uint32_t u32Address;

	// Call read page function to provide input buffer data.
	HIDTrans_PrepareWritePage(&u32Address,pCmd->u32Arg1,pCmd->u32Arg2);
	// Set output data buffer address
	if( u32Address == 0 )
		g_pu8DataBuff = NULL;
	else
		g_pu8DataBuff = (uint8_t*)u32Address;

    g_u32BytesInPageBuf = 0;
    pCmd->u32Signature = 0;
    return 0;
}
uint32_t HIDTrans_CalCheckSum(uint8_t *pu8buf, uint32_t u32Size)
{
    uint32_t u32Sum = 0, u32i = 0;

    while(u32Size--)
    {
        u32Sum += pu8buf[u32i++];
    }
    return u32Sum;
}
int32_t HIDTrans_ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
    USBD_MemCopy((uint8_t *)&g_sHidCmd, pu8Buffer, u32BufferLen);

    /* Check size */
    if((g_sHidCmd.u8Size > sizeof(g_sHidCmd)) || (g_sHidCmd.u8Size > u32BufferLen))
        return -1;

    /* Check signature */
    if(g_sHidCmd.u32Signature != HIDTRANS_CMD_SIGNATURE)
        return -1;

    /* Calculate checksum & check it*/
    if(HIDTrans_CalCheckSum((uint8_t *)&g_sHidCmd, g_sHidCmd.u8Size) != g_sHidCmd.u32Checksum)
        return -1;

    switch(g_sHidCmd.u8Cmd)
    {
        case HIDTRANS_CMD_ERASE:
        {
            HIDTrans_CmdEraseSectors(&g_sHidCmd);
            break;
        }
        case HIDTRANS_CMD_READ:
        {
            HIDTrans_CmdReadPages(&g_sHidCmd);
            break;
        }
        case HIDTRANS_CMD_WRITE:
        {
            HIDTrans_CmdWritePages(&g_sHidCmd);
            break;
        }
        default:
            return -1;
    }
    return 0;
}
void HIDTrans_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    // Check if it is in the data phase of write command
    if((g_sHidCmd.u8Cmd == HIDTRANS_CMD_WRITE) && (g_sHidCmd.u32Signature<g_sHidCmd.u32Arg2))
    {
        // Get data from HID OUT
        USBD_MemCopy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, EP7_MAX_PKT_SIZE);
        g_u32BytesInPageBuf += EP7_MAX_PKT_SIZE;

        // The HOST must make sure the data is PAGE_SIZE alignment
        if(g_u32BytesInPageBuf >= HIDTRANS_PAGESIZE)
        {
			memcpy(&g_pu8DataBuff[g_sHidCmd.u32Signature*HIDTRANS_PAGESIZE],g_u8PageBuff,HIDTRANS_PAGESIZE);
            g_sHidCmd.u32Signature++;
            // Write command complete!
            if(g_sHidCmd.u32Signature >= g_sHidCmd.u32Arg2)
			{
				HIDTrans_GetWriteData((uint32_t)g_pu8DataBuff,g_sHidCmd.u32Arg2);
                g_sHidCmd.u8Cmd = HIDTRANS_CMD_NONE;
            }
			g_u32BytesInPageBuf = 0;
        }
    }
    else
    {
        // Check and process the command packet
        HIDTrans_ProcessCommand(pu8EpBuf, u32Size);
    }
}
void HIDTrans_SetInReport(void)
{
    uint8_t* ptr;

    // Check if it is in data phase of read command
    if(g_sHidCmd.u8Cmd == HIDTRANS_CMD_READ)
    {
        // Process the data phase of read command
        if((g_sHidCmd.u32Signature >= g_sHidCmd.u32Arg2) && (g_u32BytesInPageBuf == 0))
            g_sHidCmd.u8Cmd = HIDTRANS_CMD_NONE;
        else
        {
            if(g_u32BytesInPageBuf == 0)
            {
				// The previous page has sent out. Read new page to page buffer
				if( g_pu8DataBuff == NULL )
					memset(g_u8PageBuff,0, HIDTRANS_PAGESIZE);
				else
					memcpy(g_u8PageBuff, &g_pu8DataBuff[g_sHidCmd.u32Signature*HIDTRANS_PAGESIZE], HIDTRANS_PAGESIZE);
				g_u32BytesInPageBuf = HIDTRANS_PAGESIZE;
                // Update the page counter
                g_sHidCmd.u32Signature++;
            }

            // Prepare the data for next HID IN transfer
            ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(INT_IN_EP_NUM));
            USBD_MemCopy(ptr, (void *)&g_u8PageBuff[HIDTRANS_PAGESIZE - g_u32BytesInPageBuf], EP6_MAX_PKT_SIZE);
            USBD_SET_PAYLOAD_LEN(INT_IN_EP_NUM, EP6_MAX_PKT_SIZE);
            g_u32BytesInPageBuf -= EP6_MAX_PKT_SIZE;
        }
    }
}



/* Get number of data in buffer */
uint32_t GetSamplesInBuf(void)
{
    int32_t i32Tmp;

    i32Tmp = g_u32PlayPos_In;
    i32Tmp -= g_u32PlayPos_Out;
    if(i32Tmp < 0)
        i32Tmp += BUF_LEN;

    return (uint32_t)i32Tmp;
}

volatile uint32_t fifo_ptr;
volatile uint32_t systick_val_prev;
volatile uint32_t systick_val;
/*--------------------------------------------------------------------------*/
void DPWM_IRQHandler(void)
{
	uint32_t u32Idx, u32Temp, i;
	systick_val = SysTick->VAL;

	/* Player buffer is not half-filled */
	if( g_u8PlayEn == 0)
	{
		fifo_ptr = DPWM_GET_FIFOPOINTER(DPWM);
//		if (fifo_ptr > 8)
//		{
//			CRITICAL_ERROR("test");
//		}
//		if( DPWM_IS_FIFOEMPTY(DPWM))
		if (fifo_ptr < 16)
		{
			/* Fill 0x00 to DPWM FIFO */
			for( i=0; i<8; i++)
				DPWM_WRITE_INDATA(DPWM,0);
		}
		systick_val_prev = SysTick->VAL;
	}
	else
	{
		for(i=0; i<8; i++)
		{
                  u32Temp = g_u32PlayPos_In;
			if(!DPWM_IS_FIFOFULL(DPWM) && (g_u32PlayPos_Out != u32Temp))
			{
				/* Check ring buffer turn around */
				u32Idx = g_u32PlayPos_Out + 1;
				if(u32Idx >= BUF_LEN)
					u32Idx = 0;

				/* Write two channels data to FIFO */
				u32Temp = (g_au32PcmPlayBuf[u32Idx] & 0xFFFF0000) >> 16;
				DPWM_WRITE_INDATA(DPWM, g_au32PcmPlayBuf[u32Idx] & 0x0000FFFF);
				DPWM_WRITE_INDATA(DPWM, u32Temp);

				/* Update OUT index */
				g_u32PlayPos_Out = u32Idx;
			}
			else
				break;
		}
	}
}

volatile uint32_t tmp2;

/*--------------------------------------------------------------------------*/
void USBD_IRQHandler(void)
{
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_FLDET)
	{
        // Floating detect
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

        if(USBD_IS_ATTACHED())
		{
			// Enable GPB15(VBUS) pull down state to solute suspend event issue.
			//GPIO_EnablePullState(PB,BIT15,GPIO_PUSEL_PULL_DOWN);
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
		else
		{
			// Disable GPB15 pull down state.
		//	GPIO_DisablePullState(PB,BIT15);
            /* USB Un-plug */
            USBD_DISABLE_USB();
        }
    }

//------------------------------------------------------------------

	if ( u32IntSts & USBD_INTSTS_SOF )
	{
		/* Clear event flag */
		USBD_CLR_INT_FLAG(USBD_INTSTS_SOF);
		g_u8StartOfFrame = 1;
	}

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_BUS)
	{
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

        if(u32State & USBD_STATE_USBRST) {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
            DBG_PRINTF("Bus reset\n");
        }
        if(u32State & USBD_STATE_SUSPEND) {
            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
            DBG_PRINTF("Suspend\n");
        }
        if(u32State & USBD_STATE_RESUME) {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
            DBG_PRINTF("Resume\n");
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_USB)
	{
        // EP events
        if(u32IntSts & USBD_INTSTS_EP0) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
            // control IN
            USBD_CtrlIn();
        }

        if(u32IntSts & USBD_INTSTS_EP1) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            // control OUT
            USBD_CtrlOut();
        }

        if(u32IntSts & USBD_INTSTS_EP2)
		{
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);
            //
            EP2_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP3)
		{
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);
            //
            EP3_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP4)
		{
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
        }

        if(u32IntSts & USBD_INTSTS_EP5)
		{
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
        }

        if(u32IntSts & USBD_INTSTS_EP6) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP6);
            EP6_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP7) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP7);
            EP7_Handler();
        }

        // USB event
        if(u32IntSts & USBD_INTSTS_SETUP) {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION(EP0);
            USBD_STOP_TRANSACTION(EP1);

            USBD_ProcessSetupPacket();
        }
    }

    tmp2 = USBD_GET_BUS_STATE();


}


void EP2_Handler(void)
{
	/* Bulk IN */
    g_u8EP2Ready = 1;
}

uint8_t RecLen, RecBuf[768];
void EP3_Handler(void)
{
	usb_dev_out_endpoint_callback_func_t callback_func;
	uint32_t u32Len;
    int32_t i;
    uint8_t *pu8Buf;
    uint8_t *pu8Src;
    uint32_t u32Idx;

    /* Get the address in USB buffer */
    pu8Src = (uint8_t *)((uint32_t)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP3));

    /* Prepare for nex OUT packet */
    USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /* Get the temp buffer */
    pu8Buf = (uint8_t *)g_au32UsbTmpBuf;

    /* Calculate byte size of play data */
    u32Len = ALT2_PLAY_RATE / 1000 * PLAY_CHANNELS * 2;

    /* Copy all data from USB buffer to SRAM buffer */
    /* We assume the source data are 4 bytes alignment. */
    for(i = 0; i < u32Len; i += 4)
    {
        pu8Buf[i] = pu8Src[i];
        pu8Buf[i + 1] = pu8Src[i + 1];
        pu8Buf[i + 2] = pu8Src[i + 2];
        pu8Buf[i + 3] = pu8Src[i + 3];
    }
#if 1
	callback_func = callback_functions[3];
	if (NULL != callback_functions)
	{
		callback_func(callback_devs[3], pu8Buf, u32Len);
	}


#else
    /* Calculate word length */
    u32Len = u32Len >> 2;
    for(i = 0; i < u32Len; i++)
    {
        /* Check ring buffer turn around */
        u32Idx = g_u32PlayPos_In + 1;
        if(u32Idx >= BUF_LEN)
            u32Idx = 0;

        /* Check if buffer full */
        if(u32Idx != g_u32PlayPos_Out)
        {
            /* Update play ring buffer only when it is not full */
            g_au32PcmPlayBuf[u32Idx] = g_au32UsbTmpBuf[i];

            /* Update IN index */
            g_u32PlayPos_In = u32Idx;
        }
    }

    if(g_u8PlayEn == 0)
    {
        /* Start play data output through I2S only when we have enough data in buffer */
        if(GetSamplesInBuf() > BUF_LEN / 2)
            g_u8PlayEn = 1;
    }
#endif
}


// Interrupt IN handler
void EP6_Handler(void)
{
	HIDTrans_SetInReport();
}

// Interrupt OUT handler
void EP7_Handler(void)
{
	uint8_t *ptr;
    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(INT_OUT_EP_NUM));
    HIDTrans_GetOutReport(ptr, USBD_GET_PAYLOAD_LEN(INT_OUT_EP_NUM));
    USBD_SET_PAYLOAD_LEN(INT_OUT_EP_NUM, EP7_MAX_PKT_SIZE);
}



void UAC_Init(void)
{
    /* Init setup packet buffer */
    /* Buffer range for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    /*****************************************************/
    /* EP2 ==> Iso IN endpoint, address 2 */
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | USBD_CFG_TYPE_ISO | ISO_IN_EP_NUM);
    /* Buffer range for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

    /* EP3 ==> Iso Out endpoint, address 3 */
    USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | USBD_CFG_TYPE_ISO | ISO_OUT_EP_NUM);
    /* Buffer range for EP3 */
    USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);

    /* trigger to receive OUT data */
    USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /*****************************************************/
    /* EP6 ==> Interrupt IN endpoint, address 6 */
    USBD_CONFIG_EP(EP6, USBD_CFG_EPMODE_IN | INT_IN_EP_NUM);
    /* Buffer range for EP6 */
    USBD_SET_EP_BUF_ADDR(EP6, EP6_BUF_BASE);

    /* EP7 ==> Interrupt Out endpoint, address 7 */
    USBD_CONFIG_EP(EP7, USBD_CFG_EPMODE_OUT | INT_OUT_EP_NUM);
    /* Buffer range for EP7 */
    USBD_SET_EP_BUF_ADDR(EP7, EP7_BUF_BASE);
    /* trigger to receive OUT data */
    USBD_SET_PAYLOAD_LEN(EP7, EP7_MAX_PKT_SIZE);

}

void UAC_ClassRequest(void)
{
    uint8_t buf[8];
    uint32_t u32Temp;

    USBD_GetSetupPacket(buf);

    if(buf[0] & 0x80)    /* request data transfer direction */
	{
		// Device to host
		switch(buf[1])
		{
			case UAC_GET_CUR:
			{
				switch(buf[3])
				{
					case MUTE_CONTROL:
					{
						if (REC_FEATURE_UNITID == buf[5])
                                                {
                                                  u32Temp = g_usbd_RecMute;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                }
						else if (PLAY_FEATURE_UNITID == buf[5])
                                                {
                                                  u32Temp = g_usbd_PlayMute;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                }

						/* Data stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 1);
						break;
					}
					case VOLUME_CONTROL:
					{
						if (REC_FEATURE_UNITID == buf[5])
						{
							/* Left or right channel */
							if(buf[2] == 1)
							{
                                                          u32Temp = g_usbd_RecVolumeL;
								M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                                u32Temp = g_usbd_RecVolumeL >> 8;
								M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
							}
							else
							{
                                                          u32Temp = g_usbd_RecVolumeR;
								M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                                u32Temp = g_usbd_RecVolumeR >> 8;
								M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
							}
						}
						else if (PLAY_FEATURE_UNITID == buf[5])
						{
							/* Left or right channel */
							if(buf[2] == 1)
							{
                                                          u32Temp = g_usbd_PlayVolumeL;
									M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                                        u32Temp = g_usbd_PlayVolumeL >> 8;
									M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
							}
							else
							{
                                                          u32Temp = g_usbd_PlayVolumeR;
									M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                                        u32Temp = g_usbd_PlayVolumeR >> 8;
									M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
							}
						}
						/* Data stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 2);
						break;
					}
					default:
					{
							/* Setup error, stall the device */
							USBD_SetStall(0);
					}
				}

				// Trigger next Control Out DATA1 Transaction.
				/* Status stage */
				USBD_PrepareCtrlOut(0, 0);
				break;
			}
			case UAC_GET_MIN:
			{
				switch(buf[3])
				{
					case VOLUME_CONTROL:
					{
						if (REC_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_RecMinVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_RecMinVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}
						else if (PLAY_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_PlayMinVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_PlayMinVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}
						/* Data stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 2);
						break;
					}
					default:
						/* STALL control pipe */
						USBD_SetStall(0);
				}
				// Trigger next Control Out DATA1 Transaction.
				/* Status stage */
				USBD_PrepareCtrlOut(0, 0);
				break;
			}
			case UAC_GET_MAX:
			{
				switch(buf[3])
				{
					case VOLUME_CONTROL:
					{
						if (REC_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_RecMaxVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_RecMaxVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}
						else if (PLAY_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_PlayMaxVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_PlayMaxVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}

						/* Data stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 2);
						break;
					}
					default:
						/* STALL control pipe */
						USBD_SetStall(0);
				}
				// Trigger next Control Out DATA1 Transaction.
				/* Status stage */
				USBD_PrepareCtrlOut(0, 0);
				break;
			}
			case UAC_GET_RES:
			{
				switch(buf[3])
				{
					case VOLUME_CONTROL:
					{
						if (REC_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_RecResVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_RecResVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}
						else if (PLAY_FEATURE_UNITID == buf[5])
						{
                                                  u32Temp = g_usbd_PlayResVolume;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = u32Temp;
                                                        u32Temp = g_usbd_PlayResVolume >> 8;
							M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = u32Temp;
						}
						/* Data stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 2);
						break;
					}
					default:
						/* STALL control pipe */
						USBD_SetStall(0);
				}
				// Trigger next Control Out DATA1 Transaction.
				/* Status stage */
				USBD_PrepareCtrlOut(0, 0);
				break;
			}
			default:
			{
				/* Setup error, stall the device */
				USBD_SetStall(0);
			}
		}
	}
	else
	{
        // Host to device
        switch(buf[1])
		{
			case UAC_SET_CUR:
			{
				switch(buf[3])
				{
					case MUTE_CONTROL:
						if (REC_FEATURE_UNITID == buf[5])
							USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecMute, buf[6]);
						else if (PLAY_FEATURE_UNITID == buf[5])
							USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayMute, buf[6]);
						/* Status stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 0);
						break;
					case VOLUME_CONTROL:
						if (REC_FEATURE_UNITID == buf[5])
						{
							if (buf[2] == 1) {
								/* Prepare the buffer for new record volume of left channel */
								USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecVolumeL, buf[6]);
							} else {
								/* Prepare the buffer for new record volume of right channel */
								USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecVolumeR, buf[6]);
							}
						}
						else if (PLAY_FEATURE_UNITID == buf[5])
						{
							if (buf[2] == 1) {
									/* Prepare the buffer for new play volume of left channel */
									USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayVolumeL, buf[6]);
							} else {
									/* Prepare the buffer for new play volume of right channel */
									USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayVolumeR, buf[6]);
							}
						}
						/* Status stage */
						USBD_SET_DATA1(EP0);
						USBD_SET_PAYLOAD_LEN(EP0, 0);
						break;
					default:
						/* STALL control pipe */
						USBD_SetStall(0);
						break;
				}
				break;
			}
			case HID_SET_REPORT:
			{
				if(buf[3] == 2)
				{
					/* Request Type = Output */
					USBD_SET_DATA1(EP1);
					USBD_SET_PAYLOAD_LEN(EP1, buf[6]);

					/* Status stage */
					USBD_PrepareCtrlIn(0, 0);
				}
				break;
			}
			case HID_SET_IDLE:
			{
				/* Status stage */
				USBD_SET_DATA1(EP0);
				USBD_SET_PAYLOAD_LEN(EP0, 0);
				break;
			}
			case HID_SET_PROTOCOL:
			default: {
				/* Setup error, stall the device */
				USBD_SetStall(0);
				break;
			}
        }
    }
}



/**
 * usb_i94xxx_ioctl()
 *
 * return:
 */
uint8_t usb_i94xxx_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
//	struct usb_i94xxx_cfg_t *cfg_hndl;
	uint32_t freq;

//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
	    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB14MFP_Msk |
	    		SYS_GPB_MFPH_PB15MFP_Msk);
	    SYS->GPB_MFPH |=  (SYS_GPB_MFPH_PB13MFP_USBD_DN |
	    		SYS_GPB_MFPH_PB14MFP_USBD_DP | SYS_GPB_MFPH_PB15MFP_USBD_VBUS);


		DEV_IOCTL_0_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_ENABLE);
		freq = 48000000;
		DEV_IOCTL_1_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_SET_FREQ, &freq);
		DEV_IOCTL_0_PARAMS(i94xxx_usb_clk_dev, CLK_IOCTL_ENABLE);


		//irq_register_device_on_interrupt(USBD_IRQn, adev);
		irq_register_interrupt(USBD_IRQn, USBD_IRQHandler);
		irq_set_priority(USBD_IRQn, OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(USBD_IRQn);
		break;

	case IOCTL_USB_DEVICE_SET_DEVICE_DESC :
		l_gsInfo.gu8DevDesc = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_CONFIG_DESC :
		l_gsInfo.gu8ConfigDesc = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_STRING_DESC :
		l_gsInfo.gu8StringDesc = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_HID_REPORT_DESC :
		l_gsInfo.gu8HidReportDesc = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_BOS_DESC :
		l_gsInfo.gu32BOSDesc = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_HID_REPORT_LEN :
		l_gsInfo.gu32HidReportSize = aIoctl_param1;
		break;
	case IOCTL_USB_DEVICE_SET_HID_DESC_INDEX :
		l_gsInfo.gu32ConfigHidDescIdx = aIoctl_param1;
		break;

	case IOCTL_USB_DEVICE_START :
		{
			uint32_t *check_pointer;

			check_pointer = (uint32_t *)&l_gsInfo;
			while (check_pointer <
					(uint32_t *)(((uint32_t)&l_gsInfo) + sizeof(l_gsInfo)))
			{
				if (NULL == (uint32_t *)(*check_pointer))
				{
					CRITICAL_ERROR("l_gsInfo structure should be initialized");
				}
				check_pointer++;
			}
		}

		/* usb initial */
		USBD_Open(&l_gsInfo, UAC_ClassRequest, NULL);
		/* Endpoint configuration */
		UAC_Init();


		/* use following code instead of USBD_Start() because USBD_Start uses
		* CLK_SysTickDelay function and it touches systick that is used in our
		* framework as OS tick
		*/
		{
			os_delay_ms(100);
			/* Disable software-disconnect function */
			USBD_CLR_SE0();

			/* Clear USB-related interrupts before enable interrupt */
			USBD_CLR_INT_FLAG(USBD_INT_BUS | USBD_INT_USB | USBD_INT_FLDET | USBD_INT_WAKEUP);

			/* Enable USB-related interrupts. */
			USBD_ENABLE_INT(USBD_INT_BUS | USBD_INT_USB | USBD_INT_FLDET | USBD_INT_WAKEUP | USBD_INTEN_SOFIEN_Msk);
		}
		break;
	case IOCTL_USB_DEVICE_SET_OUT_ENDPOINT_CALLBACK :
		{
			struct set_out_endpoint_callback_t *set_out_endpoint_callback;
			uint8_t endpoint_num;

			set_out_endpoint_callback =
					(struct set_out_endpoint_callback_t*)aIoctl_param1;
			endpoint_num = set_out_endpoint_callback->endpoint_num;
			if ( MAX_NUM_OF_ENDPOINTS <= endpoint_num )
			{
				CRITICAL_ERROR("illegal endpoint");
			}
			callback_functions[endpoint_num] = set_out_endpoint_callback->func;
			callback_devs[endpoint_num] =
					set_out_endpoint_callback->callback_dev;
		}
		break;
//	case IOCTL_USB_SET_ISR_CALLBACK_TX_DEV:
//		cfg_hndl->callback_tx_dev =(struct dev_desc_t *) aIoctl_param1;
//		break;
//
//	case IOCTL_USB_SET_ISR_CALLBACK_RX_DEV:
//		cfg_hndl->callback_rx_dev =(struct dev_desc_t *) aIoctl_param1;
//		break;



	default :
		return 1;
	}
	return 0;
}
