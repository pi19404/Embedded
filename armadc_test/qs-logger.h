//*****************************************************************************
//
// qs-logger.h - Defines data types used in the data logger application.
//
// Copyright (c) 2011-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F232 Firmware Package.
//
//*****************************************************************************

#ifndef __QSLOGGER_H__
#define __QSLOGGER_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The following defines the order of data items to log.  It must match the
// order that they appear in the "CHANNELS" menu (see menus.c), and also
// the order they are stored in the ADC data buffer (see acquire.c).
//
//*****************************************************************************
#define LOG_ITEM_USER0      0
#define LOG_ITEM_USER1      1
#define LOG_ITEM_USER2      2
#define LOG_ITEM_USER3      3
#define LOG_ITEM_ACCELX     4
#define LOG_ITEM_ACCELY     5
#define LOG_ITEM_ACCELZ     6
#define LOG_ITEM_EXTTEMP    7
#define LOG_ITEM_INTTEMP    8
#define LOG_ITEM_CURRENT    9
#define NUM_LOG_ITEMS       10

static char *itemtext[] =
{
		{"User ADC0"},
		{"User ADC1"},
		{"User ADC2"},
		{"User ADC3"},
		{"ACCELX"},
		{"ACCELY"},
		{"ACCELZ"},
		{"EXTEMP"},
		{"INTEMP"},
		{"CURRENT"},
		{"NNUMBER"},
};

//*****************************************************************************
//
// These are additional definitions of items that may be displayed on the
// screen that are not acquired data.  These are used for updating dynamic
// text screens on the display.
//
//*****************************************************************************
#define TEXT_ITEM_STATUS1       10
#define TEXT_ITEM_STATUS2       11
#define TEXT_ITEM_STATUS3       12
#define TEXT_ITEM_STATUS_TITLE  13
#define TEXT_ITEM_DATE          14
#define TEXT_ITEM_TIME          15
#define NUM_TEXT_ITEMS          16

//*****************************************************************************
//
// A structure that defines a data record.  This is the binary format of the
// acquired data that will be stored.
//
//*****************************************************************************
typedef struct
{
    unsigned long ulSeconds;        // 32 bits of seconds
    unsigned short usSubseconds;    // 15 bits of subseconds (1 bit unused)
    unsigned short usItemMask;      // 16 bits means maximum 16 data items
    unsigned long sItems[4];
} tLogRecord;


//*****************************************************************************
//
// The values indicating which storage medium is to be used for logging data.
//
//*****************************************************************************
#define CONFIG_STORAGE_NONE   0
#define CONFIG_STORAGE_USB    1
#define CONFIG_STORAGE_HOSTPC 2
#define CONFIG_STORAGE_FLASH  3
#define CONFIG_STORAGE_VIEWER 4
#define CONFIG_STORAGE_CHOICES 5


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
