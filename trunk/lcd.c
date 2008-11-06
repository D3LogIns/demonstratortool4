#include <string.h>
#include "addr.h"
#include "lib.h"
#include "def.h"
#include "lcdlib.h"
#include "glib.h"
#include "lcd.h"

  


void Lcd_Port_Init(void);
void Lcd_Port_Return(void);

void __irq Lcd_Int_Frame(void);
void __irq Lcd_Int_Fifo(void);
void __irq Lcd_Int_Fifo_640480(void);

unsigned save_rGPCUP,save_rGPCDAT,save_rGPCCON;
unsigned save_rGPDUP,save_rGPDDAT,save_rGPDCON; 
unsigned lcd_count;

void Lcd_Port_Init(void)
{
    save_rGPCCON=rGPCCON;
    save_rGPCDAT=rGPCDAT;
    save_rGPCUP=rGPCUP;
    save_rGPDCON=rGPDCON;
    save_rGPDDAT=rGPDDAT;
    save_rGPDUP=rGPDUP;
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaaaaaa; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[23:8]
//    Uart_Printf("Initializing GPIO ports..........\n");
}

void Lcd_Port_Return(void)
{
    rGPCCON=save_rGPCCON;
    rGPCDAT=save_rGPCDAT;
    rGPCUP=save_rGPCUP;
    rGPDCON=save_rGPDCON;
    rGPDDAT=save_rGPDDAT;
    rGPDUP=save_rGPDUP;
}





void __irq Lcd_Int_Frame(void)
{
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    // GPG4 is .... 
    rGPGDAT&=(~(1<<4)); // GPG4=Low
    Delay(50); // GPG4=Low
    rGPGDAT|=(1<<4); //GPG4=High

    rLCDSRCPND=2; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=2; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(2)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}


//240320
void __irq Lcd_Int_Fifo(void)
{
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    if((lcd_count%20)==0) Uart_Printf("\n");
    Uart_Printf(".");
    lcd_count++;

    rLCDSRCPND=1; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=1; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(1)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}


//640480
void __irq Lcd_Int_Fifo_640480(void)
{
    int i;
    rLCDINTMSK|=3; // Mask LCD SUB Interrupt

    // GPG4 is .... 
    rGPGDAT&=(~(1<<4)); // GPG4=Low
    for(i=0;i<1;i++); // GPG4=Low
    rGPGDAT|=(1<<4); //GPG4=High

    rLCDSRCPND=1; // Clear LCD SUB Interrupt source pending
    rLCDINTPND=1; // Clear LCD SUB Interrupt pending
    rLCDINTMSK&=(~(1)); // Unmask LCD FRAME Interrupt
    ClearPending(BIT_LCD);
}
void Test_Lcd_Tft_16Bit_800480(void)
{/*
    int i,j,k;
    U32 *palette;
   //unsigned char palette;
 //   Lcd_Port_Init();
   // Lcd_Init(MODE_TFT_16BIT_800480);
   // Glib_Init(MODE_TFT_16BIT_800480);
  //  Lcd_PowerEnable(0, 1);
  //  Lcd_EnvidOnOff(1);
    
    
  //  rLCDCON5|=(1<<11); // 5:6:5 Palette Setting
    palette=(U32 *)PALETTE;
    for(i=0;i<256;i++)
	*palette++=DEMO256pal[i];
 
   */ 

}
void Test_Lcd_Tft_8Bit_800600(void)
{
   
}


