/*****************************************
  NAME: lcdlib.c
  DESC: lcd configuration low-level codes
  HISTORY:
  2002.03.13:draft ver 0.0
 *****************************************/
#include <string.h>
#include "addr.h"
#include "lib.h"
#include "def.h"

#include "lcdlib.h"

//Mono STN
U32 (*frameBuffer1Bit)[SCR_XSIZE_STN/32];
//Gray
U32 (*frameBuffer2Bit)[SCR_XSIZE_STN/16];
U32 (*frameBuffer4Bit)[SCR_XSIZE_STN/8];
//Color STN
U32 (*frameBuffer8Bit)[SCR_XSIZE_CSTN/4];
U32 (*frameBuffer12Bit)[SCR_XSIZE_CSTN*3/8];
//TFT 240320
U32 (*frameBuffer8BitTft240320)[SCR_XSIZE_TFT_240320/4];
U32 (*frameBuffer16BitTft240320)[SCR_XSIZE_TFT_240320/2];
//TFT 640480
U32 (*frameBuffer1BitTft640480)[SCR_XSIZE_TFT_640480/32];
U32 (*frameBuffer8BitTft640480)[SCR_XSIZE_TFT_640480/4];
U32 (*frameBuffer16BitTft640480)[SCR_XSIZE_TFT_640480/2];
U32 (*frameBuffer24BitTft640480)[SCR_XSIZE_TFT_640480];
//TFT 800600
U32 (*frameBuffer1BitTft800600)[SCR_XSIZE_TFT_800600/32];
U32 (*frameBuffer8BitTft800600)[SCR_XSIZE_TFT_800600/4];
U32 (*frameBuffer16BitTft800600)[SCR_XSIZE_TFT_800600/2];
//TFT 800480
//I changed 2008-4-2
U32 (*frameBuffer16BitTft800480)[SCR_XSIZE_TFT_800480];


void SPI_Send_command(unsigned int data)			
{
    unsigned int i;
	 rGPBDAT &= ~SPICS; 				
	 
  rGPGDAT &= ~SPIDATA;    //0
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;	
  
  rGPGDAT &= ~SPIDATA;    //0
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;    //1  id
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT &= ~SPIDATA;    //0=command
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;

  rGPGDAT &= ~SPIDATA;    //0=write
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
	for( i = 0; i < 16; i++ )
	{ 
	 if ( ( data & 0x8000 ) != 0 )   //?10?
	   		rGPGDAT |= SPIDATA;
	   else rGPGDAT &= ~SPIDATA;
	//  Delay(1); 
	 rGPGDAT &= ~SPICLK;
	 Delay(10);
     rGPGDAT |= SPICLK;
	//   Delay(3);
	  data <<=1;//Delay(1);
	     
	}
   rGPBDAT |= SPICS;
}


void SPI_Send_data(unsigned int data)			//***char
{
    unsigned int i;
	 rGPBDAT &= ~SPICS; 				//***	
	 
  rGPGDAT &= ~SPIDATA;    //0
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;   //1
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;	
  
  rGPGDAT &= ~SPIDATA;    //0
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;    //1  id
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
   rGPGDAT |= SPIDATA;    //1=data
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;

  rGPGDAT &= ~SPIDATA;    //0=write
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;
  
	for( i = 0; i < 16; i++ )
	{ 
	 if ( ( data & 0x8000 ) != 0 )   //?10?
	   		rGPGDAT |= SPIDATA;
	   else rGPGDAT &= ~SPIDATA;
	//  Delay(1); 
	 rGPGDAT &= ~SPICLK;
	 Delay(10);
     rGPGDAT |= SPICLK;
	//   Delay(3);
	  data <<=1;//Delay(1);
	     
	}
   rGPBDAT |= SPICS;
}

/*
//3-wires (9 bits) ,commond=0,data=1
void SPI_Command()
{	
	
  rGPBDAT &= ~SPICS;   
  rGPGDAT &= ~SPIDATA;
  rGPGDAT &= ~SPICLK;
  Delay(10);	
  rGPGDAT |= SPICLK;

}


void SPI_Data()
{
  rGPBDAT &= ~SPICS;     //#define SPICS (1<<5)  #define SPICLK (1<<6) 
  rGPGDAT |= SPIDATA;    //#define SPIDATA (1<<7) 
  rGPGDAT &= ~SPICLK;
  Delay(10);
  rGPGDAT |= SPICLK;	

}


void SPI_Send_Data(unsigned int data)			//char
{
    unsigned int i;
	 rGPBDAT &= ~SPICS; 				//	
	 

	for( i = 0; i < 8; i++ )
	{ 
	 if ( ( data & 0x80 ) != 0 )   //?10?
	   		rGPGDAT |= SPIDATA;
	   else rGPGDAT &= ~SPIDATA;
	//  Delay(1); 
	 rGPGDAT &= ~SPICLK;
	 Delay(10);
     rGPGDAT |= SPICLK;
	//   Delay(3);
	  data <<=1;//Delay(1);
	     
	}
  rGPBDAT |= SPICS; 
}
*/



void Lcd_Init(int type)
{
    switch(type)
    {
    case MODE_TFT_16BIT_800480:
   
    frameBuffer16BitTft800480=(U32 (*)[SCR_XSIZE_TFT_800480])LCDFRAMEBUFFER;
	
	rLCDCON1=(CLKVAL_TFT_800480<<8)|(1<<7)|(3<<5)|(13<<1)|0;  // TFT LCD panel,16bpp TFT,ENVID=off	
	rLCDCON2=(VBPD_800480<<24)|(LINEVAL_TFT_800480<<14)|(VFPD_800480<<6)|(VSPW_800480<<0); 
	
	rLCDCON3=(HBPD_800480<<19)|(HOZVAL_TFT_800480<<8)|(HFPD_800480<<0);
	rLCDCON4=(13<<8)|(HSPW_800480<<0);
	
	rLCDCON5=(1<<11)|(1<<10)|(1<<9)|(1<<8);   //CLK上升沿取值(10),line(9) &frame(8) :both invert
	rLCDSADDR1=(((U32)frameBuffer16BitTft800480>>22)<<21)|M5D((U32)frameBuffer16BitTft800480>>1);
	//rLCDSADDR1=((0x33800000>>22)<<21)|(0);
    rLCDSADDR2=M5D( ((U32)frameBuffer16BitTft800480+(SCR_XSIZE_TFT_800480*LCD_YSIZE_TFT_800480*4))>>1 );
	//rLCDSADDR2=0x3384b000;
	//rLCDSADDR3=(((SCR_XSIZE_TFT_800480-LCD_XSIZE_TFT_800480)*2)<<11)|(LCD_XSIZE_TFT_800480*2);
	rLCDSADDR3=((SCR_XSIZE_TFT_800480-LCD_XSIZE_TFT_800480)*2<<11)|(LCD_XSIZE_TFT_800480*2);
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
    rTPAL=0; // Disable Temp Palette
    
	 /*	
    // 2008-3-25 这个配置是lq043t3dxxx的配置,做个备份----阿达
    	frameBuffer16BitTft800480=(U32 (*)[SCR_XSIZE_TFT_800480])LCDFRAMEBUFFER;
	rLCDCON1=(1<<8)|(1<<7)|(3<<5)|(13<<1)|0;  // TFT LCD panel,16bpp TFT,ENVID=off	
	rLCDCON2=(1<<24)|(543<<14)|(1<<6)|(9<<0); 
	
	rLCDCON3=(1<<19)|(959<<8)|(1<<0);
	rLCDCON4=(13<<8)|(40<<0);
	
	rLCDCON5=(1<<11)|(1<<9)|(1<<8);
	rLCDSADDR1=(((U32)frameBuffer16BitTft800480>>22)<<21)|M5D((U32)frameBuffer16BitTft800480>>1);
	//rLCDSADDR1=((0x33800000>>22)<<21)|(0);
    rLCDSADDR2=M5D( ((U32)frameBuffer16BitTft800480+(SCR_XSIZE_TFT_800480*LCD_YSIZE_TFT_800480*4))>>1 );
	//rLCDSADDR2=0x3384b000;
	//rLCDSADDR3=(((SCR_XSIZE_TFT_800480-LCD_XSIZE_TFT_800480)*2)<<11)|(LCD_XSIZE_TFT_800480*2);
	rLCDSADDR3=(0<<11)|(0); 
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
    rTPAL=0; // Disable Temp Palette
	*/
    break;


    }	
}


void Init_Tcon(void)
{
     rGPHDAT |= RESET;               //RESET=H 
     Delay(10); 
     rGPHDAT &= ~RESET;              //RESET=L
     Delay(100); 
     rGPHDAT |= RESET;               //RESET=H
     Delay(10); 
   
 
 //driver output control

   SPI_Send_command(0x1);
   SPI_Send_data(0x030f);   //030f

//lcd-driving-waveform control     
   SPI_Send_command(0x2);
   SPI_Send_data(0x0c02);
//power control (1)    
    SPI_Send_command(0x3);
   SPI_Send_data(0x040e);
   /*
//separate gamma (1)    
    SPI_Send_command(0x4);
    SPI_Send_data(0x0400);
    */
//Frame cycle control    
   SPI_Send_command(0x0b);
   SPI_Send_data(0xd000);
//power control (2)    
   SPI_Send_command(0x0c);
   SPI_Send_data(0x0005);
//power control (3)    
   SPI_Send_command(0x0d);
   SPI_Send_data(0x000f);
//power control (4)    
   SPI_Send_command(0x0e);
   SPI_Send_data(0x2e00);
//gate scan starting position    
/*
   SPI_Send_command(0x0f);
   SPI_Send_data(0x0000);

//double sorce amp       
   SPI_Send_command(0x15);
   SPI_Send_data(0x0090);  
   */
//pixel per line    
     SPI_Send_command(0x16);
   SPI_Send_data(0xef8e);      //ef8e
//vertical porch    
      SPI_Send_command(0x17);
   SPI_Send_data(0x0003);
//power control (5)    
    SPI_Send_command(0x1e);
   SPI_Send_data(0x0266);
   /*
//power control (5)    
    SPI_Send_command(0x1f);
   SPI_Send_data(0x1834);   
//Gamma Buffer biasing current    
     SPI_Send_command(0x26);
   SPI_Send_data(0x2800);
   

//second  booster freq
      SPI_Send_command(0x2b);
   SPI_Send_data(0x0520);
       
 //software shut(5) 
      SPI_Send_command(0x2d);
   SPI_Send_data(0x3f40);    

 //read sdo (5) 
   SPI_Send_command(0x2e);
   SPI_Send_data(0xb544);
   */
//**********************************************************    
//γ control (1)       
      SPI_Send_command(0x30);
   SPI_Send_data(0x0002);
//γ control (1)    
  SPI_Send_command(0x31);
   SPI_Send_data(0x0407);
//γ control (1)    
  SPI_Send_command(0x32);
   SPI_Send_data(0x0000);
//γ control (1)    
  SPI_Send_command(0x33);
   SPI_Send_data(0x0004);
//γ control (1)
   SPI_Send_command(0x34);
   SPI_Send_data(0x0707);

//γ control (1)
   SPI_Send_command(0x35);
   SPI_Send_data(0x0003);
//  γ control (1)  
  SPI_Send_command(0x36);
   SPI_Send_data(0x0407);
 //γ control (1)   
   SPI_Send_command(0x37);
   SPI_Send_data(0x0605);
 //γ control (2)   
   SPI_Send_command(0x3a);
   SPI_Send_data(0x1f00);
 //γ control (2)  
    SPI_Send_command(0x3b);
   SPI_Send_data(0x0000);
      
//**********************************************************
//vcom otp
   SPI_Send_command(0x28);
   SPI_Send_data(0x0006);
//vcom eq    
     SPI_Send_command(0x2a);
   SPI_Send_data(0x01d2);   
//software 8 color    
    SPI_Send_command(0x10);
   SPI_Send_data(0x02cc);
//new add    
    SPI_Send_command(0x26);
   SPI_Send_data(0x2800);
         
 //new add
   SPI_Send_command(0x15);
   SPI_Send_data(0x0090);
 
//second  booster freq  
   SPI_Send_command(0x2c);
   SPI_Send_data(0x3bbd);      //3bbd   
   

}




void Lcd_CstnOnOff(int onoff)
{
    // 1:CSTN Panel on  0:CSTN Panel off //	
    if(onoff==1)
    	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
    rGPBUP=rGPBUP|(1<<5); // Pull-up disable
    rGPBDAT=rGPBDAT&(~(1<<5))|(onoff<<5); // GPB5=On or Off
    rGPBCON=rGPBCON&(~(3<<10))|(1<<10);    //GPD9=output
}    


void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}    


void Lcd_Lpc3600Enable(void)
{
    rLPCSEL&=~(7);
    rLPCSEL|=(7); // 240320,Enable LPC3600
}       


void Lcd_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
    rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
    //Enable LCD POWER ENABLE Function
    rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}    


void MoveViewPort(int type)
{
   
}


void Lcd_MoveViewPort(int vx,int vy,int type)
{
   
      
}    
    
