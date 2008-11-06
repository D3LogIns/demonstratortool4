/*****************************************
  NAME: pll.c
  DESC: PLL & Maximum frequency test.
  HISTORY:
  03.21.2002:purnnamu: first release for S3C2410X
  09.16.2002:SOP: Test_PLL()
  09.24.2002:SOP: Test_PLL() 
 *****************************************/
#include <math.h>
#include <stdlib.h>

#include "def.h"
#include "option.h"
#include "addr.h"
#include "lib.h"
#include "slib.h" 
#include "pll.h"

#define FIN 	12000000


void Test_PLL(void)
{
    int temp;
    int i,j, mdiv, pdiv, sdiv, sval, fclk;    
    U32 mps[][4]={
	101250000,127,2,2, //FCLK,MDIV,PDIV,SDIV    <-- SOP 09/24/2002
	113000000,105,1,2, 
	118500000,150,2,2,
	124000000,116,1,2,
	135000000, 82,2,1,
	147000000, 90,2,1,
	152000000, 68,1,1,
	158000000, 71,1,1,
	170000000, 77,1,1,
	180000000, 82,1,1,
	186000000, 85,1,1,
	192000000, 88,1,1,
	202800000,161,3,1,
	214500000,135,2,1,
	220000000,102,1,1,
	226000000,105,1,1,
    237000000,150,2,1,
    };
    

    Uart_Printf("[ PLL frequency change test ]\n");
    Uart_TxEmpty(0);

    rLOCKTIME=((U32)(1807*0.8)<<12)|((U32)(1807*0.8)<<0); 
    	// 1807 is 150us at 12Mhz
    srand(0);

    for(j=0;j<30;j++)    //SOP 09/24/2002
    {
    	i=rand()/(float)RAND_MAX*(sizeof(mps)/16);
    	Uart_Printf("FCLK=%d MDIV=%xh,PDIV=%xh,SDIV=%xh\n",mps[i][0],mps[i][1],mps[i][2],mps[i][3]);
	    Uart_TxEmpty(0);    	

        ChangeClockDivider(1,1);        // 1:2:4    <-- SOP 09/24/2002        
	    ChangeMPllValue(mps[i][1],mps[i][2],mps[i][3]);
	    Uart_Init(mps[i][0]/4,115200);
	    Uart_Printf("...I'm running in changed FCLK...\n\n");
    }  
    
    ChangeClockDivider(1,1);          // 1:2:4        
    ChangeMPllValue(0xa1,0x3,0x1);    // FCLK=202.8MHz     <--- SOP 09/24/2002
    Uart_Init(0,115200);              // SOP 09/24/2002
 }


void ChangePLL(void)
{
    int pdiv, mdiv, sdiv, sval, fclk;

    Uart_Printf("[Running change test of M/P/S value]\n");

    Uart_Printf("Input M vlaue\n");
    mdiv=Uart_GetIntNum();        
    Uart_Printf("Input P vlaue\n");
    pdiv=Uart_GetIntNum();    
    Uart_Printf("Input S vlaue\n");
    sdiv=Uart_GetIntNum();
    sval=(int)pow(2,sdiv);
    fclk=( (mdiv+8)*FIN )/( (pdiv+2)*sval );

    Uart_Printf("FCLK=%d,M=0x%x,P=0x%x,S=0x%x\n",fclk,mdiv,pdiv,sdiv);
    Uart_Printf("Now change PLL value\n");
    Uart_TxEmpty(0);
    
    ChangeMPllValue(mdiv,pdiv,sdiv);
    Uart_Init((fclk/4),115200);         //Changed fclk ---> (fclk/4)   Sep. 16, 2002 SOP
    Uart_Printf("...I'm running in changed FCLK...\nPress any key to return!\n");
 }


void Test_PllOnOff(void)
{
    int i,j;
    unsigned int saveREFRESH;

    Uart_Printf("[PLL On/Off test]\n");
    Uart_TxEmpty(0);	//To avoid being crushed the character

    rLOCKTIME=((U32)(1807*0.8)<<12)|((U32)(1807*0.8)<<0); 
    	// 1807 is 150us at 12Mhz
    for(j=0;j<1000;j++)
    {
	rCLKSLOW=0|(1<<4)|(1<<5)|(1<<7); //FCLK=FIN/1,SLOW mode,MPLL=off,UPLL=on

    	saveREFRESH=rREFRESH;
    	rREFRESH=(1<<23)|(U32)(2048+1-12*15.6); //Trp=2clk,Trc=4clk
            //The other memory control register should be optimized for SLOW mode.
	
        rCLKSLOW=0|(1<<4)|(0<<5)|(1<<7);//PLL on,MPLL=on,UPLL=on
    	for(i=0;i<2048/4;i++); //S/W MPLL lock-time
    	rCLKSLOW=0|(0<<4)|(0<<5)|(1<<7);//NORMAL mode,PLL on,MPLL=on,UPLL=on
    
    	rREFRESH=saveREFRESH;
    	Uart_Printf(".");
	Uart_TxEmpty(0);	//To avoid being crushed the character    	
    }
}