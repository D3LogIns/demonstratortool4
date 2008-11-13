#include <string.h>
#include "..\common\addr.h"
#include "..\common\lib.h"
#include "..\common\def.h"
//#include "lcdlib.h"
#include "glib.h"

#include "lcdset.h"


void Lcd_Init(void)
{  
   	
	rLCDCON1=(CLKVAL<<8)|(MMODE<<7)|(PNRMODE<<5)|(BPPMODE<<1)|ENVID; 
	rLCDCON2=(VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW<<0); 
	rLCDCON3=(HBPD<<19)|(HOZVAL<<8)|(HFPD<<0);
	rLCDCON4=(MVAL)|(HSPW<<0);	
	rLCDCON5=(BPP24BL<<12)|(INVVCLK<<10)|(INVVLINE<<9)|(INVVFRAME<<8);
	
	rLCDSADDR1=(LCDBAND)<<21|(LCDBASEU)>>1;
	rLCDSADDR2=(LCDBASEL)>>1;
	rLCDSADDR3=(OFFSIZE)<<11|(PAGEWIDTH);
	
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
    rTPAL=0; // Disable Temp Palette
	
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
 //   rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
//    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}    
