//====================================================================
// File Name : 2410test.c
// Function  : S3C2410 Test Main Menu
// Program   : Shin, On Pil (SOP)
// Date      : June 13, 2003
// Version   : 0.0
// History
//   0.0 : Programming start (February 20,2002) -> SOP
//   1.0 (20020530) : First release for customer
//   1.1 (20020801) : Strata NOR Flash Added and etc. -> SOP
//   1.2 (20020930) : Added IIS Slave mode Test Menu. -> SOP
//       (20021208) : IIS Record & Play Test
//       (20030324) : Added K9S1208 Erase -> SOP
//       (20030331) : Added Test_Cache -> SOP
//       (20030613) : SOP
//====================================================================

#include <stdlib.h>
#include <string.h>

//Shin, On Pil  
#include "def.h"
#include "option.h"
#include "addr.h"
#include "lib.h"
#include "slib.h" 

#include "etc.h"

#include "int.h"

//#include "swi.h"

//#include "eint.h"
//#include "extdma.h"
#include "k9s1208.h"
#include "mmu.h"


#include "usbfifo.h"

//Shin, Jeong Seuk

#include "lcd.h"
#include "lcdlib.h"
#include "glib.h"
#include "palette.h"



#include "etc.h"
#include "flash.h"
//#include "idle.h"
//#include "pd6710.h"
#include "pll.h"
//#include "power.h"
//#include "pwr_c.h"
#include "PowerOff.h"

//================================================
#define s2   ((rGPBDAT&2)>>1)      //GPB1
#define s3   ((rGPHDAT&2)>>1)      //GPH1
#define s4   (rGPHDAT&1)           //GPH0
#define s5   ((rGPCDAT&(1<<5))>>5) //GPC5
//======================================================
//Jeon, Chan Sik
//#include "cache.h"
unsigned char LCD_code,LCD_data_mode,LCD_dots,BMP_start_block,BMP_block_number;
unsigned int LCD_x_size,LCD_y_size;
unsigned int *Image_data;
void Isr_Init(void);
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);
void Read_BPB(void);
void Display_logo(int);
//================================================
void Key_Intinit(void);

//==============================================
extern void Test_Uart1_Int(void);
extern void Uart_Init(int pclk,int baud);
extern unsigned char Data_buffer[0x200];
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
 
//===================================================================
void Main(void)
{
   int h=0;
 
    LCD_code=0;//MODE_TFT_16BIT_800480;
    Image_data=(unsigned int *)0x33000000;
  //  Led_Display(15); 
 //   Led_Display(1);
 //     Led_Display(2);
      Led_Display(4);
 //     Delay(100);
	//rGPFDAT=0x14;
	//Delay(100);
   // rGPFDAT=0x10;
   // Delay(100); 
   // rGPFDAT=0x14;
   // Delay(100);
     
     MMU_Init();
   
  
    
 // ChangeClockDivider(0,0);          // 1:1:1    
// ChangeClockDivider(0,1);          // 1:1:2    
//    rCLKDIVN |= (1<<2);                     // 1:4:4
//  ChangeClockDivider(1,0);          // 1:2:2 
   ChangeClockDivider(1,1);          // 1:2:4    

//   ChangeMPllValue(0xa1,0x3,0x3);    // FCLK=50.7MHz    
 //   ChangeMPllValue(0x7f,0x2,0x2);    // FCLK=101.25MHz
// ChangeMPllValue(0x96,0x5,0x1);    // FCLK=135.428571MHz
//    ChangeMPllValue(82,1,1);    //FCLK=180.0Mhz    
 //     ChangeMPllValue(0x2a,0x1,0x0);    // FCLK=200MHz   
  //   ChangeMPllValue(0x5c,0x1,0x1);    // FCLK=200MHz
  // ChangeMPllValue(0x5c,0x4,0x0);    // FCLK=200MHz
//  ChangeMPllValue(0x8e,0x7,0x0);    // FCLK=200MHz        
 // ChangeMPllValue(0xa1,0x3,0x1);    // FCLK=202.8MHz  
 ChangeMPllValue(0x66,0x1,0x1);    // FCLK=220MHz    xxxxx 
// ChangeMPllValue(0x69,0x1,0x1);    // FCLK=226MHz       
  //  ChangeMPllValue(0x7d,0x4,0x0);    // FCLK=266MHz        
      
      Port_Init();

///////////////////////////////以上为系统设定部分一般不改//////////////////////////////////
      
      Uart_Init(0,115200);    
      Uart_Select(0);
  //    Uart_Printf("yys1 ok.\n"); 
      
      Isr_Init();
     
 //  Lcd_Lpc3600Enable(); 
     Lcd_Port_Init();
     Init_Tcon();
     
     Lcd_Init(MODE_TFT_16BIT_800480);
      
      Glib_Init(MODE_TFT_16BIT_800480);
      Lcd_PowerEnable(0, 1);
      Lcd_EnvidOnOff(1);
      
      Key_Intinit();            //new added ，按键中断初始化      
    
  	 Test_SDI();
   	 Read_BPB();
   
   rr:     

    switch(h)
  {
     case 0://黑
           Glib_ClearScr(0x00000000, MODE_TFT_16BIT_800480);
	       Delay(10000);
           break;       
     case 1://白   
           Glib_ClearScr(0xFFFFFFFF, MODE_TFT_16BIT_800480);
           Delay(10000);
           break;       
     case 2://R
           Glib_ClearScr(0x00FF0000, MODE_TFT_16BIT_800480);
           Delay(10000);
           break;       
     case 3://G
           Glib_ClearScr(0x0000FF00, MODE_TFT_16BIT_800480);
           Delay(10000);
           break;           
     case 4://B
           Glib_ClearScr(0x000000FF, MODE_TFT_16BIT_800480);
           Delay(10000);
           break;      
     case 5://灰阶
           Glib_64jietiao();
           Delay(10000);
           break;      
     case 12://千鸟
           Glib_qianniao();
           Delay(10000);
           while(1);
           break;           
 /////////////////////////////////////////////////////////////// 
     case 7:
          Display_logo(1); 
          Delay(10000);
          break;
     case 8:
          Display_logo(2); 
          Delay(10000);
          break;
     case 9:
         Display_logo(3); 
         Delay(10000);
         break;     
     case 10:
         Display_logo(4); 
         Delay(10000);
         break;
     case 11:
         Display_logo(5); 
         Delay(10000);
         break;
     case 6:
         Display_logo(6); 
         Delay(10000);
         break;                                 
                    
    }
    //------------------------------------
       if(!s4)
       { Delay(5000);
         if(!s4)
          {
            Delay(10000);//Delay(10000);Delay(10000);
            while(1)
                {
                  if(!s4)
                    { Delay(5000);
                       if(!s4)
                          {
                           break; //break out while
                         }  
                     }     
                 }//end while
          }//end second if
       }    
  
    h++;
    h=h%13;
    //--------------------------------------------------
   
 goto rr;
 //   
  
}


//-******************************************
void Key_Intinit(void)
{  
  
 
  rGPHCON=rGPHCON&(~(0xf));         //S3,S4  (3210) //都设定为in口 (10) 
  
  //pull up
 rGPHUP=rGPHUP|(0x3);//  disable pull up

  
}

//********************************************************
//===================================================================
void Isr_Init(void)
{
    pISR_UNDEF  = (unsigned)HaltUndef;
    pISR_SWI    = (unsigned)HaltSwi;
    pISR_PABORT = (unsigned)HaltPabort;
    pISR_DABORT = (unsigned)HaltDabort;
    
    rINTMOD     = 0x0;                     //All=IRQ mode
//    rINTCON=0x5;                           //Non-vectored,IRQ enable,FIQ disable    
    rINTMSK     = BIT_ALLMSK;              //All interrupt is masked.
    rINTSUBMSK  = BIT_SUB_ALLMSK;          //All sub-interrupt is masked. <- April 01, 2002 SOP

//    rINTSUBMSK  = ~(BIT_SUB_RXD0);         //Enable Rx0 Default value=0x7ff
//    rINTMSK     = ~(BIT_UART0);            //Enable UART0 Default value=0xffffffff    
    
//    pISR_UART0=(unsigned)RxInt;            //pISR_FIQ,pISR_IRQ must be initialized
}

//===================================================================
void HaltUndef(void)
{
    Uart_Printf("Undefined instruction exception.\n");
    while(1);
}

//===================================================================
void HaltSwi(void)
{
    Uart_Printf("SWI exception.\n");
    while(1);
}

//===================================================================
void HaltPabort(void)
{
    Uart_Printf("Pabort exception.\n");
    while(1);
}

//===================================================================
void HaltDabort(void)
{
    Uart_Printf("Dabort exception.\n");
    while(1);
}


