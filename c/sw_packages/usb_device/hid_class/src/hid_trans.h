/******************************************************************************
 * @file     hid_transfer.h
 * @brief    I94100 series USB HID transfer header file
 *
 * @note
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __HID_TRANS_H__
#define __HID_TRANS_H__

/* Define HID transfer buffer */
#define HIDTRANS_PAGESIZE        2048
#define HIDTRANS_SECTORSIZE      4096

/* Declare HID transfer APIs */
//void HIDTrans_Initiate(void);
//void HIDTrans_ClassRequest(void);

// HID transfer callback function.
void HIDTrans_EraseSector(uint32_t u32StartSector,uint32_t u32Sectors);
void HIDTrans_PrepareReadPage(uint32_t* pu32Address,uint32_t u32StartPage,uint32_t u32Pages);
void HIDTrans_PrepareWritePage(uint32_t* pu32Address,uint32_t u32StartPage,uint32_t u32Pages);
void HIDTrans_GetWriteData(uint32_t u32Address,uint32_t u32Pages);


/*!<Define HID Class Specific Request */
#define GET_REPORT          0x01
#define GET_IDLE            0x02
#define GET_PROTOCOL        0x03
#define SET_REPORT          0x09
#define SET_IDLE            0x0A
#define SET_PROTOCOL        0x0B

/*!<USB HID Interface Class protocol */
#define HID_NONE            0x00
#define HID_KEYBOARD        0x01
#define HID_MOUSE           0x02

/*!<USB HID Class Report Type */
#define HID_RPT_TYPE_INPUT      0x01
#define HID_RPT_TYPE_OUTPUT     0x02
#define HID_RPT_TYPE_FEATURE    0x03

#endif  

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
