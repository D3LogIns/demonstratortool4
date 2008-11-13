#include <string.h>
#include "..\common\addr.h"
#include "..\common\lib.h"


#define DEBUG 0

#define ADCPRS 39

//------newly added------------
#define TOUCH_MAX_X 	965//885  //950
#define TOUCH_MIN_X 	35//75  //90
#define TOUCH_MAX_Y 	921//892 //960
#define TOUCH_MIN_Y 	48//87  //50

#define TOUCH_X	 480    //240
#define TOUCH_Y	 272    //320

 int Touch_Screen_Flag;
 int Globe_Touch_Position_X;
 int Globe_Touch_Position_Y;
//extern int Test_touch_screen;
//extern volatile int PAUSE;
extern void Drawcircle(int x,int y,int r,int c);
extern void Glib_ClearScr(unsigned int c);
extern int H;
//------newly added------------
 
void Touch_CoordinateConversion(int *px, int *py)
{
	int TmpX, TmpY;
	int TmpX0, TmpY0;

	TmpX0 = *px; TmpY0 = *py;

    
   	TmpX = (*px >= TOUCH_MAX_X) ? (TOUCH_MAX_X) : *px;
	TmpY = (*py >= TOUCH_MAX_Y) ? (TOUCH_MAX_Y) : *py;
	
	TmpX -= TOUCH_MIN_X;
    TmpY -= TOUCH_MIN_Y;
    
    TmpX = (TmpX>0) ? TmpX : 0;
    TmpY = (TmpY>0) ? TmpY : 0;
    

	*px = (TmpX * TOUCH_X) / (TOUCH_MAX_X-TOUCH_MIN_X);	
	*py = (TOUCH_Y - (TmpY * TOUCH_Y) / (TOUCH_MAX_Y-TOUCH_MIN_Y));


	*px = (*px >= TOUCH_X)? (TOUCH_X - 1) : *px;
	*py = (*py >= TOUCH_Y)? (TOUCH_X - 1) : *py;
	
	return;
}

void __irq Adc_or_TsAuto(void)
{
    rINTSUBMSK|=(BIT_SUB_ADC|BIT_SUB_TC);	// Mask sub interrupt (ADC and TC) 
   
    if(rADCTSC&0x100)
    {
    Touch_Screen_Flag=1;
#if DEBUG    
	Uart_Printf("\nStylus Up!!\n");
#endif	
	rADCTSC&=0xff;	// Set stylus down interrupt
//--------------------------------------------------------------------------------------

	if(H!=9)   //非测试触摸屏间段,若误按则忽略;
	   {
#if  DEBUG	   
	   	 Uart_Printf("\n am in TP int,stylus down,PIC NO.is:%d \n",H); 
#endif	   	 
	   	 goto end;
	   }	   

    }
    else
    {   

#if DEBUG    
    Uart_Printf("\n am in interupt else\n");  
	Uart_Printf("\nStylus Down!!\n");
#endif
	
	// <Auto X-Position and Y-Position Read>
	rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
	// Stylus Down,Don't care,Don't care,Don't care,Don't care,XP pullup Dis,Auto,No operation

	rADCCON|=0x1;	// Start Auto conversion

	while(rADCCON & 0x1);		//check if Enable_start is low
	while(!(0x8000&rADCCON));	// Check ECFLG
	
//	Uart_Printf("X-Posion[AIN5] is %04d\n", (0x3ff&rADCDAT0));
//	Uart_Printf("Y-Posion[AIN7] is %04d\n", (0x3ff&rADCDAT1));
	//added 2008-4-28
	 Globe_Touch_Position_Y=0x3ff&rADCDAT0;  //changed 2008-4-29
	 Globe_Touch_Position_X=0x3ff&rADCDAT1;
	rADCTSC=(1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
   	// Stylus Up,Don't care,Don't care,Don't care,Don't care,XP pullup En,Normal,Waiting mode
   }
//--------------------------------------------------------------------------------------    
end:
    rSUBSRCPND|=BIT_SUB_TC;
    rINTSUBMSK=~(BIT_SUB_TC);	// Unmask sub interrupt (TC)     
    ClearPending(BIT_ADC);
//    Uart_Printf("88 interput,flag %d\n",Touch_Screen_Flag);
}
			

void Ts_Auto_initial(void)
{

#if DEBUG
    Uart_Printf("Touch Screen initial start.\n");
    Uart_Printf("Auto X/Y position conversion mode test\n");
#endif

    rADCDLY=(5000);	// ADC Start or Interval Delay50000

    rADCCON = (1<<14)|(ADCPRS<<6)|(0<<3)|(0<<2)|(0<<1)|(0);	
    // Enable Prescaler,Prescaler,AIN5/7 fix,Normal,Disable read start,No operation
    rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);//tark
    // Down,YM:GND,YP:AIN5,XM:Hi-z,XP:AIN7,XP pullup En,Normal,Waiting for interrupt mode

    pISR_ADC=(unsigned)Adc_or_TsAuto;
    rINTMSK&=~(BIT_ADC);			//开中断;增加&不屏蔽其他中断
    rINTSUBMSK&=~(BIT_SUB_TC);

#if DEBUG   
    Uart_Printf("Touch Screen initial end\n");
#endif
}

//-----------2008-4-28-----add touchscreen-----------
//************new added**************************************   	 
//******** Ts_Auto();*********************
//    |-------------------| 
//    |   1          2    |
//    |         3         |
//    |   4          5    |
//    |-------------------|   
void  Test_touchscreen(void)
{    
   int Test_Point[5][2]={ {TOUCH_X/4,TOUCH_Y/4},{TOUCH_X*3/4,TOUCH_Y/4},{TOUCH_X/2,TOUCH_Y/2},{TOUCH_X/4,TOUCH_Y*3/4},{TOUCH_X*3/4,TOUCH_Y*3/4}};
   int r=20,i;
   int rr,temp;
   int DOT_LEFT=5;

   rr=r*r;   
   Glib_ClearScr(0x0);
      
   for(i=0;i<5;i++)
     { 
      Drawcircle(Test_Point[i][0],Test_Point[i][1],r,0xff0000);
//      Drawcircle(120,68,20,0xffffffff);

#if DEBUG
      Uart_Printf("test point(%d , %d),& r:%d\n",Test_Point[i][0],Test_Point[i][1],r);
#endif
     
     }
     
#if DEBUG     
 Uart_Printf("draw finished\n");
#endif

while(DOT_LEFT)	
  {
    if(H!=9)goto end;  //如果按键改变值,就跳出
     
	 while(Touch_Screen_Flag)
	   {
	    Touch_Screen_Flag=0;

#if DEBUG	    
        Uart_Printf("touch position %4d  %4d\n",Globe_Touch_Position_X,Globe_Touch_Position_Y);
#endif
	    Touch_CoordinateConversion(&Globe_Touch_Position_X,&Globe_Touch_Position_Y);
#if DEBUG
        Uart_Printf("lcd position %4d  %4d\n",Globe_Touch_Position_X,Globe_Touch_Position_Y);
#endif
	 	for(i=0;i<5;i++)
	 	  {
			temp=(Globe_Touch_Position_X-Test_Point[i][0])*(Globe_Touch_Position_X-Test_Point[i][0])+(Globe_Touch_Position_Y-Test_Point[i][1])*(Globe_Touch_Position_Y-Test_Point[i][1]);
			if(temp<=rr) 
			    {
			     Drawcircle(Test_Point[i][0],Test_Point[i][1],r,0x0);	 	
//2008-8-1  为防止在已经消除的位置又按下去,把总点数变少了故做此改动,把该点移到屏外(2倍屏大小);
                 Test_Point[i][0]=2*TOUCH_X;
                 Test_Point[i][1]=2*TOUCH_Y;
			     DOT_LEFT--;			
#if DEBUG			          
			     Uart_Printf("dot left number:%d \n",DOT_LEFT);
#endif
			     }
	 	   }//end of for
	 	}//end of 2nd while
   }//end of 1st while
end:
    return;

}
//**********************************************************
