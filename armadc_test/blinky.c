#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
//#include "inc/lm4f232h5qd.h"
#include "buttons.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
//#include "inc/lm4f232h5qd.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "qs-logger.h"
#include "usbstick.h"
#include "inc/hw_gpio.h"
#include "inc/hw_hibernate.h"
#include "usbstick.h"
#include "utils/ustdlib.h"
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/adc.h"
#include "driverlib/hibernate.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "usblib/usblib.h"
#include "utils/ustdlib.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "qs-logger.h"
#include "acquire.h"
#include "inc/hw_gpio.h"
#include "inc/hw_hibernate.h"
#include "usbstick.h"
#include "flashstore.h"



#define CLOCK_RATE      100
#define MS_PER_SYSTICK (1000 / CLOCK_RATE)


#define USBMSC_DRIVE_RETRY      4



tLogRecord sRecord;
void led_init();
void init();
void sleep(int);
void led_on();
void led_off();
void ReadButton();
int mode=0;


volatile unsigned long g_ulTickCount;
volatile int runmode=0;
void SysTickIntHandler(void)
{
    g_ulTickCount++;

}


void ReadButton()
{

	static volatile unsigned long ulLastTickCount = 0;
    if(g_ulTickCount != ulLastTickCount)
    {
    unsigned char ucButtonState;
    unsigned char ucButtonChanged;




    ulLastTickCount = g_ulTickCount;
    ucButtonState = ButtonsPoll(&ucButtonChanged, 0);

    if(BUTTON_PRESSED(UP_BUTTON, ucButtonState, ucButtonChanged))
    {

    	led_on();
    	AcquireStart();
    	runmode=1;


    }
    if(BUTTON_PRESSED(DOWN_BUTTON, ucButtonState, ucButtonChanged))
    {

    	led_off();
    	runmode=0;
    	AcquireStop();

    }
    if(BUTTON_PRESSED(LEFT_BUTTON, ucButtonState, ucButtonChanged))
    {

        led_off();

    }
    if(BUTTON_PRESSED(RIGHT_BUTTON, ucButtonState, ucButtonChanged))
    {
    	//led_off();
    }
    }

}



void led_init()
{
//    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOG;
  //  GPIO_PORTG_DIR_R = 0x04;
   // GPIO_PORTG_DEN_R = 0x04;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
    ROM_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);

}

void led_on()
{
	ROM_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2,0x04);
	//GPIO_PORTG_DATA_R|=0x04;
}

void led_off()
{

	ROM_GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2,0);
}

volatile tUSBMode g_eCurrentUSBMode = USB_MODE_NONE;


unsigned long
GetTickms(void)
{
    unsigned long ulRetVal;
    unsigned long ulSaved;
    static unsigned long ulLastTick = 0;

    ulRetVal = g_ulTickCount;
    ulSaved = ulRetVal;

    if(ulSaved > ulLastTick)
    {
        ulRetVal = ulSaved - ulLastTick;
    }
    else
    {
        ulRetVal = ulLastTick - ulSaved;
    }

    //
    // This could miss a few milliseconds but the timings here are on a
    // much larger scale.
    //
    ulLastTick = ulSaved;

    //
    // Return the number of milliseconds since the last time this was called.
    //
    return(ulRetVal * MS_PER_SYSTICK);
}
void sysinit()
{
    //
     // Enable lazy stacking for interrupt handlers.  This allows floating-point
     // instructions to be used within interrupt handlers, but at the expense of
     // extra stack usage.
     //
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run from the PLL at 50MHz.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |SYSCTL_OSC_MAIN);

}

void usbinit()
{

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    ROM_GPIOPinConfigure(GPIO_PG4_USB0EPEN);
    ROM_GPIOPinTypeUSBDigital(GPIO_PORTG_BASE, GPIO_PIN_4);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

}

void init()
{

	sysinit();
	led_init();
	ButtonsInit();


    //
    // Enable the USB peripheral
    //
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);


	usbinit();

    ///unsigned long ulSysClock;






    //
        // Erratum workaround for silicon revision A1.  VBUS must have pull-down.

    if(CLASS_IS_BLIZZARD && REVISION_IS_A1)
    {
            HWREG(GPIO_PORTB_BASE + GPIO_O_PDR) |= GPIO_PIN_1;
    }


	g_ulTickCount = 0;
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet()/CLOCK_RATE);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();

    //USBStackModeSet(0, USB_MODE_HOST,0);

    //
    // Enable Interrupts
    //
//    ROM_IntMasterEnable();

    //USBStickInit();
	AcquireInit();

AcquireStart();
runmode=1;
    while(1)
    {

//    	ReadButton();
    	///USBStickRun();
    	if(runmode==1)
    	{
    	AcquireRun();
    	}


    }







}

//sleep in ms
void sleep(int ms)
{
    volatile unsigned long ulLoop=0;
	SysCtlDelay(ROM_SysCtlClockGet()*1000.0/(2*ms));
    /*for(ulLoop = 0; ulLoop < 200000; ulLoop++)
    {
    }*/
}

void debugled(int k)
{
	while(k-->0)
	{
	    led_on();
	    sleep(2000);
	    led_off();
	    sleep(2000);

	}
	sleep(4000);
}

int
main(void)
{
init();



}
