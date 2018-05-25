/******************************************************************************
 * @file     massstorage.h
 * @brief    I94100 series USB mass storage header file
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
 
#include "stdint.h" 
#include "I94100.h"
 
/* Define the vendor id and product id */
#define USBD_VID     0x0416
#define USBD_PID     0x8230

/* Define Descriptor information */
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  100  /* The unit is in 2mA. ex: 100 * 2mA = 200mA */

#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE+LEN_ENDPOINT*2)


/* Audio Interface Subclass Codes */
#define AUDIO_SUBCLASS_UNDEFINED               0x00
#define AUDIO_SUBCLASS_AUDIOCONTROL            0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING          0x02
#define AUDIO_SUBCLASS_MIDISTREAMING           0x03

/* Audio Interface Protocol Codes */
#define AUDIO_PROTOCOL_UNDEFINED               0x00

/*!<Define Audio information */
#define PLAY_RATE        192000      /* The audo play sampling rate. It could be 8000, 16000, 32000, and 48000 */
#define ALT1_PLAY_RATE   96000      
#define ALT2_PLAY_RATE   48000
#define ALT3_PLAY_RATE   44100
#define PLAY_CHANNELS    2           /* Number of channels. Don't Change */

#define REC_RATE            48000    /* The record sampling rate. Must be the same with PLAY_RATE */
#define REC_CHANNELS        2        /* Number of channels. Don't Change */
#define REC_CHANNELS_MONO   1
#define REC_CHANNELS_LR     2

/* Define Terminal and Unit ID  */
#define PLAY_IT_ID  				0x01
#define PLAY_OT_ID  				0x03
#define PLAY_FEATURE_UNITID   		0x06
#define REC_IT_ID   				0x04
#define REC_OT_ID   				0x02
#define REC_FEATURE_UNITID    		0x05

/* Define temp buffer length */
#define BUF_LEN     32*12
#define REC_LEN     REC_RATE / 1000

/* Define Descriptor information */
#if(PLAY_CHANNELS == 1)
    #define PLAY_CH_CFG     1
#endif
#if(PLAY_CHANNELS == 2)
    #define PLAY_CH_CFG     3
#endif

#if(REC_CHANNELS == 1)
    #define REC_CH_CFG     1
#endif
#if(REC_CHANNELS == 2)
    #define REC_CH_CFG     3
#endif

#define PLAY_RATE_LO    (PLAY_RATE & 0xFF)
#define PLAY_RATE_MD    ((PLAY_RATE >> 8) & 0xFF)
#define PLAY_RATE_HI    ((PLAY_RATE >> 16) & 0xFF)

#define REC_RATE_LO     (REC_RATE & 0xFF)
#define REC_RATE_MD     ((REC_RATE >> 8) & 0xFF)
#define REC_RATE_HI     ((REC_RATE >> 16) & 0xFF)

/***************************************************/
/*      Audio Class-Specific Request Codes         */
/***************************************************/
/*!<Define Audio Class Specific Request */
#define UAC_REQUEST_CODE_UNDEFINED  0x00
#define UAC_SET_CUR                 0x01
#define UAC_GET_CUR                 0x81
#define UAC_SET_MIN                 0x02
#define UAC_GET_MIN                 0x82
#define UAC_SET_MAX                 0x03
#define UAC_GET_MAX                 0x83
#define UAC_SET_RES                 0x04
#define UAC_GET_RES                 0x84
#define UAC_SET_MEM                 0x05
#define UAC_GET_MEM                 0x85
#define UAC_GET_STAT                0xFF
/*!<Define HID Class Specific Request */
#define HID_SET_REPORT              0x09
#define HID_SET_IDLE                0x0A
#define HID_SET_PROTOCOL            0x0B

#define MUTE_CONTROL                0x01
#define VOLUME_CONTROL              0x02

/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
// #define EP0_MAX_PKT_SIZE    64
// #define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
// #define EP2_MAX_PKT_SIZE    256
// #define EP3_MAX_PKT_SIZE    PLAY_RATE*PLAY_CHANNELS*2/1000

/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    64     
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
#define EP2_MAX_PKT_SIZE  0//  REC_RATE*REC_CHANNELS*2/1000
#define EP3_MAX_PKT_SIZE  0// now set dynamically PLAY_RATE*PLAY_CHANNELS*2/1000
#define EP4_MAX_PKT_SIZE  0// now set dynamically was 16
#define EP5_MAX_PKT_SIZE  0// now set dynamically was 16
#define EP6_MAX_PKT_SIZE    64 
#define EP7_MAX_PKT_SIZE    64 

/* Define EP buffer base and length */
#define SETUP_BUF_BASE      0
#define SETUP_BUF_LEN       8
#define EP0_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN         (EP0_MAX_PKT_SIZE)
#define EP1_BUF_BASE        (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN         (EP1_MAX_PKT_SIZE)
#define EP2_BUF_BASE        (EP1_BUF_BASE + EP1_BUF_LEN)
#define EP2_BUF_LEN         (EP2_MAX_PKT_SIZE)
#define EP3_BUF_BASE        (EP2_BUF_BASE + EP2_BUF_LEN)
#define EP3_BUF_LEN         (EP3_MAX_PKT_SIZE)
#define EP4_BUF_BASE        (EP3_BUF_BASE + EP3_BUF_LEN)
#define EP4_BUF_LEN         (EP4_MAX_PKT_SIZE)
#define EP5_BUF_BASE        (EP4_BUF_BASE + EP4_BUF_LEN)
#define EP5_BUF_LEN         (EP5_MAX_PKT_SIZE)
#define EP6_BUF_BASE        (EP5_BUF_BASE + EP5_BUF_LEN)
#define EP6_BUF_LEN         (EP6_MAX_PKT_SIZE)
#define EP7_BUF_BASE        (EP6_BUF_BASE + EP6_BUF_LEN)
#define EP7_BUF_LEN         (EP7_MAX_PKT_SIZE)


/* Define the interrupt In EP number */
#define ISO_IN_EP_NUM      0x02
#define ISO_OUT_EP_NUM     0x03
#define INT_IN_EP_NUM      0x06
#define INT_OUT_EP_NUM     0x07


/******************************************************************************/
/*                USBD Mass Storage Structure                                 */
/******************************************************************************/
/** @addtogroup M480_USBD_Mass_Exported_Struct M480 USBD Mass Exported Struct
  M480 USBD Mass Specific Struct
  @{
*/


static __INLINE uint32_t get_be32(uint8_t *buf)
{
    return ((uint32_t) buf[0] << 24) | ((uint32_t) buf[1] << 16) |
           ((uint32_t) buf[2] << 8) | ((uint32_t) buf[3]);
}



/*-------------------------------------------------------------*/
void DataFlashWrite(uint32_t addr, uint32_t size, uint32_t buffer);
void DataFlashRead(uint32_t addr, uint32_t size, uint32_t buffer);
void UAC_Init(void);
void UAC_ClassRequest(void);
void UAC_SetConfig(void);

/*-------------------------------------------------------------*/
void EP2_Handler(void);
void EP3_Handler(void);
void EP6_Handler(void);
void EP7_Handler(void);


/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
