//====================================================================
// File Name : main.c
// Function  : �ṩ�û��ӿ�
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
volatile U32 CurrentStatus;			//�Ƿ����OTP

volatile S32  H;
S8 RxBuffer[16];
U8 OTP_Init[16]={'O','T','P','.','.','.','.','.','.','.','.','.','.','.','.','.'};

volatile S8 Position;//0~5
U8 Location_Table[6]={2,3,9,10,11,12};

volatile U8 LcdShutDown;
volatile U8 Pause;

volatile U8 FlagEdit;							//��Otp״̬��,�Ƿ�����˱༭״̬:1��,0��

Fat16BPB_Struct  Globe_BPB;
Fat16RtDirInfo_Struct Globe_RtDirInfo;
Fat16Dir_Struct Point_UpDir,Point_DownDir;

Fat16Dir_Struct OTP_File,Pic_Dir;                                            //otp�ļ�,��ͼƬ�ļ���

 U8 dir1[11]={'4',0x20,0x20,0x20,0x20,0x20,0x20,0x20,'3',0x20,0x20};
 
 U8 f1[11]={'0','0','1',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f2[11]={'0','0','2',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f3[11]={'0','0','3',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 U8 f4[11]={'0','0','4',0x20,0x20,0x20,0x20,0x20,'B','M','P'};
 //-----------------------------------
 U8 file5[11]={'O','T','P',0x20,0x20,0x20,0x20,0x20,'C','F','G'};              //otp.cfg�ļ���
 CmdTable_Struct *LinkHead,*LinkCurrent;                                                    //�����ļ��������ͷ
 
//===================================================================
void Main(void)
{
  CurrentStatus=TEST;
  LcdShutDown=0;    //����
  Pause=1;			//����ͣ,�������涯
  FlagEdit=0;
  H=0;
  
 
   MMU_Init();   //�˶α��������ǰ��
//-------system about----------------------------
   ChangeClockDivider(1,1);          // 1:2:4    
   ChangeMPllValue(0xa1,0x3,0x1);    // FCLK=202.8MHz     

//------------------------------------------------     
      Isr_Init();     
//------------------------------------------------      
      Port_Init();
      
           

//===================================================================
//�ô���0�뵥Ƭ��ͨ��
//--------------------------------------------

    Uart_Select(0);  
    Uart_Init(0,9600);    //���д��ڲ������趨Ϊ9600.

#if DEBUG              
    Uart_Printf("Uart0 is still OK.\n"); 
#endif
//---------touch screen test initial---------------
     Ts_Auto_initial();								//��������ʼ��
//--------------------------------------------
     Key_Intinit();									//�����жϳ�ʼ��
//------------------------------------------------     
     Lcd_Init();									//lcd��ʼ��
     Lcd_PowerEnable(0, 1);
     Lcd_EnvidOnOff(1);
//------------------------------------------------
  // if(SD_card_init())								//sd��ʶ���ʶ���ɹ�
  //     Uart_Printf("\n sd initial ok\n");
     SD_card_init();  
//--------------------------------------------------       
     Fat16_ReadBPB();							//��bpb��
     Fat16_RenewRtInfo();						//���¸�Ŀ¼,�������Ϣ.
/////////////////////////////////////////////////////////////     
      Fat16_FindDirAtRoot(dir1,&Point_DownDir); 
      Pic_Dir=Point_DownDir; 						//�ҵ�pic�ļ����ڴ�
      
      Fat16_FindDirAtRoot(file5,&Point_DownDir); 
      OTP_File=Point_DownDir;						 //�ҵ�OTP�ļ����ڴ�
           
//------------------------------------------------     
//	Test_SDI();
//    Read_BPB();
//------------------------------------------------    


//////////////////////////////////////////////////////////////////////////// 
//*************************************************************************//	
//********************����**********************************************//
//*************************************************************************//
rr:
 
#if DEBUG      
	Uart_Printf("am in rr .\n"); 
#endif

//------�����ж�״̬�Ĳ���---------------------------
 while(CurrentStatus==OTP)
   {

#if DEBUG   
     Uart_Printf("am in OTP .\n");
#endif

     OTP_Main();   
     while(1);

   }

//��test״̬
while(CurrentStatus==TEST)  
{


//�������VI���Բ���




//------------------------
    if(Pause==0)   //��������ͣ״̬,���Զ�����;
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

