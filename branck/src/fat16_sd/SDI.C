#include <stdio.h>
#include <string.h>
#include "..\common\option.h"
#include "..\common\addr.h"
#include "..\common\lib.h"
#include "sdi.h"

#define DEBUG 0

#define INICLK	400000//300000
#define NORCLK	5000000
//----------sd卡 读写模式设定---------------------
#define POL	0
#define INT	1
#define DMA	2
#define SD_RW_MODE  POL
//------------------------------------------------------------------
#define SD_MODE  0
#define MMC_MODE 1
int CARD_MODE=SD_MODE;
//------由CARD_MODE值来判断卡的类型----预设为:SD卡,初始判断后重新设定-

// Global variables
U32 *Tx_buffer=(U32 *)0x31000000;	//128[word]*16[blk]=8192[byte]=2048 words
U32 *Rx_buffer=(U32 *)0x31800000;	//128[word]*16[blk]=8192[byte]
//-------Tx_buffer 是向sd卡写入数据的缓冲区---最大可读写16个block--
//-------Tx_buffer 是从sd卡读出数据的缓冲区----即128word*16blk-----

volatile unsigned int rd_cnt;
volatile unsigned int wt_cnt;
volatile unsigned int block;
volatile unsigned int TR_end=0;

int Wide=0;       // 0:1bit, 1:4bit

volatile int RCA;
   int aa[128];
int CMD13(void);    // Send card status
int CMD9(void);
//-----------------------------
extern unsigned char Data_buffer[0x200];
//--------------------------------------------------------------------------
void Test_SDI(void)
{


    Uart_Printf("\n[SDI test]\n");
    
    if(!SD_card_init())
       {
         Uart_Printf("\n initial sd failed \n");
     	 return;
       }	 
    /*
    Tx_Buf_Init();

     block=5;   


  // SD_WriteBlock(0,3,Tx_buffer);
     SD_ReadBlock(0,1,aa);
 
    View_Rx_buf(1);
	
//-------------------MMC 模式-----------------------------    
    if(CARD_MODE==MMC_MODE)
      {
       Tx_Buf_Init();
	   Wt_Stream();
	   Rd_Stream();
	   View_Rx_buf(block);
      }
//-------------------MMC 模式-----------------------------    

    Card_sel_desel(0);	// Card deselect

    if(!CMD9())
	Uart_Printf("Get CSD fail!!!\n");
    rSDIDCON=0;//tark???
    rSDICSTA=0xffff;
    */
}

//-- Tx & Rx Buffer initialize
void Tx_Buf_Init(void)
{
 
    int i ;

    for(i=0;i<2048;i++)	     //128[word]*16[blk]=8192[byte]
	   *(Tx_buffer+i)=i;

 }


//-- Display Rx buffer ,此时Rx是从sd卡中读出来的数据了
void View_Rx_buf(int block_num)
{
     int i;
     
     Rx_buffer=(U32 *)0x31800000;
    
    Uart_Printf("Check Rx data\n");
    
    for(i=0;i<128*block_num;i++)     
        Uart_Printf("  %x",aa[i]);  
   
}


void View_Tx_buf(void)
{
    //-- Display Tx buffer 
    int i;

    for(i=0;i<2048;i++)
	Uart_Printf("TB[%02x]=%x,",Tx_buffer[i]);
}


//------------------ SD controller & card initialize ----------------------

BOOL SD_card_init(void)
{
    int i;

//--------------------------------------------------------
    RCA=0;

    CARD_MODE=SD_MODE;
    
    rGPEUP  = 0xf83f;     // The pull up
    rGPECON = 0xaaaaaaaa;
//--------------------------------------------------------
    /* Important notice for MMC test condition */
    /* Cmd & Data lines must be enabled pull up resister */

    rSDIPRE=PCLK/(2*INICLK)-1;	// 400KHz
//    rSDICON=(1<<4)|(1<<1)|1;	// Type B, FIFO reset, clk enable
//----阿达changed 2008-8-26---------------------------------------------
    rSDICON=(1<<1)|1;		// Type A, FIFO reset, clk enable  
    rSDIBSIZE=0x200;		// 512byte(128word),设定block的大小使得与一个扇区sector一样大
    rSDIDTIMER=0xffff;		// Set timeout count

    for(i=0;i<0x1000;i++);  // Wait 74SDCLK for MMC card

//Uart_Printf("rSDIRSP0=0x%x\n",rSDIRSP0);
    CMD0();
    
#if DEBUG    
    Uart_Printf("\nIn idle\n");
#endif    
//--------------------------------------------------------

//-- Check MMC card OCR---------------------------------------------
    if(Chk_MMC_OCR()) 
    {
#if DEBUG    
	  Uart_Printf("\nIn MMC ready\n");
#endif	  
	  CARD_MODE=MMC_MODE;
	  goto RECMD2;
    }

//--------------MMC check end!-----------------------------
    //-- Check SD card OCR
    if(Chk_SD_OCR()) 
       {
#if DEBUG       
        Uart_Printf("\nIn SD ready\n");
#endif        
        CARD_MODE=SD_MODE;
        } 
    else
       {
#if DEBUG       
	   Uart_Printf("\nInitialize fail\nNo Card assertion\n");
#endif	   
        return 0;
       }
//---------判段是何种卡:SD/MMC,或没有卡插入----------------       

RECMD2:
    //-- Check attaced cards, it makes card identification state
    rSDICARG=0x0;   // CMD2(stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x42; //lng_resp, wait_resp, start, CMD2

    //-- Check end of CMD2
    if(!Chk_CMDend(2, 1)) 
	goto RECMD2;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

#if DEBUG
    Uart_Printf("\nEnd id\n");
#endif

RECMD3:
    //--Send RCA
    rSDICARG=CARD_MODE<<16;	    // CMD3(MMC:Set RCA, SD:Ask RCA-->SBZ)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x43;	// sht_resp, wait_resp, start, CMD3

    //-- Check end of CMD3
    if(!Chk_CMDend(3, 1)) 
	goto RECMD3;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //--Publish RCA
    if(CARD_MODE==MMC_MODE) 
	   RCA=1;
    else 
	   RCA=( rSDIRSP0 & 0xffff0000 )>>16;

#if DEBUG	   
    Uart_Printf("RCA=0x%x\n",RCA);
#endif

    //--State(stand-by) check
    if( rSDIRSP0 & 0x1e00!=0x600 )  // CURRENT_STATE check
	goto RECMD3;
    
#if DEBUG    
    Uart_Printf("\nIn stand-by\n");
#endif
    
    rSDIPRE=PCLK/(2*NORCLK)-1;	// Normal clock=25MHz

    Card_sel_desel(1);	// Select

    if(CARD_MODE==SD_MODE)
	   Set_4bit_bus();
    else
	   Set_1bit_bus();

    return 1;
}

void Card_sel_desel(char sel_desel)
{
    //-- Card select or deselect
    if(sel_desel)
    {
RECMDS7:	
	rSDICARG=RCA<<16;	// CMD7(RCA,stuff bit)
	rSDICCON= (0x1<<9)|(0x1<<8)|0x47;   // sht_resp, wait_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 1))
	    goto RECMDS7;
	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	//--State(transfer) check
	if( rSDIRSP0 & 0x1e00!=0x800 )
	    goto RECMDS7;
    }
    else
    {
RECMDD7:	
	rSDICARG=0<<16;		//CMD7(RCA,stuff bit)
	rSDICCON=(0x1<<8)|0x47;	//no_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 0))
	    goto RECMDD7;
	//rSDICSTA=0x800;	// Clear cmd_end(no rsp)
    }
}

void __irq Rd_Int(void)
{
    U32 i,status;

    status=rSDIFSTA;
    
    if( (status&0x200) == 0x200 )	  // Check Last interrupt?
      {
	     for(i=(status & 0x7f)/4;i>0;i--)
			{
	          *Rx_buffer++=rSDIDAT;
	           rd_cnt++;
			}
      }
    else if( (status&0x80) == 0x80 )	// Check Half interrupt?
   		   {
      		  for(i=0;i<8;i++)
      			  {
    	 		   *Rx_buffer++=rSDIDAT;
	  			   rd_cnt++;
				  }
           }

    ClearPending(BIT_SDI);
}

void __irq Wt_Int(void)
{
    ClearPending(BIT_SDI);

    rSDIDAT=*Tx_buffer++;
    wt_cnt++;
    
}

void __irq DMA_end(void)
{
    ClearPending(BIT_DMA0);
    
    TR_end=1;
}

/*****************************************************
*功能:  读取指定其实块block_start后的block_num块数据
*入参:  block_start读的起始块数
* 		block_num需要读的块数量
*返回:  1成功,0失败
*****************************************************/
void SD_ReadBlock(U32 block_start,U32 block_num,U32 *read_buf)
{
 
    int status;

    rd_cnt=0;    

    rSDICON |= (1<<1);	// FIFO reset

RERDCMD:
//--------------------------------------------------------------------------
#if  SD_RW_MODE==POL    

       	rSDIDCON=(1<<19)|(1<<17)|(Wide<<16)|(2<<12)|(block_num<<0);
		// Rx after cmd, blk, 4bit bus, Rx start, blk num
		
        rSDICARG=(block_start<<9); //0x0;	// CMD17/18(addr)


	    if(block_num<2)	// SINGLE_READ
	    {
		    rSDICCON=(0x1<<9)|(0x1<<8)|0x51;      // sht_resp, wait_resp, dat, start, CMD17
	 	    if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
	 	    goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	    {
		     rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		    if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		    goto RERDCMD;
	    }

	    //rSDICSTA=0xa00;	                     // Clear cmd_end(with rsp)	    

	    while(rd_cnt<128*block_num)	             // 512*block bytes
	    {
		 if((rSDIDSTA&0x20)==0x20)         		 // Check timeout 
		    {
		    rSDIDSTA=0x1<<0x5;             		 // Clear timeout flag
		    break;
		    }
		    
		 status=rSDIFSTA;
		 if((status&0x1000)==0x1000)	    	 // Is Rx data?
		   {
		    *read_buf++=rSDIDAT;
		    rd_cnt++;
	//	   Uart_Printf("sd dat cnt,BLK NUM:%d,blkCnt:%d \n",rSDIDCNT&(0xfff<<12),rSDIDCNT&(0xfff));
		    
		    }
	    }

#endif
//--------------------------------------------------------------------------
#if  SD_RW_MODE==INT	    

	    rSDIDCON=(1<<19)|(1<<17)|(Wide<<16)|(2<<12)|(block_num<<0);
		// Rx after cmd, blk, 4bit bus, Rx start, blk num
        rSDICARG=(block_start<<9);	// CMD17/18(addr)

//--------中断初始设定-----start------------
	    pISR_SDI=(unsigned)Rd_Int;
	    rINTMSK = ~(BIT_SDI);
//--------中断初始设定-----end-------------	    
	    rSDIIMSK=5;	// Last & Rx FIFO half int.

	    if(block_num<2)	// SINGLE_READ
	    {
		  rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
		  if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
		     goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	     {
		   rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		   if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		    goto RERDCMD;
	     }
    
	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    while(rd_cnt<128*block_num);

	    rINTMSK |= (BIT_SDI);
	    rSDIIMSK=0;	// All mask
#endif
//--------------------------------------------------------------------------
#if  SD_RW_MODE==DMA

        rSDICARG=(block_start<<9);	// CMD17/18(addr)

	    pISR_DMA0=(unsigned)DMA_end;
	    rINTMSK = ~(BIT_DMA0);

	    rDISRC0=(int)(SDIDAT);	   // SDIDAT
	    rDISRCC0=(1<<1)+(1<<0);	   // APB, fix
	    rDIDST0=(U32)(read_buf);	// Rx_buffer
	    rDIDSTC0=(0<<1)+(0<<0);	    // AHB, inc
	    rDCON0=(1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(2<<24)|(1<<23)|(1<<22)|(2<<20)|128*block_num;
	    //handshake, sync PCLK, TC int, single tx, single service, SDI, H/W request, 
	    //auto-reload off, word, 128blk*num
	    rDMASKTRIG0=(0<<2)|(1<<1);    //no-stop, DMA2 channel on, no-sw trigger(0) 

	    rSDIDCON=(1<<19)|(1<<17)|(Wide<<16)|(1<<15)|(2<<12)|(block_num<<0);
		    // Rx after rsp, blk, 4bit bus, dma enable, Rx start, blk num
	    if(block_num<2)	// SINGLE_READ
	    {
			rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
			if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
		   		 goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	    {
			rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
			if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		  	    goto RERDCMD;
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    while(!TR_end);
		//Uart_Printf("rSDIFSTA=0x%x\n",rSDIFSTA);
	    rINTMSK |= (BIT_DMA0);
	    TR_end=0;
	    rDMASKTRIG0=(1<<2);	//DMA0 stop
#endif
//--------------------------------------------------------------------------	
    //-- Check end of DATA
    if(!Chk_DATend()) 
	Uart_Printf("dat error\n");
    rSDIDSTA=0x10;	// Clear data Tx/Rx end


    if(block_num>1)
    {
//--Stop cmd(CMD12)终止CMD18连续读命令    
RERCMD12:    
	rSDICARG=0x0;	    //CMD12(stuff bit)
	rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;//sht_resp, wait_resp, start, CMD12

	//-- Check end of CMD12
	if(!Chk_CMDend(12, 1)) 
	    goto RERCMD12;
	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    }
}


//======================================================================================
void Rd_Stream(void)	// only for MMC, 3blk read
{
    int status, rd_cnt=0;

    if(CARD_MODE==SD_MODE)
    {
	Uart_Printf("Stream read command supports only MMC!\n");
	return;
    }    
    Uart_Printf("\n[Stream read test]\n");
    
RECMD11:
    rSDIDCON=(1<<19)|(0<<17)|(0<<16)|(2<<12);

    rSDICARG=0x0;   // CMD11(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4b;   //sht_resp, wait_resp, dat, start, CMD11

    while(rd_cnt<128*block)
    {
	if( (rSDIDSTA&0x20) == 0x20 )
	{
	    Uart_Printf("Rread timeout error");
	    return ;
	}
	    
	status=rSDIFSTA;
	if((status&0x1000)==0x1000)
	{
	    //*Rx_buffer++=rSDIDAT;
	    //rd_cnt++;
	    Rx_buffer[rd_cnt++]=rSDIDAT;
	}
    }

    //-- Check end of CMD11
    if(!Chk_CMDend(11, 1)) 
	goto RECMD11;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //-- Check end of DATA
    rSDIDCON=(1<<19)|(0<<17)|(0<<16)|(1<<14)|(0<<12);
    while( rSDIDSTA&0x3 !=0x0 );
    if(rSDIDSTA!=0) 
	Uart_Printf("rSDIDSTA=0x%x\n", rSDIDSTA);
    rSDIDSTA=0xff;

STRCMD12:    
    //--Stop cmd(CMD12)
    rSDICARG=0x0;	    //CMD12(stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4c; //sht_resp, wait_resp, start, CMD12

    //-- Check end of CMD12
    if(!Chk_CMDend(12, 1)) 
	goto STRCMD12;

    Uart_Printf("\n--End stream read test\n");
}

//*****************************************************************************************
/*****************************************************
*功能:  把write_buf中的数据写入指定块block_start后的block_num块数据
*入参:  block_start读的起始块数
* 		block_num需要读的块数量
*返回:  1成功,0失败
*****************************************************/
void SD_WriteBlock(int block_start,int block_num,int *write_buf)
{

    int status;

    wt_cnt=0;    
    Uart_Printf("[Block write test]\n");


    rSDICON |= rSDICON|(1<<1);	// FIFO reset



REWTCMD:
//--------------------------------------------------------------------------
#if SD_RW_MODE==POL
	   rSDIDCON=(1<<20)|(1<<17)|(Wide<<16)|(3<<12)|(block_num<<0);
		    // Tx after rsp, blk, 4bit bus, Tx start, blk num

       rSDICARG=(block_start<<9);	      // CMD24/25(addr)

	    if(block_num<2)	// SINGLE_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x58;	//sht_resp, wait_resp, dat, start, CMD24
		if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;
	    }
	    else	// MULTI_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x59;	//sht_resp, wait_resp, dat, start, CMD25
		if(!Chk_CMDend(25, 1))	//-- Check end of CMD25
		    goto REWTCMD;	    
	    }

 	    
	    while(wt_cnt<128*block_num)
	    {
			status=rSDIFSTA;
			if((status&0x2000)==0x2000) 
				{
		 		   rSDIDAT=*write_buf++;
				    wt_cnt++;
				}
	    }

#endif	
//--------------------------------------------------------------------------
#if SD_RW_MODE==INT
    	rSDIDCON=(1<<20)|(1<<17)|(Wide<<16)|(3<<12)|(block_num<<0);
		    // Tx after rsp, blk, 4bit bus, Tx start, blk num

        rSDICARG=(block_start<<9);	    // CMD24/25(addr)
        
	    pISR_SDI=(unsigned)Wt_Int;
	    rINTMSK = ~(BIT_SDI);

	    rSDIIMSK=0x10;  // Tx FIFO half int.

	    if(block_num<2)	    // SINGLE_WRITE
	    {
	 	  rSDICCON=(0x1<<9)|(0x1<<8)|0x58;    //sht_resp, wait_resp, dat, start, CMD24
		  if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;
	    }
	    else	    // MULTI_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x59;    //sht_resp, wait_resp, dat, start, CMD25
		if(!Chk_CMDend(25, 1))	//-- Check end of CMD25 
		    goto REWTCMD;
	    }


	    while(wt_cnt<=128*block_num);//等待结束
	    //??	    rSDIDAT=*Tx_buffer;


	    rINTMSK |= (BIT_SDI);
	
	    rSDIIMSK=0;	// All mask	 
#endif
//--------------------------------------------------------------------------
#if  SD_RW_MODE==DMA

        rSDICARG=(block_start<<9);	    // CMD24/25(addr)

	    pISR_DMA0=(unsigned)DMA_end;
	    rINTMSK = ~(BIT_DMA0);

	    rDISRC0=(int)(write_buf);	    // Tx_buffer
	    rDISRCC0=(0<<1)+(0<<0);			// AHB, inc
	    rDIDST0=(U32)(SDIDAT);			// SDIDAT
	    rDIDSTC0=(1<<1)+(1<<0);			// APB, fix
	    rDCON0=(1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(2<<24)+(1<<23)+(1<<22)+(2<<20)+128*block_num;
	    //handshake, sync PCLK, TC int, single tx, single service, SDI, H/W request, 
	    //auto-reload off, word, 128blk*num
	    rDMASKTRIG0=(0<<2)+(1<<1)+0;    //no-stop, DMA0 channel on, no-sw trigger
	    
	    rSDIDCON=(1<<20)|(1<<17)|(Wide<<16)|(1<<15)|(3<<12)|(block_num<<0);
		    // Tx after rsp, blk, 4bit bus, dma enable, Tx start, blk num
	    if(block_num<2)	    // SINGLE_WRITE
	    {
		  rSDICCON=(0x1<<9)|(0x1<<8)|0x58;    //sht_resp, wait_resp, dat, start, CMD24
		  if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;	    
	    }
	    else	    // MULTI_WRITE
	    {
		  rSDICCON=(0x1<<9)|(0x1<<8)|0x59;    //sht_resp, wait_resp, dat, start, CMD25
		  if(!Chk_CMDend(25, 1))	//-- Check end of CMD25 
		    goto REWTCMD;	    
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    while(!TR_end);		
	    rINTMSK |= (BIT_DMA0);
	    TR_end=0;
	    rDMASKTRIG0=(1<<2);	//DMA0 stop

#endif
//--------------------------------------------------------------------------    
    //-- Check end of DATA
    if(!Chk_DATend()) 
	    Uart_Printf("dat error\n");

    rSDIDSTA=0x10;	// Clear data Tx/Rx end

    if(block_num>1)
    {
	//--Stop cmd(CMD12)
REWCMD12:    
	rSDIDCON=(1<<18)|(1<<17)|(0<<16)|(1<<12)|(block<<0);
	
	rSDICARG=0x0;	    //CMD12(stuff bit)
	rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;    //sht_resp, wait_resp, start, CMD12

	//-- Check end of CMD12
	if(!Chk_CMDend(12, 1)) 
	    goto REWCMD12;
 
	//-- Check end of DATA(with busy state)
	if(!Chk_BUSYend()) 
	    Uart_Printf("error\n");
	rSDIDSTA=0x08;
    }
}

//*****************************************************************************************

void Wt_Stream(void)	// only for MMC, 3blk write
{
    int status, wt_cnt=0;

    if(CARD_MODE==SD_MODE)
    {
	Uart_Printf("Stream write command supports only MMC!\n");
	return;
    }
    Uart_Printf("\n[Stream write test]\n");
RECMD20:
    rSDIDCON=(1<<20)|(0<<17)|(0<<16)|(3<<12);  // stream mode

    rSDICARG=0x0;	// CMD20(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x54;    //sht_resp, wait_resp, dat, start, CMD20

    //-- Check end of CMD25
    if(!Chk_CMDend(20, 1)) 
	goto RECMD20;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    while(wt_cnt<128*block)
    {
	status=rSDIFSTA;
	if((status&0x2000)==0x2000) 
	    rSDIDAT=Tx_buffer[wt_cnt++];
    }

    //-- Check end of DATA
    while( rSDIFSTA&0x400 );
    Delay(10);    // for the empty of DATA line(Hardware)
    rSDIDCON=(1<<20)|(0<<17)|(0<<16)|(1<<14)|(0<<12);
    while( (rSDIDSTA&0x3)!=0x0 );
    if(rSDIDSTA!=0) 
	Uart_Printf("rSDIDSTA=0x%x\n", rSDIDSTA);
    rSDIDSTA=0xff;

STWCMD12:    
    //--Stop cmd(CMD12)
    rSDIDCON=(1<<18)|(1<<17)|(0<<16)|(1<<12);

    rSDICARG=0x0;	    //CMD12(stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;   //sht_resp, wait_resp, start, CMD12

    //-- Check end of CMD12
    if(!Chk_CMDend(12, 1)) 
	goto STWCMD12;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //-- Check end of DATA(with busy state)
    if(!Chk_BUSYend()) 
	Uart_Printf("error\n");
    rSDIDSTA=0x08;
#if DEBUG
    Uart_Printf("\n--End Stream write test\n");
#endif    
}

//=======一下是命令实现==============================================================================
int Chk_CMDend(int cmd, int be_resp)
//0: Timeout
{
    int finish0;

    if(!be_resp)    // No response
      {
    	finish0=rSDICSTA;
	    while((finish0&0x800)!=0x800)	// Check cmd end
	         finish0=rSDICSTA;

	     rSDICSTA=finish0;// Clear cmd end state
	     return 1;
      }
    else	// With response
     {
    	finish0=rSDICSTA;
	    while( !( ((finish0&0x200)==0x200) | ((finish0&0x400)==0x400) ))    // Check cmd/rsp end
    	    finish0=rSDICSTA;

	    if(cmd==1 | cmd==9 | cmd==41)	// CRC no check
		  {
	         if( (finish0&0xf00) != 0xa00 )  // Check error
	            {
			    	rSDICSTA=finish0;   // Clear error state

					if(((finish0&0x400)==0x400))
		  			  return 0;	// Timeout error
    	  		}
	  	     rSDICSTA=finish0;	// Clear cmd & rsp end state
		 }
	   else	// CRC check
		{
	      if( (finish0&0x1f00) != 0xa00 )	// Check error
	         {
#if DEBUG	         
		    	Uart_Printf("CMD%d:rSDICSTA=0x%x, rSDIRSP0=0x%x\n",cmd, rSDICSTA, rSDIRSP0);
#endif
				rSDICSTA=finish0;   // Clear error state

				if(((finish0&0x400)==0x400))
		 		   return 0;	// Timeout error
    	     }
	      rSDICSTA=finish0;
	   }//end of 2nd else
	 return 1;
    }//end of first else
}
//--------------------------------------------
int Chk_DATend(void)
{
    int finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) ))	
	// Chek timeout or data end
	finish=rSDIDSTA;

    if( (finish&0xfc) != 0x10 )
    {
#if DEBUG    
        Uart_Printf("DATA:finish=0x%x\n", finish);
#endif        
        rSDIDSTA=0xec;  // Clear error state
        return 0;
    }
    return 1;
}
//--------------------------------------------
int Chk_BUSYend(void)
{
    int finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x08)==0x08) | ((finish&0x20)==0x20) ))
	finish=rSDIDSTA;

    if( (finish&0xfc) != 0x08 )
    {
#if DEBUG    
        Uart_Printf("DATA:finish=0x%x\n", finish);
#endif        
        rSDIDSTA=0xf4;  //clear error state
        return 0;
    }
    return 1;
}

void CMD0(void)
{
    //-- Make card idle state 
    rSDICARG=0x0;	    // CMD0(stuff bit)
    rSDICCON=(1<<8)|0x40;   // No_resp, start, CMD0

    //-- Check end of CMD0
    Chk_CMDend(0, 0);
    //rSDICSTA=0x800;	    // Clear cmd_end(no rsp)
}

int Chk_MMC_OCR(void)
{
    int i;

    //-- Negotiate operating condition for MMC, it makes card ready state
    for(i=0;i<15;i++)
    {
    	rSDICARG=0xffc000;	    	    //CMD1(OCR:2.6V~3.6V)
//	rSDICARG=0xff8000;	    	    //CMD1(OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x41;    //sht_resp, wait_resp, start, CMD1

    	//-- Check end of CMD1
    	if(Chk_CMDend(1, 1) & rSDIRSP0==0x80ffc000) 
//	if(Chk_CMDend(1, 1) & rSDIRSP0==0x80ff8000) 
	{
	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    return 1;	// Success
	}
    }
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}

int Chk_SD_OCR(void)
{
    int i;

    //-- Negotiate operating condition for SD, it makes card ready state
    for(i=0;i<15;i++)
    {
    	CMD55();    // Make ACMD

    	rSDICARG=0xff8000;	//ACMD41(OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x69;//sht_resp, wait_resp, start, ACMD41

	//-- Check end of ACMD41
    	if( Chk_CMDend(41, 1) & rSDIRSP0==0x80ff8000 ) 
	{
	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    return 1;	// Success	    
	}
	Delay(200); // Wait Card power up status
    }
    //Uart_Printf("SDIRSP0=0x%x\n",rSDIRSP0);
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}

int CMD55(void)
{
    //--Make ACMD
    rSDICARG=RCA<<16;			//CMD7(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x77;	//sht_resp, wait_resp, start, CMD55

    //-- Check end of CMD55
    if(!Chk_CMDend(55, 1)) 
	return 0;

    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

int CMD13(void)//SEND_STATUS
{
    int response0;

    rSDICARG=RCA<<16;			// CMD13(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4d;	// sht_resp, wait_resp, start, CMD13

    //-- Check end of CMD13
    if(!Chk_CMDend(13, 1)) 
	return 0;
    //Uart_Printf("rSDIRSP0=0x%x\n", rSDIRSP0);
    if(rSDIRSP0&0x100)
	Uart_Printf("Ready for Data\n");
    else 
	Uart_Printf("Not Ready\n");
    response0=rSDIRSP0;
    response0 &= 0x3c00;
    response0 = response0 >> 9;

#if DEBUG    
    Uart_Printf("Current Status=%d\n", response0);
#endif

    if(response0==6)
	Test_SDI();

    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

int CMD9(void)//SEND_CSD
{
    rSDICARG=RCA<<16;				// CMD9(RCA,stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x49;	// long_resp, wait_resp, start, CMD9

#if DEBUG
    Uart_Printf("\n****CSD register****\n");
#endif
    //-- Check end of CMD9
    if(!Chk_CMDend(9, 1)) 
	return 0;

#if DEBUG
    Uart_Printf(" SDIRSP0=0x%x\n SDIRSP1=0x%x\n SDIRSP2=0x%x\n SDIRSP3=0x%x\n", rSDIRSP0,rSDIRSP1,rSDIRSP2,rSDIRSP3);
#endif

    return 1;
}
//====================以上是命令实现===============================================================
void Set_1bit_bus(void)
{
    Wide=0;
    if(CARD_MODE==SD_MODE)
	SetBus();

#if DEBUG
    Uart_Printf("\n****1bit bus****\n");
#endif

}

void Set_4bit_bus(void)
{
    Wide=1;
    SetBus();

#if DEBUG    
    Uart_Printf("\n****4bit bus****\n");
#endif
    
}

void SetBus(void)
{
SET_BUS:
    CMD55();	// Make ACMD
    //-- CMD6 implement
    rSDICARG=Wide<<1;	    //Wide 0: 1bit, 1: 4bit
    rSDICCON=(0x1<<9)|(0x1<<8)|0x46;	//sht_resp, wait_resp, start, CMD55

    if(!Chk_CMDend(6, 1))   // ACMD6
	goto SET_BUS;
    //rSDICSTA=0xa00;	    // Clear cmd_end(with rsp)
}

void Set_Prt(void)
{
    //-- Set protection addr.0 ~ 262144(32*16*512) 
    Uart_Printf("[Set protection(addr.0 ~ 262144) test]\n");

RECMD28:
    //--Make ACMD
    rSDICARG=0;	    // CMD28(addr) 
    rSDICCON=(0x1<<9)|(0x1<<8)|0x5c;	//sht_resp, wait_resp, start, CMD28

    //-- Check end of CMD28
    if(!Chk_CMDend(28, 1)) 
	goto RECMD28;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
}

void Clr_Prt(void)
{
    //-- Clear protection addr.0 ~ 262144(32*16*512) 
#if DEBUG    
    Uart_Printf("[Clear protection(addr.0 ~ 262144) test]\n");
#endif

RECMD29:
    //--Make ACMD
    rSDICARG=0;	    // CMD29(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x5d;	//sht_resp, wait_resp, start, CMD29

    //-- Check end of CMD29
    if(!Chk_CMDend(29, 1)) 
	goto RECMD29;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
}

