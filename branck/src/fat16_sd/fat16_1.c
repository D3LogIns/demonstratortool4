#include "..\common\def.h"
#include "..\driver\lcd\lcdset.h"
#include "fat16_1.h"
#include "bmp.h"


#define DEBUG 0

extern  Fat16BPB_Struct  Globe_BPB;
extern  Fat16RtDirInfo_Struct Globe_RtDirInfo;
/*****************************************************
*����:  ��ȡFAT16��bpb��,�������(ȫ�ֱ���Globe_BPB)
*���:  Null
*����:  1�ɹ�,0ʧ��
*****************************************************/
BOOL Fat16_ReadBPB(void)
{		
  U8 readbuf[512],*point_u8;
  U32 *point_u32;

  point_u8=readbuf;
  point_u32=(U32 *)point_u8;
 
  SD_ReadBlock(0,1,point_u32);              
	 
  if((readbuf[0]!=0xeb)|(readbuf[510]!=0x55)|(readbuf[511]!=0xaa))	//����������־�ж�	 
	  {
#if 0
	   Uart_Printf("\n read bpb error \n");
#endif	   
	   return 0;
	   }
	   
	   
  Globe_BPB.BytesPerSec=(readbuf[11]|(readbuf[12]<<8));   //offset 11,length 2,ÿ�����ֽ���  
  Globe_BPB.SecPerClus =readbuf[13];               //offset 13,length 1,ÿ��������
  Globe_BPB.RsvdSecCnt =*((U16 *)&readbuf[14]);      //offset 14,length 2,�������б���������.
  Globe_BPB.NumFats    =readbuf[16];               //offset 16,length 1,fat��ķ���(2)
  Globe_BPB.RootEntCnt =(U16)(readbuf[17]|(readbuf[18]<<8));      //offset 17,length 2,��Ŀ¼Ŀ¼����(512)  
  Globe_BPB.TotSec16   =(U16)(readbuf[19]|(readbuf[20]<<8));      //offset 19,length 2,�ĸ����е���������(<0x10000)
  Globe_BPB.Media      =readbuf[21];               //offset 21,length 1,�ƶ���̶��洢����
  Globe_BPB.FatSz16    =*((U16 *)&readbuf[22]);      //offset 22,length 2,fat16һ��fat����ռ��������
  Globe_BPB.SecPerTrk  =*((U16 *)&readbuf[24]);      //offset 24,length 2,ÿ���ŵ���������
  Globe_BPB.NumHeads   =*((U16 *)&readbuf[26]);      //offset 26,length 2,��ͷ��
  Globe_BPB.HiddSec    =*((U32 *)&readbuf[28]);      //offset 28,length 4,fat����ǰ���ص�������
  Globe_BPB.TotSec32   =*((U32 *)&readbuf[32]);
  


#if 0
  Uart_Printf("\n============BPB  Information=========\n");
  Uart_Printf(" BytePerSec: %d  \n",Globe_BPB.BytesPerSec);
  Uart_Printf(" SecPerClus: %d  \n",Globe_BPB.SecPerClus);
  Uart_Printf(" RsvdSecCnt: %d  \n",Globe_BPB.RsvdSecCnt);
  Uart_Printf(" NumFats:    %d  \n",Globe_BPB.NumFats);
  Uart_Printf(" RootEntCnt: %d  \n",Globe_BPB.RootEntCnt);
  Uart_Printf(" TotSec16:   %d  \n",Globe_BPB.TotSec16);
  Uart_Printf(" Media:      %d  \n",Globe_BPB.Media);
  Uart_Printf(" FatSz16:    %d  \n",Globe_BPB.FatSz16);
  Uart_Printf(" SecPerTrk:  %d  \n",Globe_BPB.SecPerTrk);
  Uart_Printf(" NumHeads:   %d  \n",Globe_BPB.NumHeads);
  Uart_Printf(" HiddSec:    %d  \n",Globe_BPB.HiddSec);
  Uart_Printf(" TotSec32:   %d  \n",Globe_BPB.TotSec32);
#endif  
      return 1;
}

/*****************************************************
*����:  ����FAT16��bpb��,����RootDir��һЩ��Ϣ;
*���:  Null;       
*����:  1�ɹ�,0ʧ��
*��Ҫ��Ϣ:bpb��(ͨ��Globe_BPB)
*������Ϣ:��Ŀ¼����Ϣ(ͨ��Globe_RtDirInfo)
*****************************************************/
BOOL Fat16_RenewRtInfo(void)
{				
    Globe_RtDirInfo.RtDirStartSec=Globe_BPB.FatSz16*Globe_BPB.NumFats+Globe_BPB.RsvdSecCnt;                   //��Ŀ¼��ʼ����:243*2+2=488
    Globe_RtDirInfo.RtDirTotSec=((Globe_BPB.RootEntCnt*32)+(Globe_BPB.BytesPerSec-1))/Globe_BPB.BytesPerSec;  //��Ŀ¼��ռ������(32������):(512*32+(512-1))/512=32
    Globe_RtDirInfo.DataStartSec=Globe_RtDirInfo.RtDirStartSec +Globe_RtDirInfo.RtDirTotSec;				  //��������ʼ����:520;
 
 #if 0
  Uart_Printf("\n============== Root Area Information========\n");
  Uart_Printf("RtDirStartSec:%d\n",Globe_RtDirInfo.RtDirStartSec);
  Uart_Printf("RtDirTotSec:%d\n", Globe_RtDirInfo.RtDirTotSec);
  Uart_Printf("DataStartSec: %d  \n",Globe_RtDirInfo.DataStartSec);
#endif   
    
    return 1;
}
//-----------------------------------------------------------------	
//�ڸ�Ŀ¼�������ļ���,(����ҵ�һ���ļ���)	
//���Ҹ�Ŀ¼��������,1�ҵ�,0û�ҵ�
/*****************************************************
*����:  ���Ҹ�Ŀ¼�����ļ��������ļ���Ϊs[]���ļ�,
		���Ѹ�Ŀ¼������ݸ������Fat16Dir_Struct  *Point_ReturnDir;
*���:  U8 *s,Fat16Dir_Struct  *Point_ReturnDir
*����:  1�ɹ�,0ʧ��
*��Ҫ��Ϣ:��Ŀ¼��Ϣ(��Globe_RtDirInfo�ṩ)
*****************************************************/
BOOL Fat16_FindDirAtRoot(U8 *s,Fat16Dir_Struct  *Point_ReturnDir)  //sΪ�ļ�������,addrΪ����������ַ
{
		
	U32 i,j;//FirstDataSector,
	
	U8 readbuf[512],*point_u8;
	U32 *point_u32;
	
	point_u8=readbuf;
	point_u32=(U32 *)point_u8;
	
	//----------------------------------
   	for(i=0;i<Globe_RtDirInfo.RtDirTotSec;i++)                             //������ȡ��������
	    {
	    	SD_ReadBlock(Globe_RtDirInfo.RtDirStartSec+i,1,point_u32);       
        	for(j=0;j<16;j++)                              //һ��sec��512/32=16��Ŀ¼��
	         {
	         	*Point_ReturnDir=*((Fat16Dir_Struct *)&readbuf[j*32]);  //��Ŀ¼��ĸ�ʽ��ȡĿ¼������;
#if 0
  Uart_Printf(" s_Name:%s  \n",s);
  Uart_Printf(" Dir_Name:%s  \n",Point_ReturnDir->Dir_Name);
  Uart_Printf(" Dir_Attr:   	 %d  \n",Point_ReturnDir->Dir_Attr);
  Uart_Printf(" Dir_NtRes:       %d  \n",Point_ReturnDir->Dir_NtRes);
  Uart_Printf(" Dir_CrtTimeTeenth;%d\n",Point_ReturnDir->Dir_CrtTimeTeenth);
  Uart_Printf(" Dir_CrtTime; 	%d  \n",Point_ReturnDir->Dir_CrtTime);
  Uart_Printf(" Dir_CrtDate; 	%d  \n",Point_ReturnDir->Dir_CrtDate);
  Uart_Printf(" Dir_LastAccDate;%d  \n",Point_ReturnDir->Dir_LastAccDate);
  Uart_Printf(" Dir_FstClusHi;  %d  \n",Point_ReturnDir->Dir_FstClusHi);
  Uart_Printf(" Dir_WrtTime; 	%d  \n",Point_ReturnDir->Dir_WrtTime);
  Uart_Printf(" Dir_WrtDate; 	%d  \n",Point_ReturnDir->Dir_WrtDate);
  Uart_Printf(" Dir_FstClusLo; 	%d  \n",Point_ReturnDir->Dir_FstClusLo);
  Uart_Printf(" Dir_FileSize; 	%d  \n",Point_ReturnDir->Dir_FileSize);
#endif

	         	
	    	    if(s[0]==Point_ReturnDir->Dir_Name[0]&&
	 			   s[1]==Point_ReturnDir->Dir_Name[1]&&
	 	  		   s[2]==Point_ReturnDir->Dir_Name[2]&&
	 	  		   s[3]==Point_ReturnDir->Dir_Name[3]&&
	 	  		   s[4]==Point_ReturnDir->Dir_Name[4]&&
	 	  		   s[5]==Point_ReturnDir->Dir_Name[5]&&
	 	  		   s[6]==Point_ReturnDir->Dir_Name[6]&&
	 	  		   s[7]==Point_ReturnDir->Dir_Name[7]&&
	 	  		   s[8]==Point_ReturnDir->Dir_Name[8]&&
	 	  		   s[9]==Point_ReturnDir->Dir_Name[9]&&
	 	  		   s[10]==Point_ReturnDir->Dir_Name[10])	 	  		   
	 	  		    {	 	  		   
#if DEBUG	 	  	   					  
	 	  		       Uart_Printf("\n find it \n");
#endif	 	  		
	 	  		       return 1;							//�ҵ��ļ��з��� 	  		
	 	  		    }
	 	  		 if(Point_ReturnDir->Dir_Name[0]==0x00)
	 	  		    {
#if DEBUG
					    Uart_Printf("\n none after \n");
#endif	 	  		 
                        return 0;
                    }
                  if(Point_ReturnDir->Dir_Name[0]==0xe5)
	 	  		    {
#if DEBUG
					    Uart_Printf("\n NULL Dir \n");
#endif	 	  		 
					    continue;                       
                    }  
	 	  		 
	 	    }//end of 2nd for	 
	    	    	 	               	    	 	  	    	    	 	 	    	    	 	  	    	    	 	  
       }//end of 1st for
       
       return 0;
	    	  	
}



/*****************************************************
*����:  ������Ŀ¼�����ļ��������ļ���Ϊs[]���ļ�,
		�ϼ�Ŀ¼����ϢFat16Dir_Struct  *Point_UpDir(��������Ŀ¼���ڵĴغ�);
*���:  U8 *s,Fat16Dir_Struct  *Point_UpDir,��������Fat16Dir_Struct  *Point_DownDir
*����:  1�ɹ�,0ʧ��
*****************************************************/

BOOL Fat16_FindSonDir(U8 *s,Fat16Dir_Struct  *Point_UpDir,Fat16Dir_Struct  *Point_DownDir) 
{
	U32  DataStartAtClus;
	
	U32  i,j;
	
//-----���ݻ�����-----------------------	
	U8 readbuf[512],*point_u8;
	U32 *point_u32;
	
	point_u8=readbuf;
	point_u32=(U32 *)point_u8;
	
	//---------------------------------- 
    DataStartAtClus=(Point_UpDir->Dir_FstClusLo-2)*Globe_BPB.SecPerClus+Globe_RtDirInfo.DataStartSec;           //��������Ŀ¼���ڴ�clus�ĵ�һ������λ��    

	//----------------------------------
		
   	for(i=0;i<10;i++)                             //������ȡ��������10,���Դ���n*16���ļ����ļ�����
	    {
	    	SD_ReadBlock(DataStartAtClus+i,1,point_u32);       
        	for(j=0;j<16;j++)                              //һ��sec��512/32=16��Ŀ¼��
	         {
	         	*Point_DownDir=*((Fat16Dir_Struct *)&readbuf[j*32]);  //��Ŀ¼��ĸ�ʽ��ȡĿ¼������;���������ų�.��..�ļ��е�
#if 0
  Uart_Printf(" s_Name:%s  \n",s);
  Uart_Printf(" Dir_Name:%s  \n",Point_DownDir->Dir_Name);
#endif

	         	
	    	    if(s[0]==Point_DownDir->Dir_Name[0]&&
	 			   s[1]==Point_DownDir->Dir_Name[1]&&
	 	  		   s[2]==Point_DownDir->Dir_Name[2]&&
	 	  		   s[3]==Point_DownDir->Dir_Name[3]&&
	 	  		   s[4]==Point_DownDir->Dir_Name[4]&&
	 	  		   s[5]==Point_DownDir->Dir_Name[5]&&
	 	  		   s[6]==Point_DownDir->Dir_Name[6]&&
	 	  		   s[7]==Point_DownDir->Dir_Name[7]&&
	 	  		   s[8]==Point_DownDir->Dir_Name[8]&&
	 	  		   s[9]==Point_DownDir->Dir_Name[9]&&
	 	  		   s[10]==Point_DownDir->Dir_Name[10])	 	  		   
	 	  		    {	 	  		   
#if DEBUG	 	  	   					  
	 	  		       Uart_Printf("\n find it \n");
#endif	 	  		
	 	  		       return 1;							//�ҵ��ļ��з��� 	  		
	 	  		    }
	 	  		 if(Point_DownDir->Dir_Name[0]==0x00)
	 	  		    {
#if DEBUG
					    Uart_Printf("\n none after \n");
#endif	 	  		 
                       
                        return 0;
                    }
                  if(Point_DownDir->Dir_Name[0]==0xe5)
	 	  		    {
#if DEBUG
					    Uart_Printf("\n NULL Dir \n");
#endif	 	  		 
					    continue;                       
                    }  
	 	  		 
	 	    }//end of 2nd for	 
	    	    	 	               	    	 	  	    	    	 	 	    	    	 	  	    	    	 	  
       }//end of 1st for
       
       return 0;
	    	  	
}

/********************************************************
����:���ݸ����Ĵغ���fat16�ı����ҵ���һ���غ�;������һ�غ�,�������־
*********************************************************/
U16 Fat16_LookupFatTable(U16  FirstClus )
{
    U32 FatTableSec,OffsetAtSec; 
    U16 rt_value;   
//-----���ݻ�����-----------------------	
	U8 readbuf[512],*point_u8;
	U32 *point_u32;
	
	point_u8=readbuf;
	point_u32=(U32 *)point_u8;
//--------------------------------------
    FatTableSec=Globe_BPB.RsvdSecCnt+(FirstClus*2)/Globe_BPB.BytesPerSec;
    OffsetAtSec=(FirstClus*2)%Globe_BPB.BytesPerSec;
    
   SD_ReadBlock(FatTableSec,1,point_u32);
   rt_value=*((U16 *)&readbuf[OffsetAtSec]);    //OffsetAtSec��ȻΪż��
/*   
#if DEBUG
  Uart_Printf("\n===========fat table look up==================================\n");
  Uart_Printf(" FatTableSec:%d  \n",FatTableSec);
  Uart_Printf(" OffsetAtSec:%d  \n",OffsetAtSec);  
  Uart_Printf(" NextClus:%d  \n",rt_value);  
#endif  
*/
   return (rt_value);

}
/*****************************************************************************
display
******************************************************************************/
BOOL RenewLcdBuf(U8 *p)
{

 int i,j,k;
 U32 c;
 for(j=0;j<LCD_Y;j++)
     for(i=0;i<LCD_X;i++)
         {
          k=(LCD_Y-j)*LCD_X+i;
          c=(U32) (p[3*k]|(p[3*k+1]<<8)|(p[3*k+2]<<16));
		  frameBuffer[j][i]=((frameBuffer[j][i]&(0x0)) | c);   
		 }
 return 1;

}
/**************************************************************************************
*����:  ������Ŀ¼�����ļ��������ļ���Ϊs[]���ļ�,
		�ϼ�Ŀ¼����ϢFat16Dir_Struct  *Point_UpDir(��������Ŀ¼���ڵĴغ�);
*���:  U8 *s,Fat16Dir_Struct  *Point_UpDir,��������Fat16Dir_Struct  *Point_DownDir
*����:  1�ɹ�,0ʧ��
*****************************************************************************************/

BOOL Fat16_DispBmp(Fat16Dir_Struct  *Point_UpDir) 
{
	U32  DataStartAtClus;
	U32   *FileRamAddr=(U32 *)0x33000000;  //�õ�ַ��Ϊ�ļ�(�˴�ͼƬ)��ʱ��ſռ�,����Ҫ����Ҫռ��4M; 
	U32  FileLength=0;
	U8 * TogetherBuf;        //������ϵ��ڴ�ռ���ʼ��ַ
	U32  i;
	U16  Clus_Value,NextClus_Value;
	U32  TotClus;						//��������
	BmpHead_Struct BmpHead;
	
//-----���ݻ�����-----------------------	
	U8 ReadBuf[512],*point_u8;
	U32 *point_u32;
	
	point_u8=ReadBuf;
	point_u32=(U32 *)point_u8;
//===========================================================================	
    if(Point_UpDir->Dir_Name[8]!='B'||Point_UpDir->Dir_Name[9]!='M'||Point_UpDir->Dir_Name[10]!='P')
        {
#if DEBUG
        Uart_Printf("\n not bmp file  \n");
#endif  
         return 0;
        }
		
#if 0
  Uart_Printf(" File_Name:%s  \n",Point_UpDir->Dir_Name);
  Uart_Printf(" File_Size:%d  \n",Point_UpDir->Dir_FileSize);  
  Uart_Printf(" File_FstClusLo:%d  \n",Point_UpDir->Dir_FstClusLo); 
  Uart_Printf(" File_FstClusHi:%d  \n",Point_UpDir->Dir_FstClusHi); 
#endif

//===========���϶���ε���ȷ�����ж�=====================================
//---------------------------------- 
    DataStartAtClus=(Point_UpDir->Dir_FstClusLo-2)*Globe_BPB.SecPerClus+Globe_RtDirInfo.DataStartSec;           //��������Ŀ¼���ڴ�clus�ĵ�һ������λ��    
//----------------------------------
   SD_ReadBlock(DataStartAtClus,1,point_u32);
   BmpHead=*(BmpHead_Struct*)point_u8;
  
   if(BmpHead.BmpFlag!=('B'|('M'<<8)))
        {
#if DEBUG
        Uart_Printf("\n bmp head read error \n");
#endif  
         return 0;
        }   
   
  
#if DEBUG   
  Uart_Printf(" BMP_Flag  :%x  \n",BmpHead.BmpFlag);  
  Uart_Printf(" File_Size :%d  \n",BmpHead.FileSize);  
  Uart_Printf(" DataOffset:%d  \n",BmpHead.DataOffset);  
  Uart_Printf(" HeadSize  :%d  \n",BmpHead.HeadSize);
  Uart_Printf(" DataSize  :%d  \n",BmpHead.BmpDataSize); 
  Uart_Printf(" Width     :%d  \n",BmpHead.Width);
  Uart_Printf(" Height    :%d  \n",BmpHead.Height);    
  Uart_Printf(" bitperpix :%d  \n",BmpHead.Bpp);  
  Uart_Printf(" size of bmphead :%d  \n",sizeof(BmpHead_Struct));  
#endif
//�˴���Щ����ļٶ�:1)�ٶ�ͷ�ļ����ᳬ��512�ֽ�(��һ��sec)
//                   2)SecPerClusΪ2�������η�(�˴�Ϊ32)
//						���������ȡ;
 //---------------------------------         
  TotClus=BmpHead.BmpDataSize/(Globe_BPB.SecPerClus*Globe_BPB.BytesPerSec);//�������ܴ���,һ�����Ϊ32k
  
  SD_ReadBlock(DataStartAtClus+1,Globe_BPB.SecPerClus-1,FileRamAddr); //����ô�ʣ������  
  
  point_u32=&(FileRamAddr[(Globe_BPB.SecPerClus-1)*Globe_BPB.BytesPerSec/4]); //FileRamAddr�е�ǰ��ַ   
       
//=======================================================
  Clus_Value=Point_UpDir->Dir_FstClusLo;
  while(1)
  {
	NextClus_Value=Fat16_LookupFatTable(Clus_Value);
	Clus_Value=NextClus_Value;
//---------------------------------------	
	if(Clus_Value==0xfff7)
	   {
#if DEBUG
  Uart_Printf(" Point to a bad clus  \n");
#endif  
	   return 0;
	   }
//---------------------------------------	   
	if((Clus_Value>=0xfff0)&&(Clus_Value<=0xfff6))
	   {
#if DEBUG
  Uart_Printf(" Point to a reserved clus  \n");
#endif
	   return 0;
	   }	   
//---------------------------------------  
	if((Clus_Value>=0xfff8))
	   {
#if DEBUG
  Uart_Printf(" It's the last  clus  \n");
#endif
	   goto end;
	   }	   
//-----˵���������һ��,��ȡ��-----------------------------------  
   DataStartAtClus=(Clus_Value-2)*Globe_BPB.SecPerClus+Globe_RtDirInfo.DataStartSec;           //��������Ŀ¼���ڴ�clus�ĵ�һ������λ��     
 
   SD_ReadBlock(DataStartAtClus,Globe_BPB.SecPerClus,point_u32);  
   point_u32=point_u32+Globe_BPB.SecPerClus*Globe_BPB.BytesPerSec/4;//��ַָ���ƶ�
	   
  }	//end of while;
//=======================================================  
end:
 //---------------------------------
  TogetherBuf=(U8 *)malloc(BmpHead.BmpDataSize);        
  for(i=BmpHead.DataOffset;i<Globe_BPB.BytesPerSec;i++)     //���������ʣ���ֽ�
            TogetherBuf[FileLength++]=ReadBuf[i];

   point_u8=(U8 *)FileRamAddr;
   for(i=0;i<(BmpHead.BmpDataSize-(Globe_BPB.BytesPerSec-BmpHead.DataOffset));i++)  //�ڴ濽���ļ�ʣ���ֽ�
          TogetherBuf[FileLength++]=point_u8[i];
  

 RenewLcdBuf(TogetherBuf);
 
 free(TogetherBuf); 

  return 1;
	    	  	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**********************************************************************************************
*����:  �޸��Ѵ��ڵ��ļ�(���Ǵ���),��ʱ�ٶ�ֻ��һ���صĿռ���(���ļ���С<32k).����ֵ30k��7800h
         ������Ŀ¼�����ļ��������ļ���Ϊs[]���ļ�,
		�ϼ�Ŀ¼����ϢFat16Dir_Struct  *Point_UpDir(��������Ŀ¼���ڵĴغ�);
*���:  U8 *s,Fat16Dir_Struct  *Point_UpDir,��������Fat16Dir_Struct  *Point_DownDir
*����:  1�ɹ�,0ʧ��
***********************************************************************************************/

BOOL Fat16_WriteTxt(Fat16Dir_Struct  *Point_UpDir) 
{
	U32  DataStartAtClus;
	U32  *FileRamAddr;  			    //�õ�ַ��Ϊ�ļ�(�˴�txt)��ʱ��ſռ�,����Ҫ��չ; 
	U32  FileLength=0;
	
	U32  i;
	U16  Clus_Value;
	U32  TotSec;						//��������
	U8   c;							//��������
	CmdTable_Struct *LinkHead,*LinkUp,*LinkDown;          //��
	U8   n=0;								//������Ŀ
//-----���ݻ�����-----------------------	
	U8 ReadBuf[512],*point_u8;
	U32 *point_u32;
	
	point_u8=ReadBuf;
	point_u32=(U32 *)point_u8;
//===========================================================================	
    if(Point_UpDir->Dir_Name[8]!='T'||Point_UpDir->Dir_Name[9]!='X'||Point_UpDir->Dir_Name[10]!='T')
        {
#if DEBUG
        Uart_Printf("\n not txt file  \n");
#endif  
         return 0;
        }
		
    if(Point_UpDir->Dir_FileSize>0x7800)
        {
#if DEBUG
        Uart_Printf("\n too big files ,not fit for this version \n");
#endif  
         return 0;
        }
				
#if DEBUG
  Uart_Printf(" File_Name:%s  \n",Point_UpDir->Dir_Name);
  Uart_Printf(" File_Size:%d  \n",Point_UpDir->Dir_FileSize);  
  Uart_Printf(" File_FstClusLo:%d  \n",Point_UpDir->Dir_FstClusLo); 
  Uart_Printf(" File_FstClusHi:%d  \n",Point_UpDir->Dir_FstClusHi); 
#endif

//===========���϶���ε���ȷ�����ж�=====================================
//---------------------------------- 
  DataStartAtClus=(Point_UpDir->Dir_FstClusLo-2)*Globe_BPB.SecPerClus+Globe_RtDirInfo.DataStartSec;           //��������Ŀ¼���ڴ�clus�ĵ�һ������λ��    
//----------------------------------
  Clus_Value=Point_UpDir->Dir_FstClusLo;
  FileRamAddr=(U32 *)malloc(Point_UpDir->Dir_FileSize);
  point_u32=FileRamAddr;
  	   
//-----�˴���Ϊ�ٶ��ļ���С��һ����,�ʲ���̫���ж���---------------------------------  
   DataStartAtClus=(Clus_Value-2)*Globe_BPB.SecPerClus+Globe_RtDirInfo.DataStartSec;           //��������Ŀ¼���ڴ�clus�ĵ�һ������λ��     
 
   SD_ReadBlock(DataStartAtClus,Globe_BPB.SecPerClus,point_u32);            
//=======================================================  


#if DEBUG
   point_u8=(U8 *)FileRamAddr;
   Uart_Printf("\n--------befor--------------------\n");
   for(i=0;i<Point_UpDir->Dir_FileSize;i++)
	  Uart_Printf("%c",point_u8[i]);
#endif
	    
#if DEBUG

   LinkUp=(CmdTable_Struct *)malloc (sizeof(CmdTable_Struct));
   LinkHead=LinkUp;

   Uart_Printf("\n--------after--------------------\n");
   for(i=0;i<Point_UpDir->Dir_FileSize;i++)
       {	    
	    if(point_u8[i]=='#')
	         {	       
	          while((point_u8[i]!=0x0a))
	              i++;
	          continue;
	          }
	         
//�������һ����Ч�ı��Ļ�,���ֵĶ��ǿ����ַ�,0d,0a,������Ų���������û��ı���,���Դ�Ϊ���	              
//�ҵ��س���Ǻ�����;����һ��ͷ���¿�ʼ
         if(point_u8[i]=='R'&&point_u8[i+6]=='V')  //�ҵ���ʽ���         
             {
              n++;
             LinkUp->r    =(U8)((atoi(point_u8[i+2])<<4)|atoi(point_u8[i+3]));
             LinkUp->value=(U16)((atoi(point_u8[i+8])<<12)|(atoi(point_u8[i+9])<<8)|(atoi(point_u8[i+10])<<4)|(atoi(point_u8[i+11])));
	         LinkUp->next=(CmdTable_Struct *)malloc (sizeof(CmdTable_Struct));     
	         LinkUp=LinkUp->next;    

	         }
    
///////�˴����ٶ����в�����'#'�ַ�,���Լ�//////////	    	    
	   
	   }//end of for
	   
  LinkUp=LinkHead;
      	   
  for(i=0;i<n;i++)
     {

      Uart_Printf("R:%2xh  V:%4xh\n",LinkUp->r,LinkUp->value);
      LinkUp=LinkUp->next;
      }
      
      	   
#endif  
//�ͷſռ�
  LinkUp=LinkHead;
   for(i=0;i<=n;i++)
     {     
      LinkHead=LinkUp->next;
      free(LinkUp);
      LinkUp=LinkHead;
      
      }
 
 
 free(FileRamAddr); 

  return 1;
	    	  	
}