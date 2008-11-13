/********************************************************************
*系统的中断服务函数集,初始化
*
*
********************************************************************/
#include "addr.h"
#include "lib.h"

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


//===================================================================
void Isr_Init(void)
{
    pISR_UNDEF  = (unsigned)HaltUndef;
    pISR_SWI    = (unsigned)HaltSwi;
    pISR_PABORT = (unsigned)HaltPabort;
    pISR_DABORT = (unsigned)HaltDabort;
    
    rINTMOD     = 0x0;                     //All=IRQ mode
//    rINTCON     =0x5;                      //Non-vectored,IRQ enable,FIQ disable    
    rINTMSK     = BIT_ALLMSK;              //All interrupt is masked.
    rINTSUBMSK  = BIT_SUB_ALLMSK;          //All sub-interrupt is masked. <- April 01, 2002 SOP

//    rINTSUBMSK  = ~(BIT_SUB_RXD0);         //Enable Rx0 Default value=0x7ff
//    rINTMSK     = ~(BIT_UART0);            //Enable UART0 Default value=0xffffffff    
    
//    pISR_UART0=(unsigned)RxInt;            //pISR_FIQ,pISR_IRQ must be initialized
}

//===================================================================