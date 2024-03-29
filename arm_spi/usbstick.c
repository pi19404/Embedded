//*****************************************************************************
//
// usbstick.c - Data logger module to handle USB mass storage.
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

#include <string.h>
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/rom.h"
#include "usblib/usblib.h"
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhmsc.h"
#include "utils/ustdlib.h"
#include "third_party/fatfs/src/ff.h"
#include "qs-logger.h"
#include "usbstick.h"
#include "stdio.h"

//*****************************************************************************
//
// This module manages the USB host mass storage function. It is used when
// there is a USB memory stick attached to the evaluation board.  It manages
// the USB connection and stores data log records to the attached mass
// storage device.
//
//*****************************************************************************

volatile static int first_write=0;
#define HCD_MEMORY_SIZE         128
unsigned char g_pHCDPool[HCD_MEMORY_SIZE];
//*****************************************************************************
//
// A line of text that is written to the start of a CSV file, to provide
// column headings.
//
//*****************************************************************************
static const char g_cCSVHeaderLine[] =
    "Time(s),Frac. seconds,"                                                  \
    "CH0(mV),CH1(mV),CH2(mV),CH3(mV),"                                        \
    "AccelX(.01g),AccelY(.01g),AccelZ(.01g),"                                 \
    "Ext. Temp(.1C),Int. Temp(.1C),Current(100uA)\r\n";

//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static FIL g_sFileObject;

//*****************************************************************************
//
// Holds global flags for the system.
//
//*****************************************************************************
static volatile int g_ulFlags = 0;

//*****************************************************************************
//
// Flag indicating that some USB device is connected.
//
//*****************************************************************************
#define FLAGS_DEVICE_PRESENT    0x00000001
#define FLAGS_FILE_OPENED       0x00000002

//*****************************************************************************
//
// Hold the current state for the application.
//
//*****************************************************************************


//*****************************************************************************
//
// The instance data for the MSC driver.
//
//*****************************************************************************
unsigned long g_ulMSCInstance = 0;

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the MSC class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_USBHostMSCClassDriver,
    &g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
static const unsigned long g_ulNumHostClassDrivers =
    sizeof(g_ppHostClassDrivers) / sizeof(tUSBHostClassDriver *);

//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.  In this application uDMA is only used for USB,
// so only the first 6 channels are needed.
//
//*****************************************************************************
#if defined(ewarm)
#pragma data_alignment=1024
tDMAControlTable g_sDMAControlTable[6];
#elif defined(ccs)
#pragma DATA_ALIGN(g_sDMAControlTable, 1024)
tDMAControlTable g_sDMAControlTable[6];
#else
tDMAControlTable g_sDMAControlTable[6] __attribute__ ((aligned(1024)));
#endif

//*****************************************************************************
//
// Initializes the file system module.
//
// \param None.
//
// This function initializes the third party FAT implementation.
//
// \return Returns \e true on success or \e false on failure.
//
//*****************************************************************************
extern void sleep(int ms);
extern void led_on();
extern void led_off();
extern void debugled(int);
static tBoolean
FileInit(void)
{
    //
    // Mount the file system, using logical disk 0.
    //


    if(f_mount(0,(FATFS *)(&g_sFatFs)) != FR_OK)
    {

        return(false);
    }


    return(true);
}

//*****************************************************************************
//
// This is the callback from the MSC driver.
//
// \param ulInstance is the driver instance which is needed when communicating
// with the driver.
// \param ulEvent is one of the events defined by the driver.
// \param pvData is a pointer to data passed into the initial call to register
// the callback.
//
// This function handles callback events from the MSC driver.  The only events
// currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
// the main routine to know when an MSC device has been detected and
// enumerated and when an MSC device has been removed from the system.
//
// \return None
//
//*****************************************************************************
static void
MSCCallback(unsigned long ulInstance, unsigned long ulEvent, void *pvData)
{
    //
    // Determine the event.
    //
    switch(ulEvent)
    {
        //
        // Called when the device driver has successfully enumerated an MSC
        // device.
        //
        case MSC_EVENT_OPEN:
        {
            //
            // Proceed to the enumeration state.
            //
            g_eState = STATE_DEVICE_ENUM;

            break;
        }

        //
        // Called when the device driver has been unloaded due to error or
        // the device is no longer present.
        //
        case MSC_EVENT_CLOSE:
        {
            //
            // Go back to the "no device" state and wait for a new connection.
            //
            g_eState = STATE_NO_DEVICE;

            //
            // Re-initialize the file system.
            //
            FileInit();

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    //
    // Process each kind of event
    //
    switch(pEventInfo->ulEvent)
    {
        //
        // An unknown device has been connected.
        //
        case USB_EVENT_UNKNOWN_CONNECTED:
        {
            //
            // An unknown device was detected.
            //
            g_eState = STATE_UNKNOWN_DEVICE;
            break;
        }

        //
        // The unknown device has been been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Unknown device has been removed.
            //
            g_eState = STATE_NO_DEVICE;
            break;
        }

        //
        // A bus power fault was detected.
        //
        case USB_EVENT_POWER_FAULT:
        {
            //
            // No power means no device is present.
            //
            g_eState = STATE_POWER_FAULT;
            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Initializes the USB stack for mass storage.
//
//*****************************************************************************
void
USBStickInit(void)
{
    //
    // Enable the uDMA controller and set up the control table base.
    // The uDMA controller is used by the USB library.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    ROM_uDMAEnable();
    ROM_uDMAControlBaseSet(g_sDMAControlTable);

    ROM_IntMasterEnable();
    //
    // Initially wait for device connection.
    //
    g_eState = STATE_NO_DEVICE;
    USBStackModeSet(0, USB_MODE_HOST, 0);
    //
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ulNumHostClassDrivers);

    //
    // Open an instance of the mass storage class driver.
    //
    g_ulMSCInstance = USBHMSCDriveOpen(0, MSCCallback);

    //
    // Initialize the power configuration. This sets the power enable signal
    // to be active high and does not enable the power fault.
    //
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

    //
    // Run initial pass through USB host stack.
    //
    USBHCDInit(0, g_pHCDPool, HCD_MEMORY_SIZE);

    //
    // Initialize the file system.
    //
    FileInit();


    //USBHCDMain();



}

//*****************************************************************************
//
// This is called by the application main loop to perform regular processing.
// It keeps the USB host stack running and tracks the state of the attached
// device.
//
//*****************************************************************************
void
USBStickRun(void)
{
    //
    // Call the USB stack to keep it running.
    //
    USBHCDMain();

    //
    // Take action based on the application state.
    //
    switch(g_eState)
    {
        //
        // A device has enumerated.
        //
        case STATE_DEVICE_ENUM:
        {
        	led_off();
            //
            // Check to see if the device is ready.  If not then stay
            // in this state and we will check it again on the next pass.
            //
            if(USBHMSCDriveReady(g_ulMSCInstance) != 0)
            {
                //
                // Wait about 500ms before attempting to check if the
                // device is ready again.
                //
                ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
                break;
            }

            //
            // If there were no errors reported, we are ready for
            // MSC operation.
            //

            g_eState = STATE_DEVICE_READY;

            //
            // Set the Device Present flag.
            //
            g_ulFlags = FLAGS_DEVICE_PRESENT;

            break;
        }

        //
        // If there is no device then just wait for one.
        //
        case STATE_NO_DEVICE:
        {
        	led_off();
            if(g_ulFlags == FLAGS_DEVICE_PRESENT)
            {
                //
                // Clear the Device Present flag.
                //
                g_ulFlags &= ~(FLAGS_DEVICE_PRESENT | FLAGS_FILE_OPENED);
            }
            break;
        }

        //
        // An unknown device was connected.
        //
        case STATE_UNKNOWN_DEVICE:
        {
        	led_off();
            //
            // If this is a new device then change the status.
            //
            if((g_ulFlags & FLAGS_DEVICE_PRESENT) == 0)
            {
                //
                // Unknown device is present
                //
            }

            //
            // Set the Device Present flag even though the unknown device
            // is not useful to us.
            //
            g_ulFlags = FLAGS_DEVICE_PRESENT;

            break;
        }

        //
        // The device is ready and in use.
        //
        case STATE_DEVICE_READY:
        {

            break;
        }

        //
        // Something has caused a power fault.
        //
        case STATE_POWER_FAULT:
        {
        	led_off();
            break;
        }

        //
        // Unexpected USB state.  Set back to default.
        //
        default:
        {
        	led_off();
            g_eState = STATE_NO_DEVICE;
            g_ulFlags &= ~(FLAGS_DEVICE_PRESENT | FLAGS_FILE_OPENED);
            break;
        }
    }
}

//*****************************************************************************
//
// This is called when the data logging is stopped.  It performs a sync
// to the file system which should flush any buffered data to the storage
// device.
//
//*****************************************************************************
void
USBStickCloseFile(void)
{
	first_write=0;
    f_close(&g_sFileObject);
    g_ulFlags &= ~FLAGS_FILE_OPENED;

}

//*****************************************************************************
//
// Create a file name for the file to be saved on the memory stick.
// This function uses an incrementing numerical search scheme to determine
// an available file name.  It tries opening file names in succession until
// it finds a file that does not yet exist.
// The file name will be of the form LOGnnnn.CSV.
// The caller supplies storage for the file name through the pcFilename
// parameter.
// The function will return 0 if successful and non-zero if a file name could
// not be found.
//
//*****************************************************************************

static int
CreateFileName(char *pcFilename, unsigned long ulLen)
{


	char file[100];
     FRESULT fresult;
    unsigned long ulFileNum = 0;

    // Enter loop to search for available file name
    //
    do
    {
        //
        // Prepare a numerical based file name and attempt to open it
        //



    	strcpy(file,"");


        snprintf(file,ulLen,"LOG%04d.CSV",ulFileNum);

        fresult = f_open((FIL *)&g_sFileObject,file, FA_OPEN_EXISTING);


        if(fresult ==FR_NO_FILE)
        {


        	strcpy(pcFilename,file);
            //
            // Return to caller, indicating that a file name has been found.
            //
            return(0);
        }


        //
        // Otherwise, advance to the next number in the file name sequence.
        //
        ulFileNum++;

    } while(ulFileNum < 1000);



    //
    // If we reach this point, it means that no useable file name was found
    // after attempting 10000 file names.
    //
    return(1);
}

//*****************************************************************************
//
// This is called at the start of logging to open a file on the storage
// device in preparation for saving data.  If no file name is specified, then
// a new file will be created.
//
// If a file name is specified, then that will be used instead of searching
// for an available file.  The file name that is passed in must be a maximum
// of 8 characters (9 including trailing 0) and represents the first part of
// the file name not including the extension.
//
// The function returns a pointer to the first part of the file name
// (without file extension).  It can be up to 8 characters (9 including the
// trailing 0).  If there is any error then a NULL pointer is returned.
//
//*****************************************************************************
char *
USBStickOpenLogFile(char *pcFilename8)
{
    FRESULT fresult;

    static char cFilename[16];
    unsigned long ulLen;

    //
    // Check state for ready device
    //
    g_ulFlags &= ~FLAGS_FILE_OPENED;
    if(g_eState == STATE_DEVICE_READY)
    {

        //
        // If a file name is specified then open that file
        //
        if(pcFilename8 && pcFilename8[0])
        {

            //
            // Copy the filename into local storage and cap at 8 characters
            // length.
            //
            memcpy(cFilename, pcFilename8, 8);
            cFilename[8] = 0;

            //
            // Find the actual length of the string (8 chars or less) so
            // we know where to add the extension.
            //
            ulLen = strlen(cFilename);

            //
            // Add the extension to the file name.
            //
            snprintf(&cFilename[ulLen], 5, ".CSV");
        }

        //
        // Otherwise no file name was specified so create a new one.
        //
        else
        {

            if(CreateFileName(cFilename, sizeof(cFilename)))
            {
                //
                // There was a problem creating a file name so return an error
                //
                return(0);
            }
        }

        //
        // Open the file by name that was determined above.  If the file exists
        // it will be opened, and if not it will be created.
        //
        fresult = f_open(&g_sFileObject, cFilename, FA_OPEN_ALWAYS | FA_WRITE);
        if(fresult != FR_OK)
        {
            return(0);
        }

        //
        // Since it is possible that the file already existed when opened,
        // seek to the end of the file so new data will be appended.  If this
        // is a new file then this will just be the beginning of the file.
        //
        fresult = f_lseek(&g_sFileObject, g_sFileObject.fsize);
        if(fresult != FR_OK)
        {
            return(0);
        }

        //
        // Set flag to indicate file is now opened.
        //
        g_ulFlags |= FLAGS_FILE_OPENED;

        //
        // If no file name was specified, then this is a new file so write a
        // header line with column titles to the CSV file.
        //
        if(!pcFilename8 || !pcFilename8[0])
        {
            //
            // Write a header line to the CSV file
            //
        	/*
            fresult = f_write(&g_sFileObject, g_cCSVHeaderLine,
                                  sizeof(g_cCSVHeaderLine),
                                  &usBytesWritten);
            if(fresult != FR_OK)
            {
                g_ulFlags &= ~FLAGS_FILE_OPENED;
                return(0);
            }
*/

            //
            // Since no file name was specified that means a file name was
            // created.  Terminate the new file name at the '.' separator
            // and return it to the caller.  We know that created file names
            // are always 7 characters.  Return the newly created file name
            // (the part before the '.')
            //
            cFilename[7] = 0;
            return(cFilename);
        }

        //
        // Otherwise, a file name was specified, so no need to write a
        // header row.  The caller's file name is unchanged so return the
        // same value back.
        //
        else
        {
            return(pcFilename8);
        }
    }

    else
    {
        //
        // Device not ready so return NULL.
        //
        return(0);
    }
}

//*****************************************************************************
//
// This is called each time there is a new data record to log to the storage
// device.  A line of text in CSV format will be written to the file.
//
//*****************************************************************************
int
USBStickWriteRecord(tLogRecord *pRecord)
{
    static char cBuf[256];

    unsigned long ulIdx;
    unsigned long ulBufIdx = 0;
    unsigned long ulRecordIdx;
    unsigned long ulSelected;
    FRESULT fresult;
    unsigned short usBytesWritten;

    //
    // Check the arguments
    //
    ASSERT(pRecord);
    if(!pRecord)
    {
        return(1);
    }


    //
    // Check state for ready device and opened file
    //
    if((g_eState != STATE_DEVICE_READY) ||!(g_ulFlags & FLAGS_FILE_OPENED))
    {
        return(1);
    }

    if(first_write==0)
    {
    	ulBufIdx=0;
    	strcpy(cBuf,"");
        ulSelected = pRecord->usItemMask;
        for(ulIdx = 0; ulIdx < NUM_LOG_ITEMS; ulIdx++)
        {
        	if(ulSelected & 1)
        	{
        		ulBufIdx+=snprintf(&cBuf[ulBufIdx],sizeof(cBuf)-ulBufIdx,"%s,",itemtext[ulIdx]);
        	}
            ulSelected >>= 1;
        }
        ulBufIdx += snprintf(&cBuf[ulBufIdx], sizeof(cBuf) - ulBufIdx, "\r\n");

        fresult = f_write(&g_sFileObject,cBuf,ulBufIdx,&usBytesWritten);
        if(fresult != FR_OK || (ulBufIdx!=usBytesWritten))
        {

             g_ulFlags &= ~FLAGS_FILE_OPENED;
             return(1);
        }

        first_write=1;
        return(0);
    }
	//led_on();
	//sleep(2000);
	///led_off();




    //
    // Iterate through selected data items and print to CSV buffer
    //
    ulRecordIdx = 0;
    ulSelected = pRecord->usItemMask;
    for(ulIdx = 0; ulIdx < NUM_LOG_ITEMS; ulIdx++)
    {
        //
        // If this data item is selected, then print a value to the CSV buffer
        //
        if(ulSelected & 1)
        {
            ulBufIdx += snprintf(&cBuf[ulBufIdx], sizeof(cBuf) - ulBufIdx,                                  ",%d", pRecord->sItems[ulRecordIdx]);
            ulRecordIdx++;
        }

        //
        // Otherwise, this column of data is not selected so emit just a comma
        //
        /*else
        {
            ulBufIdx += snprintf(&cBuf[ulBufIdx], sizeof(cBuf) - ulBufIdx, ",");
        }*/

        //
        // Next selected item ...
        //
        ulSelected >>= 1;
    }

    //
    // Append a CRLF to the end
    //
    ulBufIdx += snprintf(&cBuf[ulBufIdx], sizeof(cBuf) - ulBufIdx, "\r\n");

    //
    // Now write the entire buffer to the USB stick file
    //
    fresult = f_write(&g_sFileObject, cBuf, ulBufIdx, &usBytesWritten);

    //
    // Check for errors
    //
    if((fresult != FR_OK) || (usBytesWritten != ulBufIdx))
    {
        //
        // Some error occurred
        //
        g_ulFlags &= ~FLAGS_FILE_OPENED;
        return(1);
    }

    //
    // No errors occurred, return success
    //
    else
    {
        return(0);
    }
}
