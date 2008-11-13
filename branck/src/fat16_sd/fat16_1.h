#ifndef __FAT16_1_H__
#define __FAT16_1_H__

#ifdef __cplusplus
extern "C" {
#endif

//////////��ṹ����Ϊ�˹�����Ҫ//////////////////
typedef  struct __CommandTable{
    U8   r;
    U16  value;
    U8   n;
    struct __CommandTable *next;   //ָ����һ��
    struct __CommandTable *up;	   //ָ����һ��
    }CmdTable_Struct;


   ////-------------------------------------
typedef  struct __Fat16Dir_Struct{
	U8   Dir_Name[11];
	U8   Dir_Attr;
	U8   Dir_NtRes;
	U8   Dir_CrtTimeTeenth;
	U16  Dir_CrtTime;
	U16  Dir_CrtDate;
	U16  Dir_LastAccDate;
	U16  Dir_FstClusHi;
	U16  Dir_WrtTime;
	U16  Dir_WrtDate;
	U16  Dir_FstClusLo;
	U32  Dir_FileSize;
}Fat16Dir_Struct;							//��32�ֽ�

   
   //////////////////////////////////////
 typedef struct __Fat16BPB_Struct
  {
  	U16 BytesPerSec;      //offset 11,length 2,ÿ�����ֽ���
  	U8  SecPerClus;       //offset 13,length 1,ÿ��������
  	U16 RsvdSecCnt;       //offset 14,length 2,�������б���������.
  	U8  NumFats;          //offset 16,length 1,fat��ķ���(2)
  	U16 RootEntCnt;       //offset 17,length 2,��Ŀ¼Ŀ¼����(512)
  	U16 TotSec16;         //offset 19,length 2,�ĸ����е���������(<0x10000)
  	U8  Media;            //offset 21,length 1,�ƶ���̶��洢����
  	U16 FatSz16;          //offset 22,length 2,fat16һ��fat����ռ��������
  	U16 SecPerTrk;        //offset 24,length 2,ÿ���ŵ���������
  	U16 NumHeads;         //offset 26,length 2,��ͷ��
  	U32 HiddSec;          //offset 28,length 4,fat����ǰ���ص�������
  	U32 TotSec32;         //offset 32,length 4,��������,ͬTotSec16. 	
  }Fat16BPB_Struct;							//�ܳ�25bytes
   
 //////////////////////////////////////////////////////////
 typedef  struct __Fat16RtDirInfo_Struct
 {
    U32 RtDirStartSec;   
    U32 RtDirTotSec;
    U32 DataStartSec;								
 }Fat16RtDirInfo_Struct;							


 /////////////////////////////////////////////////////////  
   
BOOL Fat16_ReadBPB(void);
BOOL Fat16_RenewRtInfo(void);
//BOOL Fat16_FindDir(U8 *s,U32 *addr);  //sΪ�ļ�������,nΪ����������ַ
BOOL Fat16_FindDirAtRoot(U8 *s,Fat16Dir_Struct  *Point_RtDir);  //sΪ�ļ�������,addrΪ����������ַ

BOOL Fat16_FindSonDir(U8 *s,Fat16Dir_Struct  *Point_UpDir,Fat16Dir_Struct  *Point_DownDir) ;

BOOL Fat16_DispBmp(Fat16Dir_Struct  *Point_UpDir) ;
BOOL Fat16_ReadTxt(Fat16Dir_Struct  *Point_UpDir) ; 
#ifdef __cplusplus
}
#endif
#endif /*__FAT16_1_H__*/
