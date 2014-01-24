#include "acquire.h"
#include <string.h>
#include <stdlib.h>
#include "math.h"
#define CHAN_USER0      ADC_CTL_CH0
#define CHAN_USER1      ADC_CTL_CH1
#define CHAN_USER2      ADC_CTL_CH2
#define CHAN_USER3      ADC_CTL_CH3
#define CHAN_ACCELX     ADC_CTL_CH8

extern void led_on();
extern void led_off();
#define fc 22000
#define SAMPLING_FREQUENCY 100000
#define SAMPLING_TIME 1/SAMPLING_FREQUENCY
#define PULSE_TIME 0.002
int FILTER_SAMPLE=(int)(PULSE_TIME*SAMPLING_FREQUENCY)+1;
#define PI 3.1412
#define PING_DELAY 0.5
int PULSE_SAMPLE=(int)(PING_DELAY*SAMPLING_FREQUENCY);

int buffer_increment;//=floor(0.5*FILTER_SAMPLE);
int buffer_size;//floor(1.5*FILTER_SAMPLE);
int result_size;
int rem;
unsigned long *adcdata;
unsigned long *buffer[2],*tmp_buffer[2];
unsigned long *filter;
unsigned long *result;

volatile unsigned long max1;
volatile unsigned long max2;
int mode1=0;
int maxi1=0;
int maxi2=0;

int distance=0.10;
float lambda=1450/fc;
volatile int acount=0;
unsigned long k=0;
int buffer_index=0;

#define SEQUENCER 2
#undef FLASH_STORE

volatile unsigned long rcount=0;
extern void debugled(int);

int rindex=0;
int i,j;

int findex=0;
volatile unsigned long res=0;
volatile unsigned long res1=0;
unsigned long g_ulADCSeq[] =
{
    CHAN_USER0, CHAN_USER1,//, CHAN_USER2, CHAN_USER3,
};

#define NUM_ADC_CHANNELS (sizeof(g_ulADCSeq) / sizeof(unsigned long))


static char text[100];
unsigned long g_ulADCData[NUM_ADC_CHANNELS];
static volatile unsigned long ulSelectedMask=15;
static unsigned long g_ulNumItems;

static volatile unsigned long g_ulADCCount;
static volatile unsigned long ulLastADCCount = 0;
#define RECORD_SIZE (sizeof(tLogRecord) + ((NUM_LOG_ITEMS - 1) * 1))
/*static union
{
    unsigned long g_ulRecordBuf[(RECORD_SIZE + 2) / sizeof(unsigned long)];
    tLogRecord sRecord;
} g_sRecordBuf;
*/
volatile int ind2;
tContext sDisplayContext;
tRectangle sRect1,sRect2;


#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#define NUM_SSI_DATA 3
unsigned long ulDataTx[NUM_SSI_DATA];
#define NUM_SSI_DATA 3

void compute_filter()
{

	buffer_increment=floor(1*0.5*FILTER_SAMPLE);
	buffer_size=floor(1*1.5*FILTER_SAMPLE)-1;
	result_size=buffer_size+2*buffer_increment;

	//tmp_buffer[0]=(unsigned long*)malloc(buffer_size);
	//tmp_buffer[1]=(unsigned long*)malloc(buffer_size);
	buffer[0]=(unsigned long*)malloc(buffer_size*sizeof(unsigned long));
	buffer[1]=(unsigned long*)malloc(buffer_size*sizeof(unsigned long));
	filter=(unsigned long *)malloc(FILTER_SAMPLE*sizeof(unsigned long));
	result=(unsigned long *)malloc(result_size*sizeof(unsigned long));

	for(i=0;i<FILTER_SAMPLE;i++)
	{
			float t=-PULSE_TIME/2-1/SAMPLING_FREQUENCY;
			t=t+1/(FILTER_SAMPLE);
			float num=sin(PI*t/PULSE_TIME)*cos(PI*t/PULSE_TIME);
			float den=(PI*t/PULSE_TIME)*(1-(4*t*t/(PULSE_TIME*PULSE_TIME)));
			//float res=num/den;
//			if(i==((int)FILTER_SAMPLE/2))
			filter[i]=1;//res;
	//		else
		//	filter[i]=0;
	}

}


void compute_sum()
{

	int ii,jj;

	for(jj=0;jj<buffer_increment;jj++)
	{


	res=0;
	res1=0;

		for (ii=0;ii<FILTER_SAMPLE;ii++)
		{
			int ind3=ii+buffer_index+jj-buffer_increment+1;

			ind3=ind3%buffer_size;
			if(ind3<0)
			ind3=ind3+buffer_size;
			res=res+(buffer[0][ind3]);
			res1=res1+(buffer[1][ind3]);
		}
		res=res/(FILTER_SAMPLE);
		res1=res1/(FILTER_SAMPLE);
		if(res>max1)
		{
			max1=res;
			maxi1=rcount+jj;
		}

		if(res1>max2)
		{
			max2=res1;
			maxi2=rcount+j;
		}


	}

	buffer_index=(buffer_index+buffer_increment)%buffer_size;
	if(buffer_index<0)
	buffer_index=buffer_index+buffer_size;

}

void compute_sum1(int flag)
{
	int ii;
	static int jj=0;

	if(flag==1)
		jj=0;
	else
		jj++;
//	for(jj=0;jj<buffer_increment;jj++)
	{


	//res=0;
	res1=0;

		for (ii=0;ii<FILTER_SAMPLE;ii++)
		{
			int ind3=ii+buffer_index+jj-buffer_increment+1;

			ind3=ind3%buffer_size;
			if(ind3<0)
			ind3=ind3+buffer_size;
			res=res+(buffer[0][ind3]);
			res1=res1+(buffer[1][ind3]);
		}
		res=res/(FILTER_SAMPLE);
		res1=res1/(FILTER_SAMPLE);
		if(res>max1)
		{
			max1=res;
			maxi1=rcount+jj;
		}

		if(res1>max2)
		{
			max2=res1;
			maxi2=rcount+j;
		}


	}

	buffer_index=(buffer_index+buffer_increment)%buffer_size;
	if(buffer_index<0)
	buffer_index=buffer_index+buffer_size;
}


void ADC0SS0Handler(void)
{
    ROM_ADCIntClear(ADC0_BASE,SEQUENCER);
    ROM_ADCSequenceDataGet(ADC0_BASE,SEQUENCER,&g_ulADCData[0]);
    g_ulADCCount++;

}
int AcquireRun(void)
{
	unsigned long ulindex;

    if(g_ulADCCount != ulLastADCCount)
    {
    	ulLastADCCount=g_ulADCCount;



        if(ind2 <0)
        ind2=ind2+buffer_size;
        else
        ind2=ind2%buffer_size;
        buffer[0][ind2]=(unsigned long)(g_ulADCData[0]&0xfff);
        //max1=max1+buffer[0][ind2];
        buffer[1][ind2]=(unsigned long)g_ulADCData[1];
        //prev_index=ind2;
        ind2++;



        rcount++;

        if(0==0)
        {

        	   //
        	    // Initialize the data to send.
        	    //
        	    ulDataTx[0] = 's';
        	    ulDataTx[1] = 'p';
        	    ulDataTx[2] = 'i';

        	    //
        	    // Display indication that the SSI is transmitting data.
        	    //
  //      	    UARTprintf("Sent:\n  ");

        	    //
        	    // Send 3 bytes of data.
        	    //
        	    for(ulindex = 0; ulindex < NUM_SSI_DATA; ulindex++)
        	    {
        	        //
        	        // Display the data that SSI is transferring.
        	        //
        	        ///UARTprintf("'%c' ", ulDataTx[ulindex]);

        	        //
        	        // Send the data using the "blocking" put function.  This function
        	        // will wait until there is room in the send FIFO before returning.
        	        // This allows you to assure that all the data you send makes it into
        	        // the send FIFO.
        	        //
        	        SSIDataPutNonBlocking(SSI0_BASE, ulDataTx[ulindex]);
            	    /*while(SSIBusy(SSI0_BASE))
            	    {
            	    }*/

        	    }

        	    //
        	    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
        	    //

        }

        rem=rcount%((int)PULSE_SAMPLE);

    	if(rem==0)
    	{

    	    mode1=(mode1+1)%2;
    	    if(mode1==0)
    	    	led_on();
    	    else
    	    	led_off();


    	/*    int angle=0;
    	    if(max2>0 && max1>0 && abs(maxi1-maxi2)<PULSE_SAMPLE)
    	    {
    	    angle=abs(maxi1-maxi2);
    	    }

    	    acount++;


    	    max1=max1*20.0/4095;
    	    max2=max2*20.0/4095;
    	    //max1=max1/(2*buffer_increment);
    		snprintf(text,sizeof(text),"%ld,%ld",max1,max2);
*/

    	    /*GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    	    GrRectFill(&sDisplayContext, &sRect1);
    	    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    	    //GrRectDraw(&sDisplayContext, &sRect1);
    	    GrContextFontSet(&sDisplayContext, g_pFontCm12);
    	    GrStringDrawCentered(&sDisplayContext,text, -1,
    	                         GrContextDpyWidthGet(&sDisplayContext) / 2, 10, 0);
*/
    	    GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    	    GrRectFill(&sDisplayContext, &sRect2);
    	    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    	    sprintf(text,"%4d,%4d",rcount,g_ulADCCount);
    	    rcount=0;
    	    g_ulADCCount=0;
    	    GrContextFontSet(&sDisplayContext, g_pFontCm12/*g_pFontFixed6x8*/);
    	    GrStringDrawCentered(&sDisplayContext, text, -1,
    	                         GrContextDpyWidthGet(&sDisplayContext) / 2,
    	                         ((GrContextDpyHeightGet(&sDisplayContext) - 24) / 2) + 24,
    	                         0);
    	    GrFlush(&sDisplayContext);

    	    /*
    		max1=0;
    	    max2=0;
    	    res=0;
    	    res1=0;
    	    maxi1=0;
    	    maxi2=0;
    	    buffer_index=0;

    	    for(i=0;i<buffer_size;i++)
    	    {
        		buffer[0][i]=0;
    	    	buffer[1][i]=0;
    	    }
    	    i=0;
    	    j=0;
    	    ind2=buffer_index-buffer_increment+1;
    	    buffer_index=0;
    	    */
    	}

    }


}

//*****************************************************************************
//
// This function is called to start an acquisition running.  It determines
// which channels are to be logged, enables the ADC sequencers, and computes
// the first RTC match value.  This will start the acquisition running.
//
//*****************************************************************************
void AcquireStart()
{
    unsigned long ulIdx;

    ulIdx = ulSelectedMask;
    g_ulNumItems = 0;
    while(ulIdx)
    {
        if(ulIdx & 1)
        {
            g_ulNumItems++;
        }
        ulIdx >>= 1;
    }



    //USBStickOpenLogFile(0);

    ROM_ADCSequenceEnable(ADC0_BASE,SEQUENCER);
    //ROM_ADCSequenceDataGet(ADC0_BASE,SEQUENCER,&g_ulADCData[0]);
    ROM_ADCIntClear(ADC0_BASE,SEQUENCER);

    ROM_ADCIntEnable(ADC0_BASE,SEQUENCER);
    ROM_IntEnable(INT_ADC0SS2);

    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

    //ROM_IntEnable(INT_TIMER1A);
    //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerEnable(TIMER1_BASE, TIMER_A);
    ROM_IntMasterEnable();
}




//*****************************************************************************
//
// This function is called to stop an acquisition running.  It disables the
// ADC sequencers and the RTC match interrupt.
//
//*****************************************************************************



void
AcquireStop(void)
{

	tLogRecord pRecord;

	//pRecord.usItemMask = (unsigned short)15;
	//pRecord.sItems[0]=rcount;
	//pRecord.sItems[1]=rcount;
	//pRecord.sItems[2]=rcount;
	//pRecord.sItems[3]=rcount;
	//USBStickWriteRecord(&pRecord);
	//USBStickWriteRecord(&pRecord);

    // Disable ADC interrupts
    //
    ROM_IntDisable(INT_ADC0SS2);
    ROM_ADCSequenceDisable(ADC0_BASE, SEQUENCER);
    ROM_TimerDisable(TIMER0_BASE,TIMER_A);

    //ROM_IntMasterDisable();
   // ROM_IntDisable(INT_TIMER1A);
  //  ROM_TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//    ROM_TimerDisable(TIMER1_BASE, TIMER_A);


    //USBStickCloseFile();
}

//*****************************************************************************
//
// This function initializes the ADC hardware in preparation for data
// acquisition.
//
//*****************************************************************************
void
AcquireInit(void)
{
    unsigned long ulChan;

    //
    // Enable the ADC peripherals and the associated GPIO port
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    ROM_SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);

    ROM_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |GPIO_PIN_7 | GPIO_PIN_3);
    ROM_GPIOPinTypeADC(GPIO_PORTP_BASE, GPIO_PIN_0);

//    ROM_ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
    //ROM_ADCR

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    HWREG(GPIO_PORTB_BASE + GPIO_O_AMSEL) |= GPIO_PIN_6;
    ADCSequenceDisable(ADC0_BASE,SEQUENCER);

    ROM_ADCSequenceConfigure(ADC0_BASE, SEQUENCER, ADC_TRIGGER_TIMER, 0);

    for(ulChan = 0; ulChan < 2; ulChan++)
    {
        unsigned long ulChCtl;


        if (ulChan ==1)
        {

            ROM_ADCSequenceStepConfigure(ADC0_BASE, SEQUENCER, ulChan, ADC_CTL_CH1|ADC_CTL_IE | ADC_CTL_END);
        }
        else if(ulChan==0)
        {
        	ulChCtl = ADC_CTL_CH0;
        	ROM_ADCSequenceStepConfigure(ADC0_BASE, SEQUENCER, ulChan, ADC_CTL_CH0);
        }


    }

    ADCHardwareOversampleConfigure(ADC0_BASE, 1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    ROM_TimerConfigure(TIMER0_BASE,TIMER_CFG_32_BIT_PER );

    //100 micro
    unsigned long freq=SAMPLING_FREQUENCY;
    unsigned long period=(2*ROM_SysCtlClockGet()/(freq));

    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,period);
    ROM_TimerControlTrigger(TIMER0_BASE, TIMER_A, true);

    //ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet() /2);




    CFAL96x64x16Init();
    GrContextInit(&sDisplayContext, &g_sCFAL96x64x16);
    sRect1.sXMin = 0;
    sRect1.sYMin = 0;
    sRect1.sXMax = GrContextDpyWidthGet(&sDisplayContext) - 1;
    sRect1.sYMax = 23;

    sRect2.sXMin = 0;
    sRect2.sYMin = 23;
    sRect2.sXMax = GrContextDpyWidthGet(&sDisplayContext) - 1;
    sRect2.sYMax = GrContextDpyHeightGet(&sDisplayContext) - 1;


	snprintf(text,sizeof(text),"STart");


    GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    GrRectFill(&sDisplayContext, &sRect1);
    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    //GrRectDraw(&sDisplayContext, &sRect1);
    GrContextFontSet(&sDisplayContext, g_pFontCm12);
    GrStringDrawCentered(&sDisplayContext,text, -1,
                         GrContextDpyWidthGet(&sDisplayContext) / 2, 10, 0);

    //GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    //GrRectFill(&sDisplayContext, &sRect1);
    //GrContextForegroundSet(&sDisplayContext, ClrWhite);
    //GrRectDraw(&sDisplayContext, &sRect1);

    /*GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    GrRectFill(&sDisplayContext, &sRect2);
    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    GrRectDraw(&sDisplayContext, &sRect2);
*/
    compute_filter();
    maxi1=0;
    maxi2=0;
    max1=0;
    max2=0;
    res=0;
    res1=0;
    g_ulADCCount=0;
    buffer_index=0;
    ulLastADCCount=0;

}


void Timer1IntHandler(void)
{
	//debugled(10);
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //debugled(3);
    int angle=0;
    if(max2>0 && max1>0 && abs(maxi1-maxi2)<PULSE_SAMPLE)
    {
    //float dt=abs(maxi1-maxi2)/SAMPLING_FREQUENCY;
    //angle=asin(dt*lambda/distance)*180/3.1412;
    angle=abs(maxi1-maxi2);
    }
    //debugled(3);
    acount++;
    //rcount=0;
    //ROM_IntMasterDisable();

	snprintf(text,sizeof(text),"%3ld,%3ld",rcount,g_ulADCCount);


    GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    GrRectFill(&sDisplayContext, &sRect1);
    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    //GrRectDraw(&sDisplayContext, &sRect1);
    GrContextFontSet(&sDisplayContext, g_pFontCm12);
    GrStringDrawCentered(&sDisplayContext,text, -1,
                         GrContextDpyWidthGet(&sDisplayContext) / 2, 10, 0);

    GrContextForegroundSet(&sDisplayContext, ClrDarkBlue);
    GrRectFill(&sDisplayContext, &sRect2);
    GrContextForegroundSet(&sDisplayContext, ClrWhite);
    sprintf(text,"%4d,%4d",buffer_increment,rem);
    GrContextFontSet(&sDisplayContext, g_pFontCm12/*g_pFontFixed6x8*/);
    GrStringDrawCentered(&sDisplayContext, text, -1,
                         GrContextDpyWidthGet(&sDisplayContext) / 2,
                         ((GrContextDpyHeightGet(&sDisplayContext) - 24) / 2) + 24,
                         0);
//    GrFlush(&sDisplayContext);
    max1=0;
    max2=0;
    res=0;
    res1=0;
    maxi1=0;
    maxi2=0;
    buffer_index=0;
    for(i=0;i<buffer_size;i++)
    {
    	buffer[0][i]=0;
    	buffer[1][i]=0;
    }
    i=0;
    j=0;
    ind2=buffer_index-buffer_increment+1;




}
