//====================================================================
// File Name : main.c
// Function  : 提供用户接口
// Program   :   (SOP)
// Date      : June 13, 2003
// Version   : 0.0
// History
//====================================================================
#include "includes.h"
//------------------------------------------------------------
#define DEBUG  0
////----------------------------------------------------------------
#define  TEST  			0
#define  OTP   			1
//--------------------------------------------
volatile U32 CurrentStatus;			//是否进入OTP

volatile S32  H;
S8 RxBuffer[16];
U8 OTP_Init[16]={'O','T','P','.','.','.','.','.','.','.','.','.','.','.','.','.'};

volatile S8 Position;//0~5
U8 Location_Table[6]={2,3,9,10,11,12};

volatile U8 LcdShutDown;
volatile U8 Pause;

volatile U8 FlagEdit;							//在Otp状态下,是否进入了编辑状态:1是,0否

Fat16BPB_Struct  Globe_BPB;
Fat16RtDirInfo_Struct Globe_RtDirInfo;
Fat16Dir_Struct Point_UpDir,Point_DownDir;

Fat16Dir_Struct OTP_File,Pic_Dir;                                            //otp文件,和图片文件夹

 U8 dir1[11]={'4',0x20,0x20,0x20,0x20,0x20,0x20,0x20,'3',0x20,0x20};
 
 U8 f1[11]={'0','0','1',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f2[11]={'0','0','2',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f3[11]={'0','0','3',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f4[11]={'0','0','4',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 //-----------------------------------
 U8 file5[11]={'O','T','P',0x20,0x20,0x20,0x20,0x20,'C','F','G'};              //otp.cfg文件名
 CmdTable_Struct *LinkHead,*LinkCurrent;                                                    //读出文件后链表的头
 
//===================================================================
void Main(void)
{
  CurrentStatus=TEST;
  LcdShutDown=0;    //不关
  Pause=1;			//有暂停,按键后画面动
  FlagEdit=0;
  H=0;
  
 
   MMU_Init();   //此段必须放在最前面
//-------system about----------------------------
   ChangeClockDivider(1,1);          // 1:2:4    
   ChangeMPllValue(0xa1,0x3,0x1);    // FCLK=202.8MHz     

//------------------------------------------------     
      Isr_Init();     
//------------------------------------------------      
      Port_Init();
      
           

//===================================================================
//用串口0与单片机通信
//--------------------------------------------

    Uart_Select(0);  
    Uart_Init(0,9600);    //所有串口波特率设定为9600.

#if DEBUG              
    Uart_Printf("Uart0 is still OK.\n"); 
#endif
//---------touch screen test initial---------------
     Ts_Auto_initial();								//触摸屏初始化
//--------------------------------------------
     Key_Intinit();									//按键中断初始化
//------------------------------------------------     
     Lcd_Init();									//lcd初始化
     Lcd_PowerEnable(0, 1);
     Lcd_EnvidOnOff(1);
//------------------------------------------------
  // if(SD_card_init())								//sd卡识别初识化成功
  //     Uart_Printf("\n sd initial ok\n");
     SD_card_init();  
//--------------------------------------------------       
     Fat16_ReadBPB();							//读bpb表
     Fat16_RenewRtInfo();						//更新根目录,及相关信息.
/////////////////////////////////////////////////////////////     
      Fat16_FindDirAtRoot(dir1,&Point_DownDir); 
      Pic_Dir=Point_DownDir; 						//找到pic文件所在簇
      
      Fat16_FindDirAtRoot(file5,&Point_DownDir); 
      OTP_File=Point_DownDir;						 //找到OTP文件所在簇
           
//------------------------------------------------     
//	Test_SDI();
//    Read_BPB();
//------------------------------------------------    


//////////////////////////////////////////////////////////////////////////// 
//*************************************************************************//	
//********************正文**********************************************//
//*************************************************************************//
rr:
 
#if DEBUG      
	Uart_Printf("am in rr .\n"); 
#endif

//------增加判断状态的部分---------------------------
 while(CurrentStatus==OTP)
   {

#if DEBUG   
     Uart_Printf("am in OTP .\n");
#endif

     OTP_Main();   
     while(1);

   }

//是test状态
while(CurrentStatus==TEST)  
{


//这儿加入VI测试部分




//------------------------
    if(Pause==0)   //若在无暂停状态,则自动播放;
         {
          H++; 
         // Delay(10000);  
         }
//------------------------         
     if(H>9)H=0;
     if(H<0)H=9;    
//-------------------------------------------------------
 switch(H)
    {
       
     case 0://red
          Glib_Color_Scr(0x00FF0000);
         break; 

     case 1://green
           Glib_Color_Scr(0x0000FF00);
           break;
     case 2://blue
          Glib_Color_Scr(0x000000FF); 
           break;
     case 3://white
        Glib_Color_Scr(0xFFFFFFFF);
           break;       
     case 4://black   
        Glib_Color_Scr(0x0);
          break;              
     case 5://64 gray step
           StepGray64();
           break;       
     case 6://qian niao
           QianNiao();
           break;  
          
     case 7://bmp @sd card
           Fat16_FindSonDir(f1,&Pic_Dir,&Point_DownDir);
           Fat16_DispBmp(&Point_DownDir);
           break;  
     case 8:
           Fat16_FindSonDir(f2,&Pic_Dir,&Point_DownDir);;          
           Fat16_DispBmp(&Point_DownDir);
           break;    
              
     case 9:
          Test_touchscreen();        
          break;                                                             
    }
}//END of while test status    
 goto rr;  

}

/////////////////////////////////////////////////////////////////////////////////

