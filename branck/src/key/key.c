#include "..\common\def.h"
#include "..\common\option.h"
#include "..\common\addr.h"
#include "..\common\isr.h"
#include "..\common\lib.h"

#include "..\fat16_sd\fat16_1.h"
#include "..\otp.h"
#include "key.h"

#define DEBUG  1

                
#define s2   ((rGPBDAT&2)>>1)      //GPB1   up
#define s3   ((rGPHDAT&2)>>1)      //GPH1   down
#define s4   (rGPHDAT&1)           //GPH0   shift

#define s1   ((rGPFDAT&2)>>1)           //GPF1   POWER
//#define s5   ((rGPCDAT&(1<<5))>>5) //GPC5  ent
#define  KEY_UP     s2
#define  KEY_DOWN   s3
#define  KEY_PASUE  s4

#define s    (0x7&((s4<<2)+(s3<<1)+s2))
//================================================
////----------------------------------------------------------------
#define  TEST  			0
#define  OTP   			1
//------------------------------------------------------
extern volatile U32 CurrentStatus;
extern volatile S32  H;

extern volatile U8 LcdShutDown;
extern volatile U8 Pause;

extern volatile U8 FlagEdit;

extern volatile char Position;
extern U8 Location_Table[6];
extern U8 OTP_Init[16];
extern CmdTable_Struct *LinkCurrent;
extern U8 RxBuffer[16];
//-******************************************
void Key_Intinit(void)
{  
  
  rGPBCON=rGPBCON&(~(3<<2));            //S2     (32)        up_key
  rGPHCON=rGPHCON&(~(0xf));             //S3,S4  (3210) down_key,shift_key
//rGPCCON=rGPCCON&(~(1<<10|1<<11));     //S5     (11 10)ent_key(not used)
 //------------以上是按键pin设定 ------ //都设定为in口 (10) 
 
  rGPFCON=(rGPFCON & ~(3<<0))|(2<<0);   //中断0设定:为 中断类型   
//  rGPFCON=(rGPFCON & ~(3<<2))|(2<<2);   //中断1设定:为 中断类型   

//pull up  disable
  rGPBUP=rGPBUP|(1<<1);
  rGPHUP=rGPHUP|(3<<0);
//  rGPCUP=rGPCUP&(~(1<<5));


  //中断设定 中断0,
  rEXTINT0=rEXTINT0&(~(0x7));       //中断0:设定为低电平触发方式,
  rINTMOD=rINTMOD&(~(0x1));         //设定为irq
  //中断1设定
  /*
  rEXTINT0=rEXTINT0&(~(0x7<<4));    //中断1:设定为低电平触发方式,
  rINTMOD=rINTMOD&(~(0x1<<1));      //设定为irq
  */      


  pISR_EINT0=(U32)Key_IntService;    //中断0地址
//  pISR_EINT1=(U32)Key_Powerserve;  //中断1地址
  
  
  ClearPending(BIT_EINT0);          //清中断0标志位
 // ClearPending(BIT_EINT1);        //清中断1
  
  

  rINTMSK &=(~BIT_EINT0);           //取消被掩盖
//  rINTMSK &=(~BIT_EINT1);  
  
#if 0
  Uart_Printf("am in key init end .\n"); 
#endif  

}
//*********************************************
//按键中断服务程序
//********************************************************
void __irq Key_IntService(void)    //设定为irq中断服务 
{

   unsigned int save;
    
   rINTMSK=(rINTMSK&BIT_EINT0);   // 屏蔽
   ClearPending(BIT_EINT0);       // 清中断
 
   
   Delay(1000);                   //delay(10) 近似为1ms  
   save=s;
   Delay(500);
   if(save!=s)goto end;
   Delay(500);
   if(save!=s)goto end;
   Delay(500);
   if(s==0x07)goto end;

if(save==s)
  {
  
#if 0  
 Uart_Printf("\n----in interrupt-------------\n");	   
 Uart_Printf("upkey is %d.\n",KEY_UP);
 Uart_Printf("downkey is %d \n",KEY_DOWN);
 Uart_Printf("pausekey is %d \n",KEY_PASUE);
 Uart_Printf("s is %d \n",s); 
 Uart_Printf("save is %d \n",save); 
 Uart_Printf("current status:%d \n",CurrentStatus);
#endif
  switch(save)
    {
      case 0x0:  //TEST/OTP
          {
      	   if(CurrentStatus==TEST)   //进入OTP状态
      	   		{
      	   		 CurrentStatus=OTP;
      	   		 Uart_Communication(OTP_Init);      	   		
      	   		 }
      	   else if(CurrentStatus==OTP)   //切换回测试状态
      	         {      	   		
      	   		  CurrentStatus=TEST;
      	   		 }
      	   break;
      	  } 
//***********对上面这种状态,在中断外解决*****************      	   
      case 0x1:  //LcdShutdown//edit
          {
           if(CurrentStatus==TEST)
           	  {
           		 if(LcdShutDown==0)   //关
           		   {     
			     	rCLKCON&=~(1<<5);
    			 	Lcd_EnvidOnOff(0);  
    			 	LcdShutDown=1;   		 
           		   }
      	         else				  //开
           		  {           		   
           		   rCLKCON|=(1<<5);
    			   Lcd_EnvidOnOff(1); 
    			   LcdShutDown=0; 
           		  }
              }	
           else	//在otp状态下是否进入编辑状态
               {
                  if(FlagEdit)FlagEdit=FALSE;
                  else
                      {
                       FlagEdit=TRUE;                                                      
                       Position=0;
                      }
               }   	 
           break;
          } 
      case 0x2:  //Pause//write otp
          {
           if(CurrentStatus==TEST)
           		{
           		  if(Pause)Pause=0;
           		  else  Pause=1;
      	   		}      	   
           break;
          }
	  case 0x3:  //KeyUp
	      {
      	   if(CurrentStatus==TEST)
      	   		{
      	   		 H--;
      	   		 if(H<0)H=9;
      	   		 }
      	   if(CurrentStatus==OTP)
      	   		{
      	   		 if(FlagEdit==FALSE)
      	   		    {
      	   		      LinkCurrent=LinkCurrent->up;
      	   		      link2char(LinkCurrent);
  					  Uart_Communication(RxBuffer);      	   		    
      	   		    }
      	   		  else   //若在编辑状态则
      	   		    {
      	   		
        	   		 RxBuffer[Location_Table[Position]]++;
    	  	   		 if(RxBuffer[Location_Table[Position]]==0x3a) 	//10->a
      		   		    RxBuffer[Location_Table[Position]]=0x41;
       	   			 if(RxBuffer[Location_Table[Position]]==0x47)   //16->0
      	   			    RxBuffer[Location_Table[Position]]=0x30;   
      	   			 Uart_Communication(RxBuffer);  //更新   
      	   		    }
      	   		}
      	   break;
      	   }
      case 0x4:  //KeyDown
           {
           if(CurrentStatus==TEST)
              {
      	   		 H++;
      	   		 if(H>9)H=0;
      	   	  }
      	   if(CurrentStatus==OTP)
      	       {
      	         if(FlagEdit==FALSE)
      	   		    {
      	   		      LinkCurrent=LinkCurrent->next;
      	   		      link2char(LinkCurrent);
  					  Uart_Communication(RxBuffer);     	   		    
      	   		    }
      	   		  else   //若在编辑状态则
      	   		    {
      	   		  	   RxBuffer[Location_Table[Position]]--;
      	   			 if(RxBuffer[Location_Table[Position]]<0x30) 	//-1->f
      	   			    RxBuffer[Location_Table[Position]]=0x46;
       	   			 if(RxBuffer[Location_Table[Position]]==0x40)   //a-1->9
      	   			    RxBuffer[Location_Table[Position]]=0x39; 
      	   			 Uart_Communication(RxBuffer);    //更新   
      	   			 }           	   		
      	   		}  	  
           break;
           }
      case 0x5:  //KeyRight
           {
           if(CurrentStatus==TEST)
              {
      	   		 H++;
      	   		 if(H>9)H=0;
      	   	  }
      	   if(CurrentStatus==OTP)
      	   	  {
      	   	    if(FlagEdit==FALSE)
      	   		    {
      	   		      LinkCurrent=LinkCurrent->next;
      	   		      link2char(LinkCurrent);
 					  Uart_Communication(RxBuffer);     	   		    
      	   		    }
      	   		  else   //若在编辑状态则
      	   		    {
      	   			 Position++;
      	   			 if(Position==6)Position=0;
      	   			 RxBuffer[15]=Location_Table[Position];
      	   			 Uart_Communication(RxBuffer); 
      	   			 }   		
      	   	  } 
           break;
           }
	  case 0x6:  //KeyLeft
	       {
      	   if(CurrentStatus==TEST)
      	      {
      	   		 H--;
      	   		 if(H<0)H=9;
      	   	  }
      	   if(CurrentStatus==OTP)
      	      {
      	      if(FlagEdit==FALSE)
      	   		    {
      	   		      LinkCurrent=LinkCurrent->up;
      	   		      link2char(LinkCurrent);
  					  Uart_Communication(RxBuffer);      	   		    
      	   		    }
      	   		else   //若在编辑状态则
      	   		    {
      	   			 Position--;
      	   		 	if(Position<0)Position=5;
      	   			 RxBuffer[15]=Location_Table[Position];
      	   			 Uart_Communication(RxBuffer); 
      	   			 }   		
      	   	  }
      	   break;
      	   }
	  default:       	   
      	   break;      	   
                      
     }//end of switch         
     
}     
     

end:
 rINTMSK=rINTMSK&~BIT_EINT0;	
 Delay(2000);  

#if 0 
 Uart_Printf("\n*********am end interrupt*************** \n");	   
 Uart_Printf("H value is %d\n",H);
 Uart_Printf("88upkey is %d.\n",KEY_UP);
 Uart_Printf("88downkey is %d \n",KEY_DOWN);
 Uart_Printf("88pausekey is %d \n",KEY_PASUE);   
 Uart_Printf("88current status:%d \n",CurrentStatus); 
#endif
 
}  

//====================================================================
/*
void __irq Key_Powerserve(void)    //设定为irq中断服务 
{
   rINTMSK=(rINTMSK&BIT_EINT1);  // 屏蔽该中断防止多次重入
   ClearPending(BIT_EINT1);      // 清中断标志
   
#if DEBUG
    Uart_Printf("s1 value:%d  \n",s1);   
#endif


   if(s1==0)
      Delay(1000);
   else 
   	  goto end;
        
   if(s1==0)
      Delay(1000);
   else 
      goto end;        
     					    //估计是因为powerkey通过3极管长生的低电位,恢复较慢

   
#if DEBUG
    Uart_Printf("am in key power service  \n");   
#endif

   if(LCDONOFF)
      {
	    rCLKCON&=~(1<<5);
    	Lcd_EnvidOnOff(0);
    	LCDONOFF=0;
    	Delay(10000);
#if DEBUG
   Uart_Printf("LCD off.\n");
#endif
    	
      }
   else
      {
      	rCLKCON|=(1<<5);
    	Lcd_EnvidOnOff(1);
    	LCDONOFF=1;        
    	Delay(10000);
    	  
#if DEBUG
   Uart_Printf("LCD on.\n");
#endif
    	
      }
      
end:
   Delay(10000);Delay(10000);
   rINTMSK=rINTMSK&~BIT_EINT1;	

}
*/