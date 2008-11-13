//====================================================================
// File Name : Adc.c
// Function  : S3C2410 ADC Test 
// Program   : Kang, Weon Tark 
// Date      : May 22, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (March 29,2002) -> KWT
//         ADC Test                          -> May 15, 2002 SOP
//====================================================================
#include <string.h>
#include "addr.h"
#include "lib.h"
#include "vitest.h"
#include "def.h"

#define REQCNT 100              //May 08, 2002 SOP
#define ADC_FREQ 2500000		//采样频率2.5MHz
//#define ADC_FREQ   1250000
//#define LOOP 1
#define LOOP 10000


int ReadAdc(int ch);	        //Return type is int, Declare Prototype function
volatile U32 preScaler;

//==================================================================================
void Test_Adc(void)
{
    int i;
    int a0=0,a1=0; //Initialize variables
  
     preScaler = PCLK/ADC_FREQ -1;               //PCLK:50.7MHz,FCLK:202.8MHz
    //ADC conversion time = 5CYCLES*(1/(ADC Freq.)), ADC Freq. = PCLK/(ADCPSR+1)
    
     for(i=0;i<REQCNT;i++)                               //显示次数100次
     {
        a0+=ReadAdc(0);
        a1+=ReadAdc(1);

     }

	a0=a0/REQCNT;
	a1=a1/REQCNT;
	/*
	此处增加对v,i值得更新,转换
	
	*/
   
    Uart_Printf("a0:%04d a1:%04d \n",a0,a1);	        //May 08, 2002 SOP    
    
    rADCCON=(0<<14)|(19<<6)|(7<<3)|(1<<2);  //stand by mode to reduce power consumption	
    
}

//==================================================================================	
//ReadAdc  读取ch频道的值
//================================================================	
int ReadAdc(int ch)
{
    int i;
    static int prevCh=-1;

    rADCCON = (1<<14)|(preScaler<<6)|(ch<<3);	//setup channel

    if(prevCh!=ch)
    {
	rADCCON = (1<<14)|(preScaler<<6)|(ch<<3);   //setup channel
	for(i=0;i<LOOP;i++);	//delay to set up the next channel
	prevCh=ch;
    }
    
    rADCCON|=0x1;   //start ADC

    while(rADCCON & 0x1);	//check if Enable_start is low
    while(!(rADCCON & 0x8000));	//check if EC(End of Conversion) flag is high

    return ( (int)rADCDAT0 & 0x3ff );
}