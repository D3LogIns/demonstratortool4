
#include "fat16_sd\sdi.h"
#include "fat16_sd\fat16_1.h"

#ifndef  DEBUG
#define  DEBUG  0
#endif


#define SPICS (1<<1)   //1 7 6 6
#define SPICLK (1<<7)   //5 6 7 21
#define SPIDATA (1<<6) 

#define RESET (1<<6) 


extern  Fat16RtDirInfo_Struct Globe_RtDirInfo;
extern  Fat16BPB_Struct  Globe_BPB;
extern Fat16Dir_Struct OTP_File;
extern CmdTable_Struct *LinkHead;
extern CmdTable_Struct *LinkCurrent;
extern volatile U8 RxBuffer[16];


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


void Adj_VCOMH(void)
{

     rGPHDAT |= RESET;               //RESET=H 
     Delay(10); 
     rGPHDAT &= ~RESET;              //RESET=L
     Delay(100); 
     rGPHDAT |= RESET;               //RESET=H
     Delay(10); 
   
 //set nMTP=1,in R1Eh
//power control (5)    
   SPI_Send_command(0x1e);
   SPI_Send_data(0x0266);

      
//**********************************************************
//vcom otp
   SPI_Send_command(0x28);
   SPI_Send_data(0x0006);
   
    
}




void OTP_Main()
{

#if DEBUG  
  Uart_Printf("am in otp main\n");
#endif

  Adj_VCOMH();
  LinkCurrent=LinkHead;
  link2char(LinkCurrent);
  Uart_Communication(RxBuffer);  

}

//-------------------------------------------------
//更改结束后发送烧写
//--------------------------------------------------
void  OTP_Send()
{


}

void  link2char(CmdTable_Struct *Link)
{
  
  RxBuffer[0]='R';  
  RxBuffer[1]=':';  
  RxBuffer[2]=itoc((Link->r)>>4);  
  RxBuffer[3]=itoc((Link->r)&0xf);  
  RxBuffer[4]='H';

  RxBuffer[5]=' ';  
  RxBuffer[6]=' ';  

  RxBuffer[7]='V';
  RxBuffer[8]=':';
  RxBuffer[9] =itoc((Link->value)>>12);  
  RxBuffer[10]=itoc(((Link->value)&0xf00)>>8); 
  RxBuffer[11]=itoc(((Link->value)&0xf0)>>4);  
  RxBuffer[12]=itoc((Link->value)&0xf); 
  
 
  RxBuffer[13]='H';  
  RxBuffer[14]=' ';
  RxBuffer[15]=' ';
   
}
//==字符转数据===============================
U8 atoi(U8 c)
{
	if((c>='0')&&(c<='9'))	//数字
	    c=(c-'0');
    else  if(((c>='a')&&(c<='f'))||    //字母a-f,或A-F:0x41,0x61
             ((c>='A')&&(c<='F')))
              {
               c=c%16;
               c=c+9;
               }
     return c;               

}
//==数据转字符(小写)===============================
U8 itoc(U8 c)
{
	if((c>9)&&c<16)	//数字
	    c=(c-10)+'a';     //小写字母
    else  c=c+'0';			//转数字字符0->'0'

    return c;               

}
